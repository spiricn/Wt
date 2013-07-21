#ifndef WT_MODEL_H
#define WT_MODEL_H


#include "wt/AResourceManager.h"
#include "wt/Animation.h"
#include "wt/Geometry.h"
#include "wt/SkeletonBone.h"
#include "wt/Texture2D.h"
#include "wt/Sp.h"
#include "wt/SkeletalAnimation.h"
#include "wt/Material.h"

namespace wt{


class ModelManager;

class Model : public AResource<Model>{
friend class ModelManager;

protected:

	void setTextureManager(TextureManager* textureManager){
		mTextureManager = textureManager;
	}

	void setAnimationManager(AnimationManager* animationManager){
		mAnimationManager = animationManager;
	}

public:
	

	class GeometrySkin{
	friend class Model;

	private:
		String mName;

	public:
		struct Mesh{
			Geometry* geometry;
			Texture2D* texture;
			Texture2D* normalMap;
			Material material;
			String geometryName;

			Mesh(Geometry* geometry) : geometry(geometry), texture(NULL), normalMap(NULL){
			}
		}; // </GeometryTexture>

		typedef std::vector<Mesh> MeshList;

	private:
		Model* mModel;
		MeshList mMeshes;

	protected:

		void removeMesh(Geometry* geometry){
			for(MeshList::iterator i=mMeshes.begin(); i!=mMeshes.end(); i++){
				if((*i).geometry == geometry){
					mMeshes.erase(i);
					return;
				}
			}
		}

		Mesh* addMesh(Geometry* geometry){
			mMeshes.push_back( Mesh(geometry) );
			return &mMeshes[mMeshes.size()-1];
		}

	public:
		GeometrySkin(Model* model, const String& name) : mModel(model), mName(name){
			for(GeoList::iterator i=model->mGeometry.begin(); i!=model->mGeometry.end(); i++){
				addMesh(*i);
			}
		}

		Mesh* findMeshByName(const String& name){
			for(MeshList::iterator i=mMeshes.begin(); i!=mMeshes.end(); i++){
				if(name.compare(i->geometry->getName())==0){
					return &(*i);
				}
			}

			return NULL;
		}

		const String& getName() const{
			return mName;
		}

		Model* getModel(){
			return mModel;
		}

		MeshList& getMeshList(){
			return mMeshes;
		}
	}; // </GeometrySkin>

	typedef std::map<String, Sp<GeometrySkin>> SkinMap;
	typedef std::map<String, SkeletalAnimation*> AnimationMap;


public:
	WT_DISALLOW_COPY(Model);

	typedef std::vector<Geometry*> GeoList;

private:
	GeoList mGeometry;
	SkeletonBone* mRootBone;
	gl::Batch mBatch;
	uint32_t mVertexOffset, mIndexOffset;
	uint32_t mNumVertices, mNumIndices;
	String generateMeshName();
	SkinMap mSkins;
	AnimationMap mAnimations;
	TextureManager* mTextureManager;
	AnimationManager* mAnimationManager;
public:
	Model(AResourceManager<Model>* manager=NULL, ResourceHandle handle=0, const String& name="");

	~Model();

	SkinMap& getSkins();

	bool hasAnimation(const String& name) const{
		return mAnimations.find(name)!=mAnimations.end();
	}

	bool hasAnimation(const Animation* animation) const{
		for(AnimationMap::const_iterator i=mAnimations.cbegin(); i!=mAnimations.cend(); i++){
			if(i->second->getAnimation()->getHandle() == animation->getHandle()){
				return true;
			}
		}

		return false;
	}

	AnimationMap& getAnimations();

	SkeletalAnimation* getSkeletalAnimation(const String& name);

	SkeletalAnimation* addSkeletalAnimation(const String& name, Animation* source);

	bool hasSkeleton() const;

	SkeletonBone* getSkeleton();

	GeometrySkin* getSkin(const String& name);

	GeometrySkin* createSkin(const String& name);

	void deleteSkin(const String& name){
		deleteSkin( getSkin(name) );
	}

	void deleteSkin(GeometrySkin* skin){
		mSkins.erase( mSkins.find(skin->getName()) );
	}

	gl::Batch& getBatch();

	uint32_t getNumVertices() const;

	uint32_t getNumIndices() const;

	GeoList& getGeometry();

	SkeletonBone* createRootBone(const String& name);

	SkeletonBone* getRootBone();

	void release();

	Geometry* findGeometryByName(const String& name);
	
	void setSize(uint32_t numVertices, uint32_t numIndices);

	Geometry* addGeometry(const String& name, const Buffer<Geometry::Vertex>& vertices, 
		const Buffer<uint32_t>& indices);

	void removeGeometry(const String& name);

	void createHwBuffers(GLuint positionStreamIdx=0, GLuint texCoordStreamIdx=1,
		GLuint normalStreamIdx=2, GLuint tangentStreamIdx=3, GLuint boneWeightStreamIdx=5,  GLuint boneIdsStreamIdx=4);

	void create();

	void serialize(LuaPlus::LuaObject& dst);

	void deserialize(const LuaPlus::LuaObject& table);

}; // </Model>


typedef AResourceGroup<Model> ModelGroup;

}; // </wt>

#endif
