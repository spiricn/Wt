#ifndef WT_TERRAINNODE_H
#define WT_TERRAINNODE_H

#include "wt/Buffer.h"
#include "wt/GLBatch.h"
#include "wt/TextureArray.h"
#include "wt/Texture2D.h"

namespace wt
{

class Terrain;

class TerrainNode{
public:
	#pragma pack(push, 1) 
	typedef struct{
		float x,y,z;
		float nx,ny,nz;
		float s1,t1; // tile texture coords
		float s2,t2; // colormap texture coords
	} Vertex;
	#pragma pack(pop)

	const physx::PxBounds3& getBounds() const;

	bool isLeaf() const;

	const TerrainNode* getChildren() const;

	void render() const;

	void setTexArrays(TextureArray* textureArray, Texture2D* mapArray);

	Texture2D* getTexMap();

	TextureArray* getTextureArray();

private:
	struct Chunk{
			physx::PxBounds3 mBounds;
			Buffer<uint32_t> mIndices;
	}; // </Chunk>

	typedef std::vector<TerrainNode::Chunk*> ChunkList;

	void calcBounds();

	TerrainNode(TerrainNode* parent=NULL, uint32_t idx=0, gl::Batch* geoBatch=NULL);

	~TerrainNode();

	void destroy();

	void split();

private:
	TerrainNode* mParent;
	TerrainNode* mChildren;
	uint32_t mIndex;
	ChunkList mChunks;
	uint32_t mSize;
	physx::PxBounds3 mBounds;
	gl::Batch* mBatch;
	TextureArray* mTextureArray;
	Texture2D* mTextureMap;

private:

	friend class Terrain;
}; // </TerrainNode>

} // </wt>

#endif // </WT_TERRAINNODE_H>
