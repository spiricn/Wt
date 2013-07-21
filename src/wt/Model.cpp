#include "wt/stdafx.h"

#include "wt/Model.h"
#include "wt/GLBuffer.h"

#define TD_TRACE_TAG "Model"

namespace wt{

Model::SkinMap& Model::getSkins(){
	return mSkins;
}

Model::AnimationMap& Model::getAnimations(){
	return mAnimations;
}

SkeletalAnimation* Model::getSkeletalAnimation(const String& name){
	AnimationMap::iterator i = mAnimations.find(name);
	return i==mAnimations.end() ? NULL : i->second;
}

SkeletalAnimation* Model::addSkeletalAnimation(const String& name, Animation* source){
	WT_ASSERT(
		mAnimations.find(name) == mAnimations.end(), "Skeletal animation named \"%s\" already exists for model \"%s\"",
		name.c_str(), getName().c_str());

	SkeletalAnimation* ani;
	mAnimations.insert(
		std::make_pair(name, ani=new SkeletalAnimation(this, mRootBone, source))
		);
	return ani;
}

bool Model::hasSkeleton() const{
	return mRootBone != NULL;
}

SkeletonBone* Model::getSkeleton(){
	return mRootBone;
}

Model::GeometrySkin* Model::getSkin(const String& name){
	SkinMap::iterator i=mSkins.find(name);
	return i==mSkins.end() ? NULL : i->second;
}

Model::GeometrySkin* Model::createSkin(const String& name){
	GeometrySkin* res;

	WT_ASSERT(
		mSkins.find(name) == mSkins.end(), "Skin named \"%s\" already exists for model \"%s\"",
		name.c_str(), getName().c_str());

	mSkins.insert( std::make_pair(name, res = new GeometrySkin(this, name)) );

	return res;
}

gl::Batch& Model::getBatch(){
	return mBatch;
}

uint32_t Model::getNumVertices() const{
	return mNumVertices;
}

uint32_t Model::getNumIndices() const{
	return mNumVertices;
}

void Model::setSize(uint32_t numVertices, uint32_t numIndices){
	mNumVertices = numVertices;
	mNumIndices = numIndices;
}

void Model::removeGeometry(const String& name){
	GeoList::iterator geometry = mGeometry.end();

	for(GeoList::iterator i=mGeometry.begin(); i!=mGeometry.end(); i++){
		if((*i)->getName().compare(name)==0){
			geometry = i;
		}
	}

	if(geometry == mGeometry.end()){
		WT_THROW("No geometry named \"%s\" found", name.c_str());
		return;
	}

	for(SkinMap::iterator i=mSkins.begin(); i!=mSkins.end(); i++){
		i->second->removeMesh(*geometry);
	}

	mGeometry.erase(geometry);
}

Geometry* Model::addGeometry(const String& name, const Geometry::VertexBuffer& vertices, 
		const Geometry::IndexBuffer& indices){

	/* assign a unique name to the structure */
	String geoName = name;
	if(name.size()==0 || findGeometryByName(name) != NULL){
		geoName = generateMeshName();
	}

	uint32_t minIndex=0, maxIndex=0;
	for(uint32_t i=0; i<indices.getCapacity(); i++){
		minIndex = glm::min(minIndex, indices[i]);
		maxIndex = glm::max(maxIndex, indices[i]);
	}

	Geometry* geo = new Geometry(geoName, &mBatch,
		gl::SubBatch(minIndex, maxIndex, mIndexOffset, indices.getCapacity()),
		vertices, indices
		);



	mGeometry.push_back(geo);

	mVertexOffset += vertices.getCapacity();
	mIndexOffset += indices.getCapacity();

	return geo;
}


String Model::generateMeshName(){
	std::stringstream ss;

	int cnt=mGeometry.size();
	do{
		ss.clear();
		ss << "GEOMETRY_" << (++cnt);
		if(findGeometryByName(ss.str()) == NULL){
			return ss.str();
		}

	}while(true);
	return 0;
}

Model::GeoList& Model::getGeometry(){
	return mGeometry;
}

Model::Model(AResourceManager<Model>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name),
	mRootBone(NULL),  mVertexOffset(0), mIndexOffset(0){
}

SkeletonBone* Model::createRootBone(const String& name){
	return mRootBone = new SkeletonBone(NULL, name, 0);
}

SkeletonBone* Model::getRootBone(){
	return mRootBone;
}

Model::~Model(){
	release();
		
}

void Model::release(){
	for(GeoList::iterator i=mGeometry.begin(); i!=mGeometry.end(); i++){
		delete *i;
	}

	mGeometry.clear();

	if(mRootBone){
		mRootBone->destroy();
		delete mRootBone;
		mRootBone=NULL;
	}
}

Geometry* Model::findGeometryByName(const String& name){
	for(GeoList::iterator i=mGeometry.begin(); i!=mGeometry.end(); i++){
		if((*i)->getName().compare(name)==0){
			return *i;
		}
	}

	return NULL;
}

void Model::createHwBuffers(GLuint positionStreamIdx,
	GLuint texCoordStreamIdx, GLuint normalStreamIdx, GLuint tangentStream, GLuint boneWeightStreamIdx,  GLuint boneIdsStreamIdx){


	/* allocate a hardware buffer */
	mBatch.create(
		GL_TRIANGLES,
		NULL, mNumVertices, sizeof(Geometry::Vertex),
		NULL, mNumIndices, sizeof(GLuint),
		GL_UNSIGNED_INT
	);

	/* position stream */
	mBatch.setVertexAttribute(positionStreamIdx, 3, GL_FLOAT, offsetof(Geometry::Vertex, x));

	/* texture coordinate stream */
	mBatch.setVertexAttribute(texCoordStreamIdx, 2, GL_FLOAT, offsetof(Geometry::Vertex, s));

	/* bone IDs stream */
	mBatch.setVertexAttribute(boneIdsStreamIdx, 4, GL_UNSIGNED_BYTE, offsetof(Geometry::Vertex, bones));

	/* normal stream */
	mBatch.setVertexAttribute(normalStreamIdx, 3, GL_FLOAT, offsetof(Geometry::Vertex, nx));

	/* tangent stream */
	mBatch.setVertexAttribute(tangentStream, 3, GL_FLOAT, offsetof(Geometry::Vertex, tx));

	/* bone weight stream  */
	mBatch.setVertexAttribute(boneWeightStreamIdx, 4, GL_FLOAT, offsetof(Geometry::Vertex, weights));

	

	uint32_t vertexOffset=0;
	uint32_t indexOffset=0;

	/* copy every individual mesh data to hardware buffer */
	for(GeoList::iterator i=mGeometry.begin(); i!=mGeometry.end(); i++){

		const Geometry::VertexBuffer& vertices = (*i)->getVertices();
		Geometry::IndexBuffer& indices = (*i)->getIndices();

		for(uint32_t j=0; j<indices.getCapacity(); j++){
			indices[j] += indexOffset;
		}

		mBatch.getVertexBuffer().setSubData(vertexOffset*sizeof(Geometry::Vertex),
			vertices.getData(), vertices.getSize());

		mBatch.getIndexBuffer().setSubData(indexOffset*sizeof(uint32_t),
			indices.getData(), indices.getSize());

		vertexOffset += vertices.getCapacity();
		indexOffset += indices.getCapacity();
	}
}


void Model::serialize(LuaPlus::LuaObject& dst){
	AResource::serialize(dst);

	// skin
	Lua::LuaObject skins;
	LUA_NEW_TABLE(skins);
	dst.Set("skins", skins);
	for(SkinMap::iterator i=mSkins.begin(); i!=mSkins.end(); i++){

		Lua::LuaObject skin;
		LUA_NEW_TABLE(skin);
		skins.Set(
			i->first.c_str(),
			skin);

		for(GeometrySkin::MeshList::iterator j=i->second->getMeshList().begin(); j!=i->second->getMeshList().end(); j++){
			Lua::LuaObject skinEntry;
			LUA_NEW_TABLE(skinEntry);

			skin.Set(
				j->geometry->getName().c_str(),
				skinEntry);

			skinEntry.Set(
				"texture",
				j->texture ? j->texture->getPath().c_str() : "");

			skinEntry.Set(
				"normal_map",
				j->normalMap? j->normalMap->getPath().c_str() : "");
		}
	}

	// animations
	Lua::LuaObject animations;
	LUA_NEW_TABLE(animations);
	dst.Set("animations", animations);

	for(AnimationMap::iterator i=mAnimations.begin(); i!=mAnimations.end(); i++){
		animations.Set(
			i->first.c_str(),
			i->second->getAnimation()->getPath().c_str()
			);
	}
}

void Model::deserialize(const LuaPlus::LuaObject& table){
	AResource::deserialize(table);

	// skins
	LuaObject& luaSkins = table.Get("skins");

	if(luaSkins.IsTable()){

		for(LuaTableIterator skinIter(luaSkins); skinIter; skinIter.Next()){
			const char* skinName = skinIter.GetKey().ToString();
			LuaObject& luaSkin = skinIter.GetValue();
			GeometrySkin* skin = createSkin(skinName);

			for(LuaTableIterator entryIter(luaSkin); entryIter; entryIter.Next()){
				const char* meshName = entryIter.GetKey().ToString();
				LuaObject& skinEntry = entryIter.GetValue();

				GeometrySkin::Mesh* mesh = skin->addMesh(NULL);

				mesh->texture = mTextureManager->getFromPath(
					skinEntry.Get("texture").ToString()
					);

				mesh->normalMap = mTextureManager->getFromPath(
					skinEntry.Get("normal_map").ToString()
					);

				mesh->geometryName = meshName;
			}
		}
	}

	// animations
	LuaObject& luaAnims = table.Get("animations");
	if(luaAnims.IsTable()){

		for(LuaTableIterator animIter(luaAnims); animIter; animIter.Next()){
			const char* animName = animIter.GetKey().ToString();
			const char* animPath = animIter.GetValue().ToString();

			Animation* animation = mAnimationManager->getFromPath(animPath);
			WT_ASSERT(animation, "No animation \"%s\"", animPath);

			addSkeletalAnimation(animName, animation);
		}
	}
}

void Model::create(){
	createHwBuffers();

	// create animations
	for(AnimationMap::iterator i=mAnimations.begin(); i!=mAnimations.end(); i++){
		// TODO terrible hack, fix this
		i->second->setSource(
			this, mRootBone, i->second->getAnimation());
		i->second->create();
	}

	
	// create skins
	for(SkinMap::iterator i=mSkins.begin(); i!=mSkins.end(); i++){
		GeometrySkin* skin = i->second;

		for(GeometrySkin::MeshList::iterator j=skin->getMeshList().begin(); j!=skin->getMeshList().end(); j++){

			j->geometry = findGeometryByName(j->geometryName);
			j->geometryName.clear();
		}
	}
}

} // </wt>
