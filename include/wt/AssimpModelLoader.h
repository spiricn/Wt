#ifndef WT_ASSIMPMODELLOADER_H
#define WT_ASSIMPMODELLOADER_H


#include "wt/stdafx.h"

#include "wt/Singleton.h"
#include "wt/Exception.h"
#include "wt/Buffer.h"
#include "wt/Common.h"
#include "wt/Model.h"
#include "wt/AResourceLoader.h"

namespace wt{

//#pragma comment(lib, "assimp.lib")

#define DEFAULT_TICKS_PER_SEC 25.0f

class AssimpModelLoader : public Singleton<AssimpModelLoader>, public AResourceLoader<Model>{
public:

	typedef std::map<String, String> TextureMap;

	AssimpModelLoader(){
	}

	virtual ~AssimpModelLoader(){
	}

	void save(const String&, Model*){
		// Not implemented
	}

	static bool nodeTreeHasMesh(aiNode* root){
		if(root->mNumMeshes){
			return true;
		}
		else{
			for(uint32_t i=0; i<root->mNumChildren; i++){
				if(nodeTreeHasMesh(root->mChildren[i])){
					return true;
				}
			}
		}

		return false;
	}

	static aiNode* findSkeletonRoot(aiNode* node){
		if(!nodeTreeHasMesh(node)){
			return node;
		}
		else{
			for(uint32_t i=0; i<node->mNumChildren; i++){
				if(!nodeTreeHasMesh(node->mChildren[i])){
					return node->mChildren[i];
				}
			}
		}

		
		return NULL;
	}

	static void buildSkeleton(SkeletonBone* bone, aiNode* aiBone, uint32_t& idx){
		for(uint32_t i=0; i<aiBone->mNumChildren; i++){
			bone->addChild(
				aiBone->mChildren[i]->mName.data, idx++);
		}

		aiConvert(aiBone->mTransformation, bone->getTransform());

		for(uint32_t i=0; i<aiBone->mNumChildren; i++){
			buildSkeleton(bone->getChildAt(i), aiBone->mChildren[i], idx);
		}
	}

	static void buildSkeleton(Model& model, aiNode* aiRoot){
		if(aiRoot == NULL){
			LOGW("Skeleton root node not found");
			return;
		}

		SkeletonBone* root = model.createRootBone(aiRoot->mName.data);
		uint32_t idx=1;
		buildSkeleton(root, aiRoot, idx);
	}

