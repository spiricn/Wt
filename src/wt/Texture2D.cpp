#include "wt/stdafx.h"

#include "wt/Texture2D.h"

namespace wt{

void Texture2D::dump(const String& path){
	bind();
	Buffer<Uint8> p;

	p.create(mWidth*mHeight*3);
	glGetTexImage(getType(), 0, Image::RGB, GL_UNSIGNED_BYTE, (GLvoid*)p.getData());

	Image img;
	img.setData(mWidth, mHeight, Image::RGB, 3, (unsigned char*)p.getData());
	DevilImageLoader::getSingleton().save(path,
		&img);
}

void Texture2D::setData(GLint width, GLint height,
	GLenum format, GLenum internalFormat, const GLbyte* data, GLenum type, bool mmap){

	if(!isCreated()){
		WT_QEXCEPT("Must create texture before setting data (%s)", mName.c_str());
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
		WT_QEXCEPT("Must create texture before setting data (%s)", mName.c_str());
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