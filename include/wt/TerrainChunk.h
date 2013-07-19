#ifndef WT_TERRAINCHUNK_H
#define WT_TERRAINCHUNK_H


#include "wt/stdafx.h"


#include "wt/GLBatch.h"
#include "wt/Texture.h"
#include "wt/TextureArray.h"
#include "wt/ASceneActor.h"

namespace wt{

class TerrainChunk{
public:
	typedef Buffer<int16_t> HeightMap;

	#pragma pack(push, 1) 
	typedef struct{
		float x,y,z;
		float nx,ny,nz;
		float s1,t1; // tile texture coords
		float s2,t2; // colormap texture coords
	} Vertex;
	#pragma pack(pop)

private:
	gl::Batch				mBatch;
	uint32_t					mNumTriangles, mNumVertices, mNumXVertices, mNumZVertices;
	float					mDepth, mWidth, mMinHeight, mMaxHeight; // bounding box
	HeightMap				mHeightMap;
	TextureArray*			mTextureArray;
	Texture2D*				mTextureMap;
	
	float mHeightScale, mColumnScale, mRowScale;

public:
	
	float getHeightScale() const{
		return mHeightScale;
	}

	glm::vec3 getPosAt(uint32_t row, uint32_t col){
		return glm::vec3(
			row*mRowScale,
			mHeightMap[row*mNumZVertices + col]*mHeightScale,
			col*mColumnScale
			);
	}

	float getRowScale() const{
		return mRowScale;
	}

	float getColumnScale() const{
		return mColumnScale;
	}

	TerrainChunk();

	~TerrainChunk();

	gl::Batch& getBatch(){
		return mBatch;
	}

	void render(){
		mBatch.render();
	}


	HeightMap& getHeightMap(){
		return mHeightMap;
	}

	void loadHeightmap(std::ifstream& inFile, uint32_t stride, uint32_t numRows,
		uint32_t numColumns, float rowScale, float columnScale, float heightScale=1.0f);

	void setTexArrays(TextureArray* textureArray, Texture2D* mapArray);

	Texture2D* getTexMap();

	TextureArray* getTextureArray();

}; // </TerrainChunk>

}; // </wt>

#endif
