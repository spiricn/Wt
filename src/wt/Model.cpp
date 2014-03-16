#include "wt/stdafx.h"

#include "wt/Model.h"
#include "wt/GLBuffer.h"
#include "wt/Lua.h"

#define TD_TRACE_TAG "Model"

namespace wt{

Model::SkinMap& Model::getSkins(){
	return mSkins;
}

Model::AnimationMap& Model::getAnimations(){
	return mAnimations;
}

bool Model::hasAnimation(const String& name) const{
	return mAnimations.find(name)!=mAnimations.end();
}

bool Model::hasAnimation(const Animation* animation) const{
	for(AnimationMap::const_iterator i=mAnimations.cbegin(); i!=mAnimations.cend(); i++){
		if(i->second->getAnimation()->getHandle() == animation->getHandle()){
			return true;
		}
	}

	return false;
}

void Model::deleteSkin(const String& name){
	deleteSkin( getSkin(name) );
}

void Model::deleteSkin(ModelSkin* skin){
	mSkins.erase( mSkins.find(skin->getName()) );
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

ModelSkin* Model::getSkin(const String& name){
	SkinMap::iterator i=mSkins.find(name);
	return i==mSkins.end() ? NULL : i->second;
}

ModelSkin* Model::createSkin(const String& name){
	ModelSkin* res;

	WT_ASSERT(
		mSkins.find(name) == mSkins.end(), "Skin named \"%s\" already exists for model \"%s\"",
		name.c_str(), getName().c_str());

	mSkins.insert( std::make_pair(name, res = new ModelSkin(this, name)) );

	for(GeoList::iterator i=mGeometry.begin(); i!=mGeometry.end(); i++){
		res->createMesh(*i);
	}

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
		i->second->deleteMesh( i->second->findMeshByGeometry(*geometry) );
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

void Model::createHwBuffers(){
	// Allocate a hardware buffer
	mBatch.create(
		GL_TRIANGLES,
		NULL, mNumVertices, sizeof(Geometry::Vertex),
		NULL, mNumIndices, sizeof(GLuint),
		GL_UNSIGNED_INT
	);

	// Position stream
	mBatch.setVertexAttribute(eATTRIB_POSITION, 3, GL_FLOAT, offsetof(Geometry::Vertex, x));

	// Texture coordinate stream
	mBatch.setVertexAttribute(eATTRIB_TEXCOORD, 2, GL_FLOAT, offsetof(Geometry::Vertex, s));

	// Bbone IDs stream
	mBatch.setVertexAttribute(eATTRIB_BONE_ID, 4, GL_UNSIGNED_BYTE, offsetof(Geometry::Vertex, bones));

	// Normal stream
	mBatch.setVertexAttribute(eATTRIB_NORMAL, 3, GL_FLOAT, offsetof(Geometry::Vertex, nx));

	// Tangent stream
	mBatch.setVertexAttribute(eATTRIB_TANGENT, 3, GL_FLOAT, offsetof(Geometry::Vertex, tx));

	// Bone weight stream
	mBatch.setVertexAttribute(eATTRIB_BONE_WEIGHT, 4, GL_FLOAT, offsetof(Geometry::Vertex, weights));

	
	uint32_t vertexOffset=0;
	uint32_t indexOffset=0;

	// Copy each individual mesh data to hardware buffer
	for(GeoList::iterator i=mGeometry.begin(); i!=mGeometry.end(); i++){

		// Copy vertex data
		const Geometry::VertexBuffer& vertices = (*i)->getVertices();
		mBatch.getVertexBuffer().setSubData(vertexOffset*sizeof(Geometry::Vertex), vertices.getData(), vertices.getSize());

		// Copy offset index data
		Geometry::IndexBuffer& indices = (*i)->getIndices();
		for(uint32_t j=0; j<indices.getCapacity(); j++){
			indices[j] += indexOffset;
		}
		mBatch.getIndexBuffer().setSubData(indexOffset*sizeof(uint32_t),
			indices.getData(), indices.getSize());


		vertexOffset += vertices.getCapacity();
		indexOffset += indices.getCapacity();
	}
}


void Model::serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
	AResource::serialize(luaState, dst);

	// Skins
	lua::LuaObject skins = getManager()->getResourceSystem()->getLuastate()->newTable();

	dst.Set("skins", skins);
	for(SkinMap::iterator i=mSkins.begin(); i!=mSkins.end(); i++){
		// Create a skin table
		lua::LuaObject skin = getManager()->getResourceSystem()->getLuastate()->newTable();
		skins.Set(i->first.c_str(), skin);

		for(ModelSkin::MeshList::iterator j=i->second->getMeshListBeg(); j!=i->second->getMeshListEnd(); j++){
			ModelSkin::Mesh* mesh = *j;

			lua::LuaObject skinEntry = getManager()->getResourceSystem()->getLuastate()->newTable();

			skin.Set(mesh->geometry->getName().c_str(), skinEntry);

			skinEntry.Set("texture", mesh->texture ? mesh->texture->getPath().c_str() : "");

			skinEntry.Set("normal_map", mesh->normalMap? mesh->normalMap->getPath().c_str() : "");

			skinEntry.Set("alpha_tested", mesh->material.isAlphaTested());
		}
	}

	// Animations
	lua::LuaObject animations = getManager()->getResourceSystem()->getLuastate()->newTable();
	dst.Set("animations", animations);

	for(AnimationMap::iterator i=mAnimations.begin(); i!=mAnimations.end(); i++){
		animations.Set(i->first.c_str(), i->second->getAnimation()->getPath().c_str());
	}
}

void Model::deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
	AResource::deserialize(luaState, table);

	// Create skins
	LuaObject& luaSkins = table.Get("skins");

	if(luaSkins.IsTable()){
		for(LuaTableIterator skinIter(luaSkins); skinIter; skinIter.Next()){
			const char* skinName = skinIter.GetKey().ToString();
			LuaObject& luaSkin = skinIter.GetValue();

			WT_ASSERT(getSkin(skinName) == NULL, "Skin already exists; (model=%s, skin=%s)",
				getPath().c_str(), skinName);


			ModelSkin* skin = createSkin(skinName);

			// Iterate over all geometry components of the skin
			for(LuaTableIterator entryIter(luaSkin); entryIter; entryIter.Next()){
				const char* meshName = entryIter.GetKey().ToString();
				LuaObject& skinEntry = entryIter.GetValue();

				ModelSkin::Mesh* mesh = skin->createMesh(NULL);

				mesh->texture = getGroup()->getResourceSystem()->getTextureManager()->getFromPath(
					skinEntry.Get("texture").ToString()
					);

				mesh->normalMap = getGroup()->getResourceSystem()->getTextureManager()->getFromPath(
					skinEntry.Get("normal_map").ToString()
					);

				bool alphaTested = false;
				
				lua::luaConv(skinEntry.Get("alpha_tested"), alphaTested);

				mesh->material.setAlphaTest(alphaTested);

				mesh->geometryName = meshName;
			}
		}
	}

	// Animations
	LuaObject& luaAnims = table.Get("animations");
	if(luaAnims.IsTable()){

		for(LuaTableIterator animIter(luaAnims); animIter; animIter.Next()){
			const char* animName = animIter.GetKey().ToString();
			const char* animPath = animIter.GetValue().ToString();

			Animation* animation = getGroup()->getResourceSystem()->getAnimationManager()->getFromPath(animPath);
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
		ModelSkin* skin = i->second;

		for(ModelSkin::MeshList::iterator j=skin->getMeshListBeg(); j!=skin->getMeshListEnd(); j++){
			ModelSkin::Mesh* mesh = *j;

			// Alrady set ?
			if(mesh->geometry == NULL){
				mesh->geometry = findGeometryByName(mesh->geometryName);
				mesh->geometryName.clear();
			}
		}
	}
}

} // </wt>
