#include "wt/stdafx.h"

#include "wt/RenderBuffer.h"

#define TD_TRACE_TAG "RenderBuffer"

namespace wt{

namespace Gl{

uint32_t RenderBuffer::getWidth() const{
	return mWidth;
}

uint32_t RenderBuffer::getHeight() const{
	return mHeight;
}

void RenderBuffer::create(){
	glGenRenderbuffers(1, &mHandle);
}

GLuint RenderBuffer::getHandle() const{
	return mHandle;
}

void RenderBuffer::setStorage(GLenum format, uint32_t w, uint32_t h){
	bind();
	glRenderbufferStorage(GL_RENDERBUFFER,
		format, w, h);

	mFormat = format;
	mWidth = w;
	mHeight = h;
}

void RenderBuffer::bind(){
	glBindRenderbuffer(GL_RENDERBUFFER, mHandle);
}

void RenderBuffer::unbind(){
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::destroy(){
	glDeleteRenderbuffers(1, &mHandle);
}

}; // </Gl>

}; // </wt>
