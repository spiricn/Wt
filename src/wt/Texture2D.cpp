#include "wt/stdafx.h"

#include "wt/Texture2D.h"
#include "wt/GLTrace.h"

#define TD_TRACE_TAG "Texture2D"

namespace wt{

void Texture2D::depthDump(Texture2D* src, const char* dst){
	src->bind();

	// Depth dump
	Buffer<float> depth;
	depth.create(src->mWidth*src->mHeight);
	gl( GetTexImage(src->getType(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)depth.getData()) );


	Buffer<uint8_t> rgb(src->mWidth*src->mHeight*3);

	for(uint32_t i=0; i<depth.getCapacity(); i++){
		float n = 1.0; // camera z near
		float f = 100.0; // camera z far
		float z = depth[i];
		z = (2.0 * n) / (f + n - z * (f - n));
		z *= 255;

		rgb[i*3 + 0] = z;
		rgb[i*3 + 1] = z;
		rgb[i*3 + 2] = z;
	}

	Image img;
	img.setData(src->mWidth, src->mHeight, Image::RGB, 3, (unsigned char*)rgb.getData());

	FileIOStream stream;
	stream.open(dst, AIOStream::eMODE_WRITE);

	DevilImageLoader::getSingleton().save(&stream,
		&img);

}


Color Texture2D::sample(float s, float t) const{
	Color res;

	Buffer<GLubyte> pixels(mWidth*mHeight*3);
	gl( GetTexImage(getType(), 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels.getData()) );


	// TODO handle different min/mag filters, wrapping etc..
	s = glm::clamp(s, 0.0f, 1.0f);
	t = glm::clamp(t, 0.0f, 1.0f);


	const uint32_t x = mWidth*s;
	const uint32_t y = mHeight*t;

	GLubyte* rgb = &pixels.getData()[(x * mHeight + y ) * 3];

	res.red = *(rgb + 0);
	res.green = *(rgb + 1);
	res.blue = *(rgb + 2);

	return res;
}

void Texture2D::dump(AIOStream& stream){
	bind();

#if 0
	// Depth dump
	// TODO do this properly
	Buffer<float> depth;
	depth.create(mWidth*mHeight);
	gl( GetTexImage(getType(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)depth.getData()) );


	Buffer<uint8_t> rgb(mWidth*mHeight*3);

	for(uint32_t i=0; i<depth.getCapacity(); i++){
		float n = 1.0; // camera z near
		float f = 100.0; // camera z far
		float z = depth[i];
		z = (2.0 * n) / (f + n - z * (f - n));
		z *= 255;

		rgb[i*3 + 0] = z;
		rgb[i*3 + 1] = z;
		rgb[i*3 + 2] = z;
	}

	Image img;
	img.setData(mWidth, mHeight, Image::RGB, 3, (unsigned char*)rgb.getData());

	DevilImageLoader::getSingleton().save(&stream,
		&img);

#else
	Buffer<GLubyte> rgb(mWidth*mHeight*3);
	gl( GetTexImage(getType(), 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)rgb.getData()) );

	Image img;
	img.setData(mWidth, mHeight, Image::RGB, 3, (unsigned char*)rgb.getData());

	DevilImageLoader::getSingleton().save(&stream,
		&img);
#endif
}

void Texture2D::generateMipmap(){
	bind();
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::dump(const String& path){
	// TODO Textrue2D should acquire this stream from elsewhere
	FileIOStream stream(path, AIOStream::eMODE_WRITE);

	dump(stream);

	stream.close();
}

void Texture2D::setData(GLint width, GLint height,
	GLenum format, GLenum internalFormat, const GLbyte* data, GLenum type, bool mmap){

	if(!isCreated()){
		WT_THROW("Must create texture before setting data (%s)", mName.c_str());
	}

	mFormat = format;
	mInternalFormat = internalFormat;
	mWidth = width;
	mHeight = height;

	bind();

	glTexImage2D(getType(), 0, internalFormat, mWidth, mHeight,
		0, mFormat, type, data);

	if(mmap && getType() != eRECT_TEXTURE){
		glGenerateMipmap(getType());
	}
}

void Texture2D::setData(Image* image){
	if(!isCreated()){
		WT_THROW("Must create texture before setting data (%s)", mName.c_str());
	}

	mFormat = image->getFormat();
	mInternalFormat = image->getNumComponents();
	mWidth = image->getWidth();
	mHeight = image->getHeigth();

	bind();

	glTexImage2D(getType(), 0, mInternalFormat, mWidth, mHeight,
		0, mFormat, GL_UNSIGNED_BYTE, image->getData());

	if(getType() != eRECT_TEXTURE){
		glGenerateMipmap(getType());
	}
}

};