#ifndef WT_IMAGE_H
#define WT_IMAGE_H


#include "wt/stdafx.h"

#include "wt/AResource.h"
#include "wt/AResourceManager.h"
#include "wt/GLBuffer.h"
#include <td/td.h>
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
	uint32_t mWidth, mHeigth, mNumComponents;
	Format mFormat;
	uint32_t mBufferSize;

public:

	Image(AResourceManager<Image>* manager=NULL, ResourceHandle handle=0, const std::string& name="");

	~Image();

	void setData(uint32_t width, uint32_t height, Format format, uint32_t numCopmonents,
		const void* data);

	void destroy();
	
	void fromFrameBuffer(Format fmt);

	uint32_t getWidth() const;

	uint32_t getHeigth() const;

	uint32_t getNumComponents() const;

	Format	getFormat()	const;

	const void* getData() const;

	void* getData();

}; // </Image>

} // </wt>

#endif // </WT_IMAGE_H>
