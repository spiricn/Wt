#ifndef WT_RENDERBUFFER_H
#define WT_RENDERBUFFER_H

namespace wt{

namespace Gl{

class RenderBuffer{
private:
	GLuint mHandle;
	GLenum mFormat;
	Uint32 mWidth, mHeight;

public:
	RenderBuffer();

	Uint32 getWidth() const;

	Uint32 getHeight() const;

	void create();

	GLuint getHandle() const;

	void setStorage(GLenum format, Uint32 w, Uint32 h);

	void bind();

	static void unbind();

	void destroy();

}; // </RenderBuffer>


}; // </Gl>

}; // </wt>

#endif