#include "wt/stdafx.h"

#include "wt/RenderBuffer.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "RenderBuffer"

namespace wt{

namespace gl{

RenderBuffer::RenderBuffer() : mCreated(false), mWidth(0), mHeight(0), mHandle(0){
}

uint32_t RenderBuffer::getWidth() const{
	return mWidth;
}

uint32_t RenderBuffer::getHeight() const{
	return mHeight;
}

void RenderBuffer::create(){
	WT_ASSERT(!mCreated, "Render buffer object already created");

	glGenRenderbuffers(1, &mHandle);
	mCreated = true;
}

GLuint RenderBuffer::getHandle() const{
	return mHandle;
}

void RenderBuffer::setStorage(GLenum format, uint32_t w, uint32_t h){
	WT_ASSERT(mCreated, "Render buffer object not created, call 'create' first");

	bind();
	glRenderbufferStorage(GL_RENDERBUFFER,
		format, w, h);

	mFormat = format;
	mWidth = w;
	mHeight = h;
}

void RenderBuffer::bind(){
	WT_ASSERT(mCreated, "Render buffer object not created, call 'create' first");
	glBindRenderbuffer(GL_RENDERBUFFER, mHandle);
}

void RenderBuffer::unbind(){
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::destroy(){
	WT_ASSERT(mCreated, "Render buffer object not created, call 'create' first");
	glDeleteRenderbuffers(1, &mHandle);
	mCreated = false;
	mWidth = 0;
	mHeight = 0;
	mFormat = 0;
}

}; // </gl>

}; // </wt>
