#ifndef WT_GLBUFFER_H
#define WT_GLBUFFER_H

#include "wt/Defines.h"

namespace wt{

namespace Gl{

class Buffer{
public:
	enum Access{
		eREAD_ONLY = GL_READ_ONLY,
		eWRITE_ONLY = GL_WRITE_ONLY,
		eREAD_WRITE = GL_READ_WRITE
	};

private:
	WT_DISALLOW_COPY(Buffer)

	GLuint mHandle;
	GLenum mType;
	bool mGenerated;
	bool mIsBufferMapped;
	uint32_t mSize;

public:
	Buffer(GLenum buffer_type);

	~Buffer();

	GLuint getHandle() const;

	void* map(Access access);

	void unmap();

	void bind() const;

	void unbind() const;

	uint32_t getSize() const;

	void create();

	void destroy();

	void setData(const void* p_data, size_t size, GLenum usage=GL_STATIC_DRAW);

	void setSubData(uint32_t offset, const void* data, uint32_t size);

	void getSubData(uint32_t offset, void* dst, uint32_t size) const;

}; // </GLBuffer>

}; // </Gl>

}; // </wt>

#endif
