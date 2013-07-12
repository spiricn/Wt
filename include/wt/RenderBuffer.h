#ifndef WT_RENDERBUFFER_H
#define WT_RENDERBUFFER_H

namespace wt{

namespace Gl{

class RenderBuffer{
private:
	GLuint mHandle;
	GLenum mFormat;
	uint32_t mWidth, mHeight;

public:
	RenderBuffer();

	uint32_t getWidth() const;

	uint32_t getHeight() const;

	void create();

	GLuint getHandle() const;

	void setStorage(GLenum format, uint32_t w, uint32_t h);

	void bind();

	static void unbind();

	void destroy();

}; // </RenderBuffer>


}; // </Gl>

}; // </wt>

#endif