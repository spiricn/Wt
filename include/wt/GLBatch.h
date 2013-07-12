#ifndef WT_GLBATCH_H
#define WT_GLBATCH_H

namespace wt{

namespace Gl{

/* forward declarations */
class Buffer;
class VertexArray;

class SubBatch{
public:
	Uint32 mMinIndex, mMaxIndex, mStartIndex, mNumIndices;

	SubBatch(Uint32 minIndex, Uint32 maxIndex, Uint32 start, Uint32 num) : mMinIndex(minIndex),
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
	
	Uint32 getIndexAt(Uint32 index) const;

	void getVertexAt(Uint32 index, void* vertex) const;

	void setIndexAt(Uint32 index, Uint32 value);

	void setVertexAt(Uint32 index, const void* vertex);

	GLuint getNumVertices() const;

	GLuint getNumIndices() const;

	void unmapIndexBuffer();

	void unmapVertexBuffer();

	bool isGenerated() const;

	void create(const void* vertices, size_t num_vertices, size_t vertex_size,
				const void* indices,	size_t num_indices,  size_t index_size,
				GLenum primitive_type=GL_TRIANGLES, GLenum index_type=GL_UNSIGNED_INT);

	void destroy();

	void bind();

	void setVertexAttribute(GLuint attrib_index, GLint component_number, GLenum component_type, GLuint component_offset);

	void render(Buffer* feedBackBuffer=NULL, const GLvoid* indices=NULL, Uint32 numIndices=0);

	void renderRange(Uint32 startIndex, Uint32 endIndex, Uint32 numPrimitives);

	void render(const SubBatch& batch);
};

}; // </Gl>

}; // </wt>
#endif
