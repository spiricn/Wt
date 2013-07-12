#ifndef WT_IMAGE_H
#define WT_IMAGE_H


#include "wt/stdafx.h"

#include "wt/AResource.h"
#include "wt/AResourceManager.h"
#include "wt/GLBuffer.h"
#include "wt/Log.h"
namespace wt{

class Image : public AResource<Image>{
public:
	WT_DISALLOW_COPY(Image);

	enum Format{
		INVALID,
		BGR = GL_BGR,
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
	};

private:
	static const String TAG;
	void* mData;
	Uint32 mWidth, mHeigth, mNumComponents;
	Format mFormat;
	Uint32 mBufferSize;

public:

	Image(ResourceHandle handle=0, const std::string& name="");

	~Image();

	void setData(Uint32 width, Uint32 height, Format format, Uint32 numCopmonents,
		const void* data);

	void destroy();
	
	void fromFrameBuffer(Format fmt);

	Uint32 getWidth() const;

	Uint32 getHeigth() const;

	Uint32 getNumComponents() const;

	Format	getFormat()	const;

	const void* getData() const;

	void* getData();

}; // </Image>

typedef ResourceLink<Image> ImageLink;

}; // </wt>

#endif
