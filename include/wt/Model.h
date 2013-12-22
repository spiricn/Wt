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
#include "ModelSkin.h"

namespace wt{


class ModelManager;

class Model : public AResource<Model>{
friend class ModelManager;

public:
	enum BufferAtribs{
		eATTRIB_INVALID=-1,

		eATTRIB_POSITION=0,
		eATTRIB_TEXCOORD,
		eATTRIB_NORMAL,
		eATTRIB_TANGENT,
		eATTRIB_BONE_ID,
		eATTRIB_BONE_WEIGHT
	}; // </BufferAtribs>

	typedef std::map<String, Sp<ModelSkin> > SkinMap;
	typedef std::map<String, SkeletalAnimation*> AnimationMap;

	WT_DISALLOW_COPY(Model);

	typedef std::vector<Geometry*> GeoList;


public:
	Model(AResourceManager<Model>* manager=NULL, ResourceHandle handle=0, const String& name="");

	~Model();

	SkinMap& getSkins();

	bool hasAnimation(const String& name) const;

	bool hasAnimation(const Animation* animation) const;

	AnimationMap& getAnimations();

	SkeletalAnimation* getSkeletalAnimation(const String& name);

	SkeletalAnimation* addSkeletalAnimation(const String& name, Animation* source);

	bool hasSkeleton() const;

	SkeletonBone* getSkeleton();

	ModelSkin* getSkin(const String& name);

	ModelSkin* createSkin(const String& name);

	void deleteSkin(const String& name);

	void deleteSkin(ModelSkin* skin);

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

	void createHwBuffers();

	void create();

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst);

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table);

private:
	GeoList mGeometry;
	SkeletonBone* mRootBone;
	gl::Batch mBatch;
	uint32_t mVertexOffset, mIndexOffset;
	uint32_t mNumVertices, mNumIndices;
	String generateMeshName();
	SkinMap mSkins;
	AnimationMap mAnimations;

}; // </Model>


typedef AResourceGroup<Model> ModelGroup;

}; // </wt>

#endif
