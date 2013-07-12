#include "Wt/stdafx.h"

#include "wt/Terrain.h"
#include "wt/TerrainChunk.h"
#include "wt/ASceneActor.h"
#include "wt/Physics.h"

#define TD_TRACE_TAG "Terrain"

namespace wt{


Terrain::Terrain(Scene* parent, uint32_t actorId, const String& name) : mTerrainTextures(NULL), ASceneActor(parent, ASceneActor::eTYPE_TERRAIN, actorId, name){
}

const TerrainDesc& Terrain::getDesc() const{
	return mDesc;
}

uint32_t Terrain::getNumRows() const{
	return mNumXVertices;
}

uint32_t Terrain::getNumCols() const{
	return mNumZVertices;
}

float Terrain::getHeightScale() const{
	return mHeightScale;
}

float Terrain::getRowScale() const{
	return mRowScale;
}

float Terrain::getColScale() const{
	return mColumnScale;
}

TerrainChunk::HeightMap& Terrain::getHeightmap(){
	return mHeightMap;
}

const TerrainChunk::HeightMap& Terrain::getHeightmap() const{
	return mHeightMap;
}

TerrainNode::ChunkList& Terrain::getChunks(){
	return mChunks;
}

Gl::Batch& Terrain::getBatch(){
	return mBatch;
}

void Terrain::calculateNormals(TerrainChunk::Vertex* vertices, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols){
	#define v(x, y) vertices[(x<0 ? 0 : x>=mNumXVertices ? mNumXVertices-1 : x)*mNumZVertices + (y<0 ? 0 : y>=mNumZVertices ? mNumZVertices-1 : y)]

	for(uint32_t row=startRow; row<startRow+numRows; row++){
		for(uint32_t col=startCol=0; col<startCol+numCols; col++){
			float dr = v(row-1, col).y - v(row+1, col).y;
			float dc = v(row, col-1).y - v(row, col+1).y;

			glm::vec3 n = glm::normalize( glm::vec3(-dr, 2*mRowScale, dc) );

			v(row, col).nx = n.x;
			v(row, col).ny = n.y;
			v(row, col).nz = n.z;
		}
	}

	#undef v
}

void Terrain::create(const TerrainDesc& desc){

	mDesc = desc;

	// textures
		
	mTerrainTextures = new TextureArray();
	mTerrainTextures->create(3,
		desc.texture1, // red channel
		desc.texture2, // green channel
		desc.texture3 // blue channel
		);

	mTextureMap = desc.textureMap;

	uint32_t numRows = desc.numRows;
	uint32_t numCols = desc.numColumns; 
	WT_ASSERT( math::isPowerOfTwo(numRows-1) && math::isPowerOfTwo(numCols-1),
		"Number of rows/columns must be power of two + 1 number");

	mRowScale = desc.rowScale;
	mColumnScale = desc.columnScale;
	mHeightScale = desc.heightScale;

	// make batch
	mWidth = (numRows-1)*mRowScale;
	mDepth = (numCols-1)*mColumnScale;


	//////////////////////////////////////////////////////////////////////

	mNumVertices = numRows*numCols;
	mNumTriangles = 2*(numRows-1)*(numCols-1);

	// Vertices
	TerrainChunk::Vertex* vertices = new TerrainChunk::Vertex[mNumVertices];
	TerrainChunk::Vertex* vertex;

	const char* chunkPath = desc.heightmapPath.c_str();
	std::ifstream inFile(chunkPath, std::ios::binary);
	WT_ASSERT(inFile.is_open(), "Error openning heightmap file \"%s\"", chunkPath);

	mHeightMap.create(numRows * numCols);

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
			int16_t sample;
			inFile.read((char*)&sample, 2);

			mHeightMap[row*numCols + col] = sample;
		}
	}

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
				

			vertex = &vertices[row*numCols + col];

			vertex->x = row*mRowScale;
			vertex->y = mHeightMap[row*numCols + col] * mHeightScale;
			vertex->z = col*mColumnScale;


			vertex->nx = 0;
			vertex->nz = 0;
			vertex->ny = 1;

			// colormap coordinates
			vertex->s1 = vertex->x/mWidth;
			vertex->t1 = -vertex->z/mDepth;

			// tiled texture coordinates
			vertex->s2 = (vertex->x + mWidth*3)/(mWidth);
			vertex->t2 = (vertex->z + mDepth*3)/(mDepth);

		}
	}

	mNumXVertices = numRows;
	mNumZVertices = numCols;

	calculateNormals(vertices, 0, 0, numRows, numCols);

	mBatch.create(	vertices, mNumVertices, sizeof(TerrainChunk::Vertex),
					0, 0, sizeof(GLuint),
					GL_TRIANGLES, GL_UNSIGNED_INT);

	// position
	mBatch.setVertexAttribute(0, 3, GL_FLOAT, offsetof(TerrainChunk::Vertex, x));
	// map tex coords
	mBatch.setVertexAttribute(1, 2, GL_FLOAT, offsetof(TerrainChunk::Vertex, s1));
	// tiled tex coords
	mBatch.setVertexAttribute(2, 2, GL_FLOAT, offsetof(TerrainChunk::Vertex, s2));
	// normals
	mBatch.setVertexAttribute(3, 3, GL_FLOAT, offsetof(TerrainChunk::Vertex, nx));

	// Build chunks (fixed size: 3 indices per 2 triangles)
	const uint32_t indicesPerQuad = 6;

	// Number of quads per chunk row/column
	uint32_t rowChunkSize = 32;
	uint32_t colChunkSize = 32;
		
	// Number of chunks per row/column
	uint32_t numChunksRow = (mNumXVertices-1)/rowChunkSize;
	uint32_t numChunksCol = (mNumZVertices-1)/colChunkSize;

	uint32_t quadsPerChunk = rowChunkSize*colChunkSize;

	// allocate chunks
	for(uint32_t i=0; i<numChunksRow * numChunksCol; i++){
		mChunks.push_back( new TerrainNode::Chunk() );
	}

	for(uint32_t chunkRow=0; chunkRow<numChunksRow; chunkRow++){
		for(uint32_t chunkCol=0; chunkCol<numChunksCol; chunkCol++){
			// current chunk
			TerrainNode::Chunk& chunk = *mChunks[chunkRow*numChunksCol + chunkCol];

			chunk.mIndices.create( quadsPerChunk*indicesPerQuad );


			for(uint32_t row=0; row<rowChunkSize; row++){
				for(uint32_t col=0; col<colChunkSize; col++){
					uint32_t quadRowOffset = (chunkRow*rowChunkSize) /* chunk base row offset */ + row /* offset within the chunk*/;
					uint32_t quadColOffset = (chunkCol*colChunkSize) + col;

					/* triangle 1
						1-2
						| /
						3
					*/
					chunk.mIndices.put(
						(quadRowOffset + 0)*mNumZVertices + (quadColOffset + 0)
						);
					chunk.mIndices.put(
						(quadRowOffset + 0)*mNumZVertices + (quadColOffset + 1)
						);
					chunk.mIndices.put(
						(quadRowOffset + 1)*mNumZVertices + (quadColOffset + 0)
						);

					/* triangle 2
							2
							/|
						1-3
					*/
					chunk.mIndices.put(
						(quadRowOffset + 1)*mNumZVertices + (quadColOffset + 0)
						);
					chunk.mIndices.put(
						(quadRowOffset + 0)*mNumZVertices + (quadColOffset + 1)
						);
					chunk.mIndices.put(
						(quadRowOffset + 1)*mNumZVertices + (quadColOffset + 1)
						);
				}
			}

				
			// calculate chunk's bounding box
			float minX = chunkRow*rowChunkSize*mRowScale;
			float maxX = ((chunkRow+1)*rowChunkSize)*mRowScale;

			float minZ = chunkCol*colChunkSize*mColumnScale;
			float maxZ = (chunkCol+1)*colChunkSize*mColumnScale;

			float minY=0;
			float maxY=0;

			for(uint32_t i=0; i<chunk.mIndices.getCapacity(); i++){
				uint32_t idx =  chunk.mIndices.getData()[i];
			
				TerrainChunk::Vertex& v = vertices[ idx ];
				if(v.y > maxY){
					maxY = v.y;
				}
				else if(v.y < minY){
					minY = v.y;
				}
			}

			chunk.mBounds = physx::PxBounds3(
				physx::PxVec3(minX, minY, minZ),
				physx::PxVec3(maxX, maxY, maxZ)
				);

		}
	}

	mRootNode.mSize = numChunksRow;
	for(uint32_t i=0; i<mChunks.size(); i++){
		mRootNode.mChunks.push_back(mChunks[i]);
	}

	mRootNode.calcBounds();
	mRootNode.mBatch = &mBatch;

	mRootNode.split();

	delete[] vertices;
}

