#ifndef WT_GLBATCH_H
#define WT_GLBATCH_H

#include "wt/GLTrace.h"

namespace wt{

namespace gl{

/* forward declarations */
class Buffer;
class VertexArray;

class SubBatch{
public:
	uint32_t mMinIndex, mMaxIndex, mStartIndex, mNumIndices;

	SubBatch(uint32_t minIndex, uint32_t maxIndex, uint32_t start, uint32_t num) : mMinIndex(minIndex),
		mMaxIndex(maxIndex), mStartIndex(start), mNumIndices(num){
	}

}; // </SubBatch>

class Batch{
private:
	Buffer* mVertexBuffer;
	Buffer* mIndexBuffer;
	bool mIndexBufferOwned;
	VertexArray* mVertexArray;
	GLuint mNumVertices, mNumIndices, mVertexSize, mIndexSize;
	GLenum mIndexType, mPrimitive;
	bool mIsInitialized;
	
public:
	Batch();

	~Batch();

	Buffer& getVertexBuffer();

	Buffer& getIndexBuffer();

	GLuint getVertexSize();

	GLuint getIndexSize();
	
	uint32_t getIndexAt(uint32_t index) const;

	void getVertexAt(uint32_t index, void* vertex) const;

	void setIndexAt(uint32_t index, uint32_t value);

	void setVertexAt(uint32_t index, const void* vertex);

	GLuint getNumVertices() const;

	GLuint getNumIndices() const;

	void unmapIndexBuffer();

	void unmapVertexBuffer();

	bool isGenerated() const;

	void create(
		GLenum primitiveType,
		const void* vertices, uint32_t numVertices, uint32_t vertexSize,
		const void* indices=NULL, uint32_t numIndices=0, uint32_t indexSize=0, GLenum indexType=GL_UNSIGNED_INT);


	void destroy();

	void bind();

	void setVertexAttribute(GLuint attrib_index, GLint component_number, GLenum component_type, GLuint component_offset);

	void render(Buffer* feedBackBuffer=NULL, const GLvoid* indices=NULL, uint32_t numIndices=0, uint32_t startElem=0, uint32_t numElems=0);

	void renderRange(uint32_t startIndex, uint32_t endIndex, uint32_t numPrimitives);

	void render(const SubBatch& batch);

};

}; // </gl>

}; // </wt>
#endif
