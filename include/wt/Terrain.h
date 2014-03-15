#ifndef WT_TERRAIN_H
#define WT_TERRAIN_H

//#include "wt/TerrainChunk.h"
#include "wt/ASceneActor.h"
#include "wt/PhysicsActor.h"
#include "wt/TerrainNode.h"
#include "wt/Heightmap.h"

namespace wt
{

class Terrain : public ASceneActor{
public:
WT_DISALLOW_COPY(Terrain);

	struct Desc{
		Image* texture1;
		Image* texture2;
		Image* texture3;
		Texture2D* textureMap;
		Heightmap* heightmap;
		uint32_t collisionGroup;

		Desc() : collisionGroup(0x800000){
		}
	}; // </Desc>

public:
	Terrain(Scene* parent, uint32_t actorId, const String& name="");

	~Terrain();

	ATransformable* getTransformable() const;

	void getPhysicsDesc(PhysicsActor::Desc& desc);

	const Desc& getDesc() const;

	float getWidth() const;

	float getDepth() const;

	float getHeightAt(const glm::vec2& coords) const;

	Heightmap* getHeightmap();

	TerrainNode::ChunkList& getChunks();

	gl::Batch& getBatch();

	void calculateNormals(TerrainNode::Vertex* vertices, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols);

	void create(const Desc& desc);

	uint32_t getTriangleIndex(uint32_t triangle);

	void editChunk(Buffer<int16_t>& samples, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols);

	Texture2D* getMapTexture() const;

	TextureArray* getTerrainTexture() const;

	const TerrainNode* getRootNode() const;

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque=NULL);

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void*);

private:
	TerrainNode::ChunkList mChunks;
	TextureArray* mTerrainTextures;
	gl::Batch mBatch;
	Heightmap* mHeightmap;
	uint32_t mNumTriangles;
	uint32_t mNumVertices;
	uint32_t mNumRows;
	uint32_t mNumColumns;
	float mDepth;
	float mWidth;
	float mMinHeight;
	float mMaxHeight;
	Texture2D* mTextureMap;
	TerrainNode mRootNode;
	Desc mDesc;
	math::Transform mTransform;
}; // </Terrain>

}; // </wt>

#endif
