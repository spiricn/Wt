#include "wt/stdafx.h"

#include "wt/TerrainChunk.h"

#define TD_TRACE_TAG "TerrainChunk"

namespace wt{

void TerrainChunk::setTexArrays(TextureArray* textureArray, Texture2D* mapArray){
	mTextureArray=textureArray;
	mTextureMap=mapArray;
}

Texture2D* TerrainChunk::getTexMap(){
	return mTextureMap;
}

TextureArray* TerrainChunk::getTextureArray(){
	return mTextureArray;
}

TerrainChunk::TerrainChunk():mNumTriangles(0), mNumVertices(0), mDepth(0), mWidth(0){
}

TerrainChunk::~TerrainChunk(){
}

void TerrainChunk::loadHeightmap(std::ifstream& inFile, uint32_t stride, uint32_t numRows, uint32_t numCols, float dx, float dz, float heightOffset){
	mRowScale = dx;
	mColumnScale = dz;
	mHeightScale = heightOffset;

	mHeightMap.create( numRows*numCols );

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
			WT_ASSERT(inFile.good(), "In file not good, @ %d", inFile.tellg() );
			
			int16_t sample;
			inFile.read((char*)&sample, 2);
			mHeightMap[row*numCols + col] = sample;
		}

		inFile.seekg(stride, std::ios::cur);
	}


	// make batch
	mWidth = (numRows-1)*dx;
	mDepth = (numCols-1)*dz;


	//////////////////////////////////////////////////////////////////////
	mNumVertices = numRows*numCols;
	mNumTriangles = 2*(numRows-1)*(numCols-1);

	// Vertices
	TerrainChunk::Vertex* vertices = new TerrainChunk::Vertex[mNumVertices];
	TerrainChunk::Vertex* vertex;

	GLuint* indices = new GLuint[mNumTriangles*3];
	GLuint* index = indices;

	float scaleFactor =1.0f;// 50.0f/maxHeight;

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
			vertex = &vertices[row*numCols + col];

			vertex->x = row*dx;
			vertex->y = mHeightMap[row*numCols+ col]  * mHeightScale;
			vertex->z = col*dz;


			// TODO calculate dynamically
			vertex->nx = 0;
			vertex->nz = 0;
			vertex->ny = 1;

			// colormap coordinates
			vertex->s1 = vertex->x/mWidth;
			vertex->t1 = -vertex->z/mDepth;

			// tiled texture coordinates
			vertex->s2 = (vertex->x + mWidth*3)/(mWidth);
			vertex->t2 = (vertex->z + mDepth*3)/(mDepth);

			if(row==(numRows-1) || col==(numCols-1)){
				continue;
			}

			// Counter-clockwise wound for backface culling

			// Triangle 1
			index[0] = row*numCols + col;	
			index[1] = row*numCols + col+1;
			index[2] = (row+1)*numCols + col;

			// Triangle 2
			index[3] = (row+1)*numCols + col;
			index[4] = row*numCols+ col+1;
			index[5] = (row+1)*numCols + col+1;

			index += 6;
		}
	}

	mBatch.create(	vertices, mNumVertices, sizeof(TerrainChunk::Vertex),
					indices, mNumTriangles*3, sizeof(GLuint),
					GL_TRIANGLES, GL_UNSIGNED_INT);

	delete[] vertices;
	delete[] indices;

	// position
	mBatch.setVertexAttribute(0, 3, GL_FLOAT, offsetof(TerrainChunk::Vertex, x));
	// map tex coords
	mBatch.setVertexAttribute(1, 2, GL_FLOAT, offsetof(TerrainChunk::Vertex, s1));
	// tiled tex coords
	mBatch.setVertexAttribute(2, 2, GL_FLOAT, offsetof(TerrainChunk::Vertex, s2));
	// normals
	mBatch.setVertexAttribute(3, 3, GL_FLOAT, offsetof(TerrainChunk::Vertex, nx));

	mNumXVertices = numRows;
	mNumZVertices = numCols;
}


}; // </wt>
