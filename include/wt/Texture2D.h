#ifndef WT_TEXTURE2D_H
#define WT_TEXTURE2D_H


#include "wt/Texture.h"
#include "wt/Image.h"
#include "wt/AResource.h"
#include "wt/AResourceManager.h"
#include "wt/Singleton.h"
#include "wt/Color.h"
#include "wt/Buffer.h"
#include "wt/DevilImageLoader.h"

namespace wt
{

class Texture2D : public AResource<Texture2D>, public Texture{
public:
	enum Type{
		eTYPE_TEXTURE_2D = GL_TEXTURE_2D,
		eTYPE_RECT = GL_TEXTURE_RECTANGLE,
	}; // </Type>

public:
	Texture2D(AResourceManager<Texture2D>* manager=NULL, ResourceHandle handle=0, const String& name="", Type type=eTYPE_TEXTURE_2D);

	void setData(GLint width, GLint height, GLenum format=GL_RGB,
		GLenum internalFormat=GL_RGB, const GLbyte* data=0, GLenum type=GL_UNSIGNED_BYTE, bool mmap=true);

	void setData(Image* image);

	Color sample(float s, float t) const;

	void generateMipmap();

	void dump(AIOStream& stream);

	void dump(const String& path);

	static void depthDump(Texture2D* src, const char* dst);

	static void copy(Texture2D* src, Texture2D* dst);

	void setSubData(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void* data);

	GLuint getWidth() const;

	GLuint getHeigth() const;

private:
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mInternalFormat;
	GLenum mFormat;

private:
	friend class TextureLoader;
	friend class TextureManager;
}; // </Texture2D>
 
} // </Wt>

#endif // </WT_TEXTURE2D_H>