	void load(const String& filePath, Model& model, Animation* ani, TextureMap* texMap=NULL){

		//
		bool invertAxis=false;

		Assimp::Importer importer;

		const aiScene* scene;

		scene = importer.ReadFile(filePath.c_str(),
			aiProcess_Triangulate|aiProcess_SortByPType|aiProcess_CalcTangentSpace);
		//importer.

		if(scene==NULL){
			WT_THROW("Error loading model file \"%s\" reason: \"%s\"",
				filePath.c_str(), importer.GetErrorString());
		}

		for(uint32_t i=0; i<scene->mNumMaterials; i++){
			Material mat;
			aiConvert(*scene->mMaterials[i], mat);
		}

		buildSkeleton(model, findSkeletonRoot(scene->mRootNode));

		uint32_t numIndices=0;
		uint32_t numVertices=0;

		for(uint32_t i=0; i<scene->mNumMeshes; i++){
			numVertices += scene->mMeshes[i]->mNumVertices;
			numIndices += scene->mMeshes[i]->mNumVertices*3;
		}


		model.setSize(numVertices, numIndices);

		for(uint32_t i=0; i<scene->mNumMeshes; i++){
			const aiMesh* mesh = scene->mMeshes[i];
			
			if(!mesh->HasNormals()){
				LOGW("Mesh normals not found.");
			}
	
			if(!mesh->HasTangentsAndBitangents()){
				LOGW("Mesh tangents not found.");
			}

			if(!mesh->HasTextureCoords(0)){
				LOG( "Mesh texture coordinates not found.");
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			Buffer<Geometry::Vertex> vertices(mesh->mNumVertices);
			Buffer<GLuint> indices(mesh->mNumFaces*3);

			// Copy indices
			for(uint32_t j=0; j<mesh->mNumFaces; j++){
				indices[j*3] = mesh->mFaces[j].mIndices[0];
				indices[j*3+1] =  mesh->mFaces[j].mIndices[1];
				indices[j*3+2] =  mesh->mFaces[j].mIndices[2];
			}

			for(uint32_t j=0; j<mesh->mNumVertices; j++){
				vertices[j].x = mesh->mVertices[j].x;
				vertices[j].y = invertAxis?mesh->mVertices[j].z:mesh->mVertices[j].y;
				vertices[j].z = invertAxis?mesh->mVertices[j].y:mesh->mVertices[j].z;

				Geometry::Vertex& v = vertices[j];

				// Vertex normal
				if(mesh->HasNormals()){
					vertices[j].nx = mesh->mNormals[j].x;
					vertices[j].ny = mesh->mNormals[j].y;
					vertices[j].nz = mesh->mNormals[j].z;
				}

				// Vertex tangents
				if(mesh->HasTangentsAndBitangents()){
					vertices[j].tx = mesh->mTangents[j].x;
					vertices[j].ty = mesh->mTangents[j].y;
					vertices[j].tz = mesh->mTangents[j].z;
				}

				// Texture coordinates
				if(mesh->HasTextureCoords(0)){
					vertices[j].s = mesh->mTextureCoords[0][j].x;
					vertices[j].t = mesh->mTextureCoords[0][j].y;
				}
	
			}


			// Mesh bones
			for(uint32_t j=0; j<mesh->mNumBones; j++){
				const aiBone* aBone = mesh->mBones[j];
				SkeletonBone* bone = model.getRootBone()->findChildByName(aBone->mName.data);

				WT_ASSERT(bone!=NULL, "Bone not found (skeleton build failed)");


				// Copy bone info & assign index
				aiConvert(aBone->mOffsetMatrix, bone->getOffset());

				// Set vertex weight & bone id attributes
				for(uint32_t k=0; k<aBone->mNumWeights; k++){
					const aiVertexWeight& aWeight = aBone->mWeights[k];
					vertices[aWeight.mVertexId].addBone(bone->getIndex(), aWeight.mWeight);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Create buffers & structures
			Geometry* modelMesh = model.addGeometry(
				mesh->mName.data,
				vertices, indices);

			// Material
			if(texMap){
				aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
				

				aiString path;
				mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
				texMap->insert(std::pair<String,String>(modelMesh->getName(), path.C_Str()));
			}
		}

		if(scene->mNumAnimations != 0 && ani != NULL){
			convertAnimation(scene->mAnimations[0], *ani);
		}
	}

	void load(const String& filePath, Model* model){
		load(filePath, *model, NULL);
	}

private:
	static void convertAnimation(const aiAnimation* aAnim, Animation& anim){
		anim.setDuration( aAnim->mDuration/(aAnim->mTicksPerSecond==0.0?DEFAULT_TICKS_PER_SEC:aAnim->mTicksPerSecond) );

		for(size_t i=0; i<aAnim->mNumChannels; i++){
			const aiNodeAnim* aNodeAnim = aAnim->mChannels[i];
			NodeAnimation& nodeAnim = *anim.addNodeAnimation();
			convertNodeAnim(aAnim, aNodeAnim, nodeAnim);
		}
	}

	static void convertNodeAnim(const aiAnimation* aAnim, const aiNodeAnim* aNodeAnim, NodeAnimation& nodeAnim){

		// Position keys
		for(uint32_t i=0; i<aNodeAnim->mNumPositionKeys; i++){
			const aiVectorKey& aPosKey = aNodeAnim->mPositionKeys[i];
			nodeAnim.getPositionKeys().push_back( NodeAnimation::PositionKey(aPosKey.mValue.x,
				aPosKey.mValue.y,
				aPosKey.mValue.z,
				aPosKey.mTime/(aAnim->mTicksPerSecond==0.0?DEFAULT_TICKS_PER_SEC:aAnim->mTicksPerSecond))
			);
		}

		// Rotation keys
		for(uint32_t i=0; i<aNodeAnim->mNumRotationKeys; i++){
			const aiQuatKey& aRotKey = aNodeAnim->mRotationKeys[i];
			nodeAnim.getRotationKeys().push_back( NodeAnimation::RotationKey(aRotKey.mValue.x,
				aRotKey.mValue.y,
				aRotKey.mValue.z,
				aRotKey.mValue.w,
				aRotKey.mTime/(aAnim->mTicksPerSecond==0.0?DEFAULT_TICKS_PER_SEC:aAnim->mTicksPerSecond))
			);
		}

		// Scaling keys
		for(uint32_t i=0; i<aNodeAnim->mNumScalingKeys; i++){
			const aiVectorKey& aScaleKey = aNodeAnim->mScalingKeys[i];
			
			nodeAnim.getScaleKeys().push_back( NodeAnimation::ScaleKey(aScaleKey.mValue.x,
				aScaleKey.mValue.y,
				aScaleKey.mValue.z,
				aScaleKey.mTime/(aAnim->mTicksPerSecond==0.0?DEFAULT_TICKS_PER_SEC:aAnim->mTicksPerSecond))
				);
		}

		nodeAnim.setTargetNode( aNodeAnim->mNodeName.data );
	}


	static void aiConvert(const aiMatrix4x4& src, math::Transform& dst){
		aiVector3D scaling,translation;
		aiQuaternion rotation;
		src.Decompose(scaling, rotation, translation);

		dst.setPosition(translation.x, translation.y, translation.z);
		dst.setScale(scaling.x, scaling.y, scaling.z);
		dst.setQuat(rotation.x, rotation.y, rotation.z, rotation.w);
	}

	static void aiConvert(const aiColor3D& src, Color& dst){
		dst.mRed = src.r;
		dst.mGreen = src.g;
		dst.mBlue = src.b;
	}

	static void aiConvert(const aiMaterial& src, Material& dst){
		aiString texPath;
		src.GetTexture(aiTextureType_DIFFUSE, 0,
			&texPath);

		aiColor3D aColor;
		Color color;
		int aInt;

		if(src.Get(AI_MATKEY_TWOSIDED, aInt) == aiReturn_SUCCESS){
			dst.setCullMode(aInt==1 ? eCULL_NONE : eCULL_COUNTERCLOCKWISE);
		}

		if(src.Get(AI_MATKEY_COLOR_AMBIENT, aColor) == aiReturn_SUCCESS){
			aiConvert(aColor, color);
			dst.setAmbient(color);
		}

		if(src.Get(AI_MATKEY_COLOR_DIFFUSE, aColor) == aiReturn_SUCCESS){
			aiConvert(aColor, color);
			dst.setDiffuse(color);
		}

		if(src.Get(AI_MATKEY_ENABLE_WIREFRAME, aInt) == aiReturn_SUCCESS){
			dst.setWireframe(aInt==1 ? true : false);
		}
	}

	static void aiConvert(const aiMatrix4x4& src, glm::mat4& dst){
		math::Transform t;
		aiConvert(src, t);

		t.getMatrix(dst);
	}

}; // </AssimpModelLoader>

}; // </wt>

#undef TAG

#endif
