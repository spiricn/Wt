#include "wt/stdafx.h"
#include "wt/GLBatch.h"

#include "wt/GLBuffer.h"
#include "wt/Exception.h"
#include "wt/VertexArray.h"

#define TD_TRACE_TAG "GLBatch"

namespace wt{

namespace gl{


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


	// TODO Not sure why but it seems that index and vertex buffer must be bound manually along side vertex array
	mIndexBuffer->bind();
	mVertexBuffer->bind();

	// TODO check if the right index buffer is bound, will cause crash otherwise
	
	/* TODO min max indices do nothing at the moment
	from what I understand their only purpose is to let GPU driver optimize the actual drawing
	so they should probably be precalculated in runtime (or maybe even stored in a model file)*/
	glDrawRangeElements(
		mPrimitive,	
		/* batch.mMinIndex */ 0,
		/* batch.mMaxIndex */ 0xFFFFFFFF,
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
	
uint32_t Batch::getIndexAt(uint32_t index) const{
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumIndices){
		WT_THROW("Index buffer index out of range (index=%d size=%d)", index, mNumIndices);
	}

	uint32_t res;
	mIndexBuffer->getSubData(index*mIndexSize, &res, mIndexSize);
	return res;
}

void Batch::getVertexAt(uint32_t index, void* vertex) const{
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumVertices){
		WT_THROW("Vertex buffer index out of range (index=%d size=%d)", index, mNumVertices);
	}

	mVertexBuffer->getSubData(index*mVertexSize, vertex, mVertexSize);
}

void Batch::setIndexAt(uint32_t index, uint32_t value){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	if(index >= mNumIndices){
		WT_THROW("Index buffer index out of range (index=%d size=%d)", index, mNumIndices);
	}

	mIndexBuffer->setSubData(index*mIndexSize, &value, mIndexSize);
}

void Batch::setVertexAt(uint32_t index, const void* vertex){
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
	if(mIndexBuffer){
		delete mIndexBuffer;
		mIndexBuffer=NULL;
	}

	delete mVertexBuffer;
	mVertexBuffer=NULL;

	delete mVertexArray;
	mVertexArray=NULL;

	mIsInitialized=false;
}

void Batch::create(GLenum primitiveType, const void* vertices, uint32_t numVertices, uint32_t vertexSize, const void* indices, uint32_t numIndices, uint32_t indexSize, GLenum indexType){
	if(!mIsInitialized){
		// Vertex array (encapsulates batch state)
		mVertexArray = new VertexArray;
		mVertexArray->create();

		// Vertex buffer
		mVertexBuffer = new Buffer(GL_ARRAY_BUFFER);
		mVertexBuffer->create();

		if(numIndices){
			mIndexBuffer = new Buffer(GL_ELEMENT_ARRAY_BUFFER);
			mIndexBuffer->create();
		}
		else{
			// Not indexed
			mIndexBuffer = NULL;
		}

		mIsInitialized = true;
	}

	// Setup members
	mPrimitive=primitiveType;
	mIndexType=indexType;
	mNumVertices=numVertices;
	mNumIndices=numIndices;
	mVertexSize=vertexSize;
	mIndexSize=indexSize;

	// Copy vertex data
	mVertexBuffer->setData(vertices, numVertices*vertexSize);

	// Copy index data
	if(mIndexBuffer){
		mIndexBuffer->setData(indices, numIndices*indexSize);
	}

	// Initialize vertex array
	mVertexArray->bind();

	mVertexBuffer->bind();
	if(mIndexBuffer){
		mIndexBuffer->bind();
	}

	mVertexArray->unbind();
}

bool Batch::isGenerated() const{
	return mIsInitialized;
}

void Batch::setVertexAttribute(GLuint attrib_index, GLint component_number, GLenum component_type, GLuint component_offset){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	mVertexArray->bind();
		gl( EnableVertexAttribArray(attrib_index) );
		gl( VertexAttribPointer(attrib_index, component_number, component_type, GL_FALSE, mVertexSize, (const GLvoid*)component_offset) );
	mVertexArray->unbind();
}

void Batch::render(Buffer* feedBackBuffer, const GLvoid* indices, uint32_t numIndices, uint32_t startElem, uint32_t numElems){
	WT_ASSERT(mIsInitialized, "Batch not initialized");

	uint32_t idcs;
	mVertexArray->bind();

	if(indices){
		idcs=numIndices;
		if(mIndexBuffer){
			mIndexBuffer->unbind();
		}
	}
	else if(mIndexBuffer){
		idcs=mNumIndices;
		mIndexBuffer->bind();
	}

//	
//		static GLuint primitivesWritten = 0;
//static GLuint query = 0;

	//if(!query){
	//	glGenQueries(1, &query);
	//}
	//

	if(feedBackBuffer){
			//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		gl( BindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedBackBuffer->getHandle()) );
		gl( BeginTransformFeedback(mPrimitive) );
	}

	
	if(indices || mIndexBuffer){
		gl( DrawElements(mPrimitive, idcs, mIndexType, static_cast<const GLvoid*>(indices)) );
	}
	else{
		gl( DrawArrays(mPrimitive, numElems ? startElem : 0 , numElems ? numElems : mNumVertices) );//primitivesWritten? primitivesWritten : ) );

		//if(feedBackBuffer){
		//	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		//	// Query the number of primitives written in the transform buffer.
		//	primitivesWritten = 0;
		//	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitivesWritten );
		//	LOG("%d", primitivesWritten);
		//}
	}

	if(feedBackBuffer){
		gl( EndTransformFeedback() );
	}
}

}; // </gl>

}; // </wt>
