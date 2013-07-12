#include "wt/stdafx.h"
#include "wt/TextureArray.h"

#define TD_TRACE_TAG "TextureArray"

namespace wt{

void TextureArray::create(uint32_t width, uint32_t height, uint32_t num_textures, GLenum format){
	mWidth=width; mHeight=height;
	mFormat=format; mNumTextures=num_textures;

	Texture::create();

	this->bind();
	glTexImage3D(getType(), 0, GL_RGBA, width, height, num_textures, 0, format, GL_UNSIGNED_BYTE, NULL);

	mIsInitialized=true;
}

uint32_t TextureArray::getNumTextures() const{
	return mNumTextures;
}

void TextureArray::create(uint32_t num, ...){
	va_list args;
	va_start(args, num);

	for(uint32_t i=0; i<num; i++){
		const Image* img = va_arg(args,const Image*);

		if(i==0 && !mIsInitialized){
			create(img->getWidth(), img->getHeigth(), num, img->getFormat());
		}
		
		addTexture(img);
	}

	va_end(args);
}

void TextureArray::addTexture(const Image* image){
	// check image dimensions
	if((uint32_t)image->getWidth()!=mWidth && (uint32_t)image->getHeigth()!=mHeight){
		WT_THROW(
				"Image size doesn't match texture array size (texture array: %x,%d, image: %dx%d)",
				mWidth, mHeight, image->getWidth(), image->getHeigth()
			);
	}
	// check internal format
	else if(image->getFormat()!=mFormat){
		WT_THROW(
			"Image format doesn't match texture array format (texture array: %d, image: %d)",
			mFormat, image->getFormat()
			);
	}
	// check texture number
	else if(mCurrTex >= mNumTextures){
		WT_THROW(
			"Texture array full (capacity %d)", mNumTextures);
	}
	// initialization check
	else if(!mIsInitialized){
		WT_THROW(
			"Texture array not initialized (addTexture called before create)");
	}

	bind();
	
	glTexSubImage3D(getType(), 0, 0, 0, mCurrTex, mWidth, mHeight, 1, mFormat, GL_UNSIGNED_BYTE, image->getData());

	mCurrTex++;

	if(mCurrTex==mNumTextures){
		mIsValid=true;
		glGenerateMipmap(getType());
	}
}

} // </wt>
