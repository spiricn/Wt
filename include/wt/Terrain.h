#ifndef WT_TERRAIN_H
#define WT_TERRAIN_H


#include "wt/TerrainChunk.h"
#include "wt/ASceneActor.h"

#define LOGEX(tag, bnds) LOGI("center = {%f, %f, %f},  extents = {%f, %f, %f}", \
			bnds.getCenter().x, bnds.getCenter().y, bnds.getCenter().z, \
			bnds.getExtents().x, bnds.getExtents().y, bnds.getExtents().z);

namespace wt{

class Terrain;

class TerrainNode{

friend class Terrain;
public:
	struct Chunk{
			physx::PxBounds3 mBounds;
			Buffer<uint32_t> mIndices;
	};

	typedef std::vector<TerrainNode::Chunk*> ChunkList;

protected:
	TerrainNode* mParent;
	TerrainNode* mChildren;
	uint32_t mIndex;
	ChunkList mChunks;
	uint32_t mSize;
	physx::PxBounds3 mBounds;
	Gl::Batch* mBatch;

	void calcBounds(){
		mBounds.setEmpty();
		for(uint32_t i=0; i<mChunks.size(); i++){
			mBounds.include( mChunks[i]->mBounds );
		}
	}

	TerrainNode(TerrainNode* parent=NULL, uint32_t idx=0, Gl::Batch* geoBatch=NULL) : mParent(parent),
		mChildren(NULL), mIndex(idx), mSize(0), mBatch(geoBatch){

		if(parent != NULL){
			mSize = parent->mSize/2;

			for(uint32_t i=0; i<mSize; i++){
				for(uint32_t j=0; j<mSize; j++){

					// top left
					if(idx==0){
						mChunks.push_back(  parent->mChunks[ (i+0)*parent->mSize + (j+0) ] );
					}
					// top right
					else if(idx==1){
						mChunks.push_back(  parent->mChunks[ (i+0)*parent->mSize + (j+mSize) ] );
					}
					// botom right
					else if(idx==2){
						mChunks.push_back(  parent->mChunks[ (i+mSize)*parent->mSize + (j+mSize) ] );
					}
					// bottom left
					else{
						mChunks.push_back(  parent->mChunks[ (i+mSize)*parent->mSize + (j+0) ] );
					}
				}
			}


			calcBounds();
		}
	}

	~TerrainNode(){
		// TODO DESTROY
	}

	void destroy(){
		if(!mChildren != NULL){
			for(uint32_t i=0; i<4; i++){
				mChildren[i].destroy();
			}

			delete[] mChildren;
			mChildren=NULL;
		}
	}

	void split(){
		if(mChunks.size()>1){
			WT_ASSERT( mChunks.size() % 4 == 0, "Invalid chunk size %d", mChunks.size());
			//LOG("SPLIT");
			mChildren = new TerrainNode[4];

			for(uint32_t i=0; i<4; i++){
 				mChildren[i] = TerrainNode(this, i);
				mChildren[i].mBatch = mBatch;
			}

			mChunks.clear();

			for(uint32_t i=0; i<4; i++){
				mChildren[i].split();
			}
		}
	}

public:

	const physx::PxBounds3& getBounds() const{
		return mBounds;
	}

	bool isLeaf() const{
		return mChildren == NULL;
	}

	const TerrainNode* getChildren() const{
		return mChildren;
	}

	void render() const{
		mBatch->render(0,
			mChunks[0]->mIndices.getData(),
			mChunks[0]->mIndices.getCapacity()
			);
	}
};


struct TerrainDesc{
	uint32_t numRows;
	uint32_t numColumns;
	float rowScale;
	float columnScale;
	float heightScale;
	Image* texture1;
	Image* texture2;
	Image* texture3;
	Texture2D* textureMap;
	String heightmapPath;
	uint32_t collisionGroup;

	TerrainDesc() : collisionGroup(0x800000){
	}
};

class Terrain : public ASceneActor{
public:
WT_DISALLOW_COPY(Terrain);

private:
	TerrainNode::ChunkList mChunks;
	TextureArray* mTerrainTextures;
	Gl::Batch mBatch;
	uint32_t mNumTriangles, mNumVertices, mNumXVertices, mNumZVertices;
	float mDepth, mWidth, mMinHeight, mMaxHeight; // bounding box
	float mHeightScale, mColumnScale, mRowScale;
	TerrainChunk::HeightMap	mHeightMap;
	Texture2D*				mTextureMap;
	TerrainNode mRootNode;
	TerrainDesc mDesc;

public:
	Terrain(Scene* parent, uint32_t actorId, const String& name="");

	~Terrain();

	const TerrainDesc& getDesc() const;

	uint32_t getNumRows() const;

	uint32_t getNumCols() const;

	float getHeightScale() const;

	float getWidth() const{
		return mNumXVertices * mRowScale;
	}

	float getDepth() const{
		return mNumZVertices * mColumnScale;
	}

	
	float getHeightAt(const glm::vec2& coords) const{
		uint32_t row = (uint32_t)(coords.x/mRowScale);
		uint32_t col = (uint32_t)(coords.y/mColumnScale);

		return mHeightMap[row*mNumZVertices + col] * mHeightScale;
	}


	float getRowScale() const;

	float getColScale() const;

	TerrainChunk::HeightMap& getHeightmap();

	const TerrainChunk::HeightMap& getHeightmap() const;

	TerrainNode::ChunkList& getChunks();

	Gl::Batch& getBatch();

	void calculateNormals(TerrainChunk::Vertex* vertices, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols);

	void create(const TerrainDesc& desc);

	uint32_t getTriangleIndex(uint32_t triangle);

	void editChunk(Buffer<int16_t>& samples, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols);

	Texture2D* getMapTexture() const;

	TextureArray* getTerrainTexture() const;

	const TerrainNode* getRootNode() const;

}; // </Terrain>

}; // </wt>

#endif
