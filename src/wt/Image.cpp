#include "wt/stdafx.h"

#include "wt/Image.h"
#include "wt/Exception.h"

namespace wt{

const String Image::TAG = "Image";

Image::Image(ResourceHandle handle, const String& name) : mData(NULL),
	mWidth(0), mHeigth(0), mFormat(INVALID), mBufferSize(0), mNumComponents(0),AResource(handle, name){
}

Image::~Image(){
	destroy();
}

void Image::fromFrameBuffer(Image::Format fmt){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLint width = viewport[2];
	GLint height = viewport[3];

	glReadBuffer(GL_FRONT);

	GLbyte* data=new GLbyte[ width*height*3 ];
	glReadPixels(0, 0, width, height, fmt, GL_UNSIGNED_BYTE, (GLvoid*)data);
	setData(width, height, fmt, 3, (const void*)data);

	delete[] data;
}

Uint32 Image::getWidth() const{
	return mWidth;
}

Uint32 Image::getHeigth() const{
	return mHeigth;
}

Uint32 Image::getNumComponents() const{
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

void Image::setData(Uint32 width, Uint32 height, Format format, Uint32 numCopmonents,
		const void* data){
	
	destroy();

	mWidth=width; mHeigth=height; mFormat=format;
	mNumComponents=numCopmonents;
	mBufferSize=mWidth*mHeigth*numCopmonents;

	mData = new char[mBufferSize];
	memcpy(mData, data, mBufferSize);
}


}; // </wt>
