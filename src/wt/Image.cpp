#include "wt/stdafx.h"

#include "wt/Image.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "Image"

namespace wt{

Image::Image(AResourceManager<Image>* manager, ResourceHandle handle, const String& name) : mData(NULL),
	mWidth(0), mHeigth(0), mFormat(INVALID), mBufferSize(0), mNumComponents(0), AResource(manager, handle, name){
}

Image::~Image(){
	destroy();
}

void Image::fromFrameBuffer(Image::Format fmt){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLint width = viewport[2];
	GLint height = viewport[3];

	/*WT_ASSERT(math::isPowerOfTwo(width) && math::isPowerOfTwo(height),
		"Frame buffer width and height must be a power of two otherwise glReadPixels will fail");*/

	glReadBuffer(GL_FRONT);

	GLbyte* data=new GLbyte[ width*height*3 ];
	glReadPixels(0, 0, width, height, fmt, GL_UNSIGNED_BYTE, (GLvoid*)data);
	setData(width, height, fmt, 3, (const void*)data);

	delete[] data;
}

uint32_t Image::getWidth() const{
	return mWidth;
}

uint32_t Image::getHeigth() const{
	return mHeigth;
}

uint32_t Image::getNumComponents() const{
	return mNumComponents;
}

Image::Format Image::getFormat()	const{
	return mFormat;
}

const void* Image::getData() const{
	return mData;
}

void* Image::getData(){
	return mData;
}

void Image::destroy(){
	if(mData != NULL){
		delete[] mData;
		mData=NULL;
	}
	mData=0;
	mBufferSize=0;
	mWidth=mHeigth=0;
	mFormat=INVALID;
}

void Image::setData(uint32_t width, uint32_t height, Format format, uint32_t numCopmonents,
		const void* data){
	
	destroy();

	mWidth=width; mHeigth=height; mFormat=format;
	mNumComponents=numCopmonents;
	mBufferSize=mWidth*mHeigth*numCopmonents;

	mData = new char[mBufferSize];
	memcpy(mData, data, mBufferSize);
}


}; // </wt>
