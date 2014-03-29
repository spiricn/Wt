#include "Wt/stdafx.h"

#include "wt/Terrain.h"
#include "wt/ASceneActor.h"
#include "wt/Physics.h"

#define TD_TRACE_TAG "Terrain"

namespace wt{


Terrain::Terrain(Scene* parent, uint32_t actorId, const String& name) : mTerrainTextures(NULL), ASceneActor(parent, ASceneActor::eTYPE_TERRAIN, actorId, name){
}

const Terrain::Desc& Terrain::getDesc() const{
	return mDesc;
}


ATransformable* Terrain::getTransformable() const{
	return const_cast<ATransformable*>( dynamic_cast<const ATransformable*>(&mTransform) );
}

Heightmap* Terrain::getHeightmap(){
	return mDesc.heightmap;
}

TerrainNode::ChunkList& Terrain::getChunks(){
	return mChunks;
}

gl::Batch& Terrain::getBatch(){
	return mBatch;
}

void Terrain::calculateNormals(TerrainNode::Vertex* vertices, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols){
	#define v(x, y) vertices[(x<0 ? 0 : x>=mNumRows ? mNumRows-1 : x)*mNumColumns + (y<0 ? 0 : y>=mNumColumns ? mNumColumns-1 : y)]

	for(uint32_t row=startRow; row<startRow+numRows; row++){
		for(uint32_t col=startCol=0; col<startCol+numCols; col++){
			float dr = v(row-1, col).y - v(row+1, col).y;
			float dc = v(row, col-1).y - v(row, col+1).y;

			glm::vec3 n = glm::normalize( glm::vec3(-dr, 2*mHeightmap->getRowScale(), dc) );

			v(row, col).nx = n.x;
			v(row, col).ny = n.y;
			v(row, col).nz = n.z;
		}
	}

	#undef v
}

void Terrain::create(const Terrain::Desc& desc){

	mDesc = desc;

	mHeightmap = desc.heightmap;
	// textures
		
	mTerrainTextures = new TextureArray();
	mTerrainTextures->create(3,
		desc.texture1, // red channel
		desc.texture2, // green channel
		desc.texture3 // blue channel
		);

	mTextureMap = desc.textureMap;

	uint32_t numRows = desc.heightmap->getNumRows();
	uint32_t numCols = desc.heightmap->getNumColumns(); 
	WT_ASSERT( math::isPowerOfTwo(numRows-1) && math::isPowerOfTwo(numCols-1),
		"Number of rows/columns must be power of two + 1 number");

	// make batch
	mWidth = (numRows-1)*mHeightmap->getRowScale();
	mDepth = (numCols-1)*mHeightmap->getColumnScale();

	mNumVertices = numRows*numCols;
	mNumTriangles = 2*(numRows-1)*(numCols-1);

	// Vertices
	TerrainNode::Vertex* vertices = new TerrainNode::Vertex[mNumVertices];
	TerrainNode::Vertex* vertex;

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
			vertex = &vertices[row*numCols + col];

			vertex->x = row * mHeightmap->getRowScale();
			vertex->y = mHeightmap->getSamples()[row*numCols + col] * mHeightmap->getHeightScale();
			vertex->z = col * mHeightmap->getColumnScale();

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

	mNumRows = numRows;
	mNumColumns = numCols;

	calculateNormals(vertices, 0, 0, numRows, numCols);

	mBatch.create(
		GL_TRIANGLES,
		vertices, mNumVertices, sizeof(TerrainNode::Vertex),
		0, 0, sizeof(GLuint),
		GL_UNSIGNED_INT
	);

	// position
	mBatch.setVertexAttribute(0, 3, GL_FLOAT, offsetof(TerrainNode::Vertex, x));
	// map tex coords
	mBatch.setVertexAttribute(1, 2, GL_FLOAT, offsetof(TerrainNode::Vertex, s1));
	// tiled tex coords
	mBatch.setVertexAttribute(2, 2, GL_FLOAT, offsetof(TerrainNode::Vertex, s2));
	// normals
	mBatch.setVertexAttribute(3, 3, GL_FLOAT, offsetof(TerrainNode::Vertex, nx));

	// Build chunks (fixed size: 3 indices per 2 triangles)
	const uint32_t indicesPerQuad = 6;

	// Number of quads per chunk row/column
	uint32_t rowChunkSize = 32;
	uint32_t colChunkSize = 32;
		
	// Number of chunks per row/column
	uint32_t numChunksRow = (mNumRows-1)/rowChunkSize;
	uint32_t numChunksCol = (mNumColumns-1)/colChunkSize;

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
						(quadRowOffset + 0)*mNumColumns + (quadColOffset + 0)
						);
					chunk.mIndices.put(
						(quadRowOffset + 0)*mNumColumns + (quadColOffset + 1)
						);
					chunk.mIndices.put(
						(quadRowOffset + 1)*mNumColumns + (quadColOffset + 0)
						);

