#include "wt/stdafx.h"

#include "wt/Texture.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "Texture"

namespace wt{

Texture::Texture(GLenum type) : mTexHandle(0), mType(type), mIsGenerated(false){
}

void Texture::create(){
	if(!mIsGenerated){
		glGenTextures(1, &mTexHandle);
		mIsGenerated=true;
	}
}

void Texture::destroy(){
	if(mIsGenerated){
		glDeleteTextures(1, &mTexHandle);
		mIsGenerated = false;
		mTexHandle=0;
	}
}

Texture::~Texture(){
	destroy();
}

void Texture::bind() const{
	glBindTexture(mType, mTexHandle);
}

void Texture::unbind() const{
	glBindTexture(mType, 0);
}

bool Texture::isCreated() const{
	return mIsGenerated;
}

GLuint Texture::getTexHandle() const{
	return mTexHandle;
}

GLenum Texture::getType() const{
	return mType;
}

}; // </wt>
