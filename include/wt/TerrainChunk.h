#ifndef WT_TERRAINCHUNK_H
#define WT_TERRAINCHUNK_H


#include "wt/stdafx.h"


#include "wt/GLBatch.h"
#include "wt/Texture.h"
#include "wt/TextureArray.h"
#include "wt/SceneActor.h"

namespace wt{

class TerrainChunk{
public:
	typedef Buffer<Int16> HeightMap;

	#pragma pack(push, 1) 
	typedef struct{
		float x,y,z;
		float nx,ny,nz;
		float s1,t1; // tile texture coords
		float s2,t2; // colormap texture coords
	} Vertex;
	#pragma pack(pop)

private:
	Gl::Batch				mBatch;
	Uint32					mNumTriangles, mNumVertices, mNumXVertices, mNumZVertices;
	float					mDepth, mWidth, mMinHeight, mMaxHeight; // bounding box
	HeightMap				mHeightMap;
	TextureArray*			mTextureArray;
	Texture2D*				mTextureMap;
	
	float mHeightScale, mColumnScale, mRowScale;

public:
	
	float getHeightScale() const{
		return mHeightScale;
	}

	glm::vec3 getPosAt(Uint32 row, Uint32 col){
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

	Gl::Batch& getBatch(){
		return mBatch;
	}

	void render(){
		mBatch.render();
	}


	HeightMap& getHeightMap(){
		return mHeightMap;
	}

	void loadHeightmap(std::ifstream& inFile, Uint32 stride, Uint32 numRows,
		Uint32 numColumns, float rowScale, float columnScale, float heightScale=1.0f);

	void setTexArrays(TextureArray* textureArray, Texture2D* mapArray);

	Texture2D* getTexMap();

	TextureArray* getTextureArray();

}; // </TerrainChunk>

}; // </wt>

#endif