uint32_t Terrain::getTriangleIndex(uint32_t triangle){
	uint32_t trianglesPerRow = (mNumZVertices-1)*2;
	uint32_t base,offset;

	if(triangle % 2 == 0){
		base = (triangle / trianglesPerRow) * mNumZVertices;
		offset = (triangle % trianglesPerRow)/2;
	}
	else{
		base = (triangle / trianglesPerRow + 1) * mNumZVertices;
		offset = (triangle % trianglesPerRow - 1)/2;
	}


	return base + offset;
}

void Terrain::editChunk(Buffer<int16_t>& samples, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols){
	// TODO safety checks

	physx::PxHeightFieldSample* pxSamples = new physx::PxHeightFieldSample[ numRows*numCols ];
	memset(pxSamples, 0x00, sizeof(physx::PxHeightFieldSample)*(numRows*numCols));

	physx::PxHeightFieldDesc desc;
	desc.format = physx::PxHeightFieldFormat::eS16_TM;
	desc.nbColumns = numCols;
	desc.nbRows = numRows;
	desc.samples.data = pxSamples;
	desc.samples.stride = sizeof(physx::PxHeightFieldSample);


	TerrainChunk::Vertex* vertices = (TerrainChunk::Vertex*)mBatch.getVertexBuffer().map(Gl::Buffer::eREAD_WRITE);

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
			int16_t sample = samples[row*numCols + col];

			// PhysX actor
			pxSamples[row*numCols + col].height = sample;

			// OpenGL batch
			vertices[(row+startRow)*mNumZVertices + (col+startCol)].y = mHeightScale*sample;

			// internal representation
			mHeightMap[(row+startRow)*mNumZVertices + (col+startCol)] = sample;
		}
	}

	physx::PxShape* shapes[1];
	((physx::PxRigidStatic*)getPhysicsActor()->getPxActor())->getShapes(shapes, 1);

	physx::PxHeightField* pxHeightField = shapes[0]->getGeometry().heightField().heightField;

	// edit PhysX actor
	pxHeightField->modifySamples(startCol, startRow, desc);

	delete[] pxSamples;

	// recalculate normals
	calculateNormals(vertices, 
		0, 0, mNumXVertices, mNumZVertices);

	// edit OpenGL batch
	mBatch.getVertexBuffer().unmap();
}

Texture2D* Terrain::getMapTexture() const{
	return mTextureMap;
} 

TextureArray* Terrain::getTerrainTexture() const{
	return mTerrainTextures;
}

const TerrainNode* Terrain::getRootNode() const{
	return &mRootNode;
}

Terrain::~Terrain(){
	for(uint32_t i=0; i<mChunks.size(); i++){
		delete mChunks[i];
	}
}

}; // </wt>