					/* triangle 2
							2
							/|
						1-3
					*/
					chunk.mIndices.put(
						(quadRowOffset + 1)*mNumColumns + (quadColOffset + 0)
						);
					chunk.mIndices.put(
						(quadRowOffset + 0)*mNumColumns + (quadColOffset + 1)
						);
					chunk.mIndices.put(
						(quadRowOffset + 1)*mNumColumns + (quadColOffset + 1)
						);
				}
			}

				
			// calculate chunk's bounding box
			float minX = chunkRow*rowChunkSize*mHeightmap->getRowScale();
			float maxX = ((chunkRow+1)*rowChunkSize)*mHeightmap->getRowScale();

			float minZ = chunkCol*colChunkSize*mHeightmap->getRowScale();
			float maxZ = (chunkCol+1)*colChunkSize*mHeightmap->getColumnScale();

			float minY=0;
			float maxY=0;

			for(uint32_t i=0; i<chunk.mIndices.getCapacity(); i++){
				uint32_t idx =  chunk.mIndices.getData()[i];
			
				TerrainNode::Vertex& v = vertices[ idx ];
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
	uint32_t trianglesPerRow = (mNumColumns-1)*2;
	uint32_t base,offset;

	if(triangle % 2 == 0){
		base = (triangle / trianglesPerRow) * mNumColumns;
		offset = (triangle % trianglesPerRow)/2;
	}
	else{
		base = (triangle / trianglesPerRow + 1) * mNumColumns;
		offset = (triangle % trianglesPerRow - 1)/2;
	}


	return base + offset;
}

