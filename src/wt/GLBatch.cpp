#include "wt/stdafx.h"
#include "wt/GLBatch.h"

#include "wt/GLBuffer.h"
#include "wt/Exception.h"
#include "wt/VertexArray.h"

#define TD_TRACE_TAG "GLBatch"

namespace wt{

namespace Gl{


void Batch::bind(){
	mVertexArray->bind();
}

Batch::Batch() : mVertexBuffer(NULL), mIndexBuffer(NULL),
		mVertexArray(NULL),
		mIndexBufferOwned(true), mNumVertices(0),
		mNumIndices(0), mVertexSize(0), mIndexSize(0), mIndexType(0),
		mPrimitive(0), mIsInitialized(false){
	}

void Batch::render(const SubBatch& batch){
#ifdef WT_CHECKED
	/* SubBatch range check */
	WT_ASSERT(batch.mStartIndex + batch.mNumIndices <= mNumIndices,
		"SubBatch index out of range (%d + %d > %d",
		batch.mStartIndex, batch.mNumIndices, mNumIndices);

	/* Initialization check */
	WT_ASSERT(mIsInitialized, "Batch not initialized (call create before attempting to render)");
#endif

	glDrawRangeElements(
		mPrimitive,
		batch.mMinIndex,
		batch.mMaxIndex,
		batch.mNumIndices,
		mIndexType, 
		(const GLvoid*)(batch.mStartIndex*mIndexSize)
	);
}

Buffer& Batch::getVertexBuffer(){
	// TODO check
	return *mVertexBuffer;
}

Buffer& Batch::getIndexBuffer(){
	// TODO check
	return *mIndexBuffer;
}

GLuint Batch::getVertexSize(){
	return mVertexSize;
}

GLuint Batch::getIndexSize(){
	return mIndexSize;
}

Batch::~Batch(){
	destroy();
}
	
Uint32 Batch::getIndexAt(Uint32 index) const{
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumIndices){
		WT_THROW("Index buffer index out of range (index=%d size=%d)", index, mNumIndices);
	}

	Uint32 res;
	mIndexBuffer->getSubData(index*mIndexSize, &res, mIndexSize);
	return res;
}

void Batch::getVertexAt(Uint32 index, void* vertex) const{
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumVertices){
		WT_THROW("Vertex buffer index out of range (index=%d size=%d)", index, mNumVertices);
	}

	mVertexBuffer->getSubData(index*mVertexSize, vertex, mVertexSize);
}

void Batch::setIndexAt(Uint32 index, Uint32 value){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumIndices){
		WT_THROW("Index buffer index out of range (index=%d size=%d)", index, mNumIndices);
	}

	mIndexBuffer->setSubData(index*mIndexSize, &value, mIndexSize);
}

void Batch::setVertexAt(Uint32 index, const void* vertex){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumVertices){
		WT_THROW("Vertex buffer index out of range (index=%d size=%d)", index, mNumVertices);
	}

	mVertexBuffer->setSubData(index*mVertexSize, vertex, mVertexSize);
}

GLuint Batch::getNumVertices() const{
	return mNumVertices;
}

GLuint Batch::getNumIndices() const{
	return mNumIndices;
}

void Batch::unmapIndexBuffer(){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	mIndexBuffer->unmap();
}

void Batch::unmapVertexBuffer(){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	mVertexBuffer->unmap();
}

void Batch::destroy(){
	delete mIndexBuffer;
	mIndexBuffer=NULL;

	delete mVertexBuffer;
	mVertexBuffer=NULL;

	delete mVertexArray;
	mVertexArray=NULL;

	mIsInitialized=false;
}

void Batch::create(	const void* vertices,	size_t num_vertices, size_t vertex_size,
					const void* indices,	size_t num_indices,  size_t index_size,
					GLenum primitive_type,	GLenum index_type){

	//WT_ASSERT(!mIsInitialized, "Batch already initialized");

	if(!mIsInitialized){
		mIndexBuffer = new Buffer(GL_ELEMENT_ARRAY_BUFFER);
		mVertexBuffer = new Buffer(GL_ARRAY_BUFFER);
		mVertexArray = new VertexArray;

		// Initialize buffers
		mIndexBuffer->create();
		mVertexBuffer->create();
		mVertexArray->create();
		mIsInitialized = true;
	}

	// Setup members
	mPrimitive=primitive_type; mIndexType=index_type;
	mNumVertices=num_vertices; mNumIndices=num_indices;
	mVertexSize=vertex_size; mIndexSize=index_size;

	// Copy vertex data
	mVertexBuffer->setData(vertices, num_vertices*vertex_size);

	// Copy index data
	mIndexBuffer->setData(indices, num_indices*index_size);

	// Initialize vertex array
	mVertexArray->bind();
		mVertexBuffer->bind();
		mIndexBuffer->bind();
	mVertexArray->unbind();
}

bool Batch::isGenerated() const{
	return mIsInitialized;
}

void Batch::setVertexAttribute(GLuint attrib_index, GLint component_number, GLenum component_type, GLuint component_offset){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	mVertexArray->bind();
		glEnableVertexAttribArray(attrib_index);
		glVertexAttribPointer(attrib_index, component_number, component_type, GL_FALSE, mVertexSize, (const GLvoid*)component_offset);
	mVertexArray->unbind();
}

void Batch::render(Buffer* feedBackBuffer, const GLvoid* indices, Uint32 numIndices){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	Uint32 idcs;
	mVertexArray->bind();
	if(indices){
		idcs=numIndices;
		mIndexBuffer->unbind();
	}
	else{
		idcs=mNumIndices;
		mIndexBuffer->bind();
	}

	if(feedBackBuffer){
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedBackBuffer->getHandle());
		glBeginTransformFeedback(mPrimitive);
	}

	
	glDrawElements(mPrimitive, idcs, mIndexType, static_cast<const GLvoid*>(indices)
		);

	if(feedBackBuffer){
		glEndTransformFeedback();
	}
}

}; // </Gl>

}; // </wt>
