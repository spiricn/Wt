#ifndef WT_TERRAIN_H
#define WT_TERRAIN_H


#include "wt/TerrainChunk.h"
#include "wt/SceneActor.h"

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
			Buffer<Uint32> mIndices;
	};

	typedef std::vector<TerrainNode::Chunk*> ChunkList;

protected:
	TerrainNode* mParent;
	TerrainNode* mChildren;
	Uint32 mIndex;
	ChunkList mChunks;
	Uint32 mSize;
	physx::PxBounds3 mBounds;
	Gl::Batch* mBatch;

	void calcBounds(){
		mBounds.setEmpty();
		for(Uint32 i=0; i<mChunks.size(); i++){
			mBounds.include( mChunks[i]->mBounds );
		}
	}

	TerrainNode(TerrainNode* parent=NULL, Uint32 idx=0, Gl::Batch* geoBatch=NULL) : mParent(parent),
		mChildren(NULL), mIndex(idx), mSize(0), mBatch(geoBatch){

		if(parent != NULL){
			mSize = parent->mSize/2;

			for(Uint32 i=0; i<mSize; i++){
				for(Uint32 j=0; j<mSize; j++){

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
			for(Uint32 i=0; i<4; i++){
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

			for(Uint32 i=0; i<4; i++){
 				mChildren[i] = TerrainNode(this, i);
				mChildren[i].mBatch = mBatch;
			}

			mChunks.clear();

			for(Uint32 i=0; i<4; i++){
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

	TerrainNode* getChildren(){
		return mChildren;
	}

	void render(){
		mBatch->render(0,
			mChunks[0]->mIndices.getData(),
			mChunks[0]->mIndices.getCapacity()
			);
	}
};


struct TerrainDesc{
	Uint32 numRows;
	Uint32 numColumns;
	float rowScale;
	float columnScale;
	float heightScale;
	Image* texture1;
	Image* texture2;
	Image* texture3;
	Texture2D* textureMap;
	String heightmapPath;
	Uint32 collisionGroup;

	TerrainDesc() : collisionGroup(0x800000){
	}
};

class Terrain : public SceneActor{
public:
WT_DISALLOW_COPY(Terrain);

private:
	TerrainNode::ChunkList mChunks;
	TextureArray* mTerrainTextures;
	Gl::Batch mBatch;
	Uint32 mNumTriangles, mNumVertices, mNumXVertices, mNumZVertices;
	float mDepth, mWidth, mMinHeight, mMaxHeight; // bounding box
	float mHeightScale, mColumnScale, mRowScale;
	TerrainChunk::HeightMap	mHeightMap;
	Texture2D*				mTextureMap;
	TerrainNode mRootNode;
	TerrainDesc mDesc;

public:
	Terrain(Uint32 actorId);

	~Terrain();

	const TerrainDesc& getDesc() const;

	Uint32 getNumRows() const;

	Uint32 getNumCols() const;

	float getHeightScale() const;

	float getWidth() const{
		return mNumXVertices * mRowScale;
	}

	float getDepth() const{
		return mNumZVertices * mColumnScale;
	}

	
	float getHeightAt(const glm::vec2& coords) const{
		Uint32 row = (Uint32)(coords.x/mRowScale);
		Uint32 col = (Uint32)(coords.y/mColumnScale);

		return mHeightMap[row*mNumZVertices + col] * mHeightScale;
	}


	float getRowScale() const;

	float getColScale() const;

	TerrainChunk::HeightMap& getHeightmap();

	const TerrainChunk::HeightMap& getHeightmap() const;

	TerrainNode::ChunkList& getChunks();

	Gl::Batch& getBatch();

	void calculateNormals(TerrainChunk::Vertex* vertices, Uint32 startRow, Uint32 startCol, Uint32 numRows, Uint32 numCols);

	void create(const TerrainDesc& desc);

	Uint32 getTriangleIndex(Uint32 triangle);

	void editChunk(Buffer<Int16>& samples, Uint32 startRow, Uint32 startCol, Uint32 numRows, Uint32 numCols);

	Texture2D* getMapTexture();

	TextureArray* getTerrainTexture();

	TerrainNode* getRootNode();

}; // </Terrain>

}; // </wt>

#endif