void Terrain::editChunk(Buffer<int16_t>& samples, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols){
	// TODO safety checks

	physx::PxHeightFieldSample* pxSamples = new physx::PxHeightFieldSample[ numRows*numCols ];
	memset(pxSamples, 0x00, sizeof(physx::PxHeightFieldSample)*(numRows*numCols));

	physx::PxHeightFieldDesc desc;
	desc.setToDefault();

	desc.format = physx::PxHeightFieldFormat::eS16_TM;
	desc.nbColumns = numCols;
	desc.nbRows = numRows;
	desc.samples.data = pxSamples;
	desc.samples.stride = sizeof(physx::PxHeightFieldSample);
	
	TerrainNode::Vertex* vertices = static_cast<TerrainNode::Vertex*>( mBatch.getVertexBuffer().map(gl::Buffer::eREAD_WRITE) );

	for(uint32_t row=0; row<numRows; row++){
		for(uint32_t col=0; col<numCols; col++){
			const uint32_t absRow = row+startRow;
			const uint32_t absCol = col+startCol;

			int16_t sample = samples[row*numCols + col];

			// PhysX actor
			pxSamples[row*numCols + col].height = sample;

			// OpenGL batch
			vertices[absRow * mNumColumns + absCol].y = mHeightmap->getHeightScale()*sample;

			// internal representation
			mHeightmap->getSamples()[absRow * mNumColumns + absCol] = sample;
		}
	}

	if(getPhysicsActor()){
		physx::PxShape* shapes[1];
		((physx::PxRigidStatic*)getPhysicsActor()->getPxActor())->getShapes(shapes, 1);

		physx::PxHeightField* pxHeightField = shapes[0]->getGeometry().heightField().heightField;

		// edit PhysX actor
		pxHeightField->modifySamples(startCol, startRow, desc);
	}

	delete[] pxSamples;

	// TODO doing this at every edit is REALLY slow, find a way to optimize it..
	// recalculate normals
	calculateNormals(vertices, 
		0, 0, mNumRows, mNumColumns);

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

void Terrain::getPhysicsDesc(PhysicsActor::Desc& desc){
	desc.type = PhysicsActor::eTYPE_STATIC;

	// TODO
	desc.group = 1;

	desc.geometryType = PhysicsActor::eGEOMETRY_HEIGHTMAP;

	desc.geometryDesc.heightfieldGeometry.heightScale = mHeightmap->getHeightScale();
	desc.geometryDesc.heightfieldGeometry.rowScale = mHeightmap->getRowScale();
	desc.geometryDesc.heightfieldGeometry.colScale = mHeightmap->getColumnScale();

	desc.geometryDesc.heightfieldGeometry.numRows = mHeightmap->getNumRows();
	desc.geometryDesc.heightfieldGeometry.numCols = mHeightmap->getNumColumns();

	// Create buffer

	desc.geometryDesc.heightfieldGeometry.heightmap = mHeightmap;
}

void Terrain::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	ASceneActor::deserialize(assets, src);

	Desc tDesc;

	String tex1, tex2, tex3, map, heightmap;

	WT_ASSERT(lua::luaConv(src.Get("tex1"), tex1), "Invalid terrain description");

	WT_ASSERT(lua::luaConv(src.Get("tex2"), tex2), "Invalid terrain description");

	WT_ASSERT(lua::luaConv(src.Get("tex3"), tex3), "Invalid terrain description");

	WT_ASSERT(lua::luaConv(src.Get("map"), map), "Invalid terrain description");

	WT_ASSERT(lua::luaConv(src.Get("heightmap"), heightmap), "Invalid terrain description");

	tDesc.texture1 = assets->getImageManager()->getFromPath( tex1 );
	WT_ASSERT(tDesc.texture1, "Missing terrain texture");

	tDesc.texture2 = assets->getImageManager()->getFromPath( tex2 );
	WT_ASSERT(tDesc.texture2, "Missing terrain texture");

	tDesc.texture3 = assets->getImageManager()->getFromPath( tex3 );
	WT_ASSERT(tDesc.texture3, "Missing terrain texture");

	tDesc.textureMap =  assets->getTextureManager()->getFromPath( map );
	WT_ASSERT(tDesc.textureMap, "Missing texture map");

	tDesc.heightmap = assets->getHeightmapManager()->getFromPath( heightmap );
	WT_ASSERT(tDesc.heightmap, "Missing heightmap \"%s\"", heightmap.c_str());

	create(tDesc);
}

void Terrain::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void*){
	ASceneActor::serialize(assets, dst);

	dst.Set("tex1", mDesc.texture1->getPath().c_str());

	dst.Set("tex2", mDesc.texture2->getPath().c_str());

	dst.Set("tex3", mDesc.texture3->getPath().c_str());

	dst.Set("map", mDesc.textureMap->getPath().c_str());

	dst.Set("heightmap", mDesc.heightmap->getPath().c_str());
}

float Terrain::getWidth() const{
	return mNumRows * mHeightmap->getRowScale();
}

float Terrain::getDepth() const{
	return mNumColumns * mHeightmap->getColumnScale();
}

	
float Terrain::getHeightAt(const glm::vec2& coords) const{
	const uint32_t row = (uint32_t)(coords.x/mHeightmap->getRowScale());

	const uint32_t col = (uint32_t)(coords.y/mHeightmap->getColumnScale());

	return mHeightmap->getSamples()[row*mNumColumns + col] * mHeightmap->getHeightScale();
}


}; // </wt>
