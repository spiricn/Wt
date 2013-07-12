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
	Uint32 mSize;

public:
	Buffer(GLenum buffer_type);

	~Buffer();

	GLuint getHandle() const;

	void* map(Access access);

	void unmap();

	void bind() const;

	void unbind() const;

	Uint32 getSize() const;

	void create();

	void destroy();

	void setData(const void* p_data, size_t size, GLenum usage=GL_STATIC_DRAW);

	void setSubData(Uint32 offset, const void* data, Uint32 size);

	void getSubData(Uint32 offset, void* dst, Uint32 size) const;

}; // </GLBuffer>

}; // </Gl>

}; // </wt>

#endif
