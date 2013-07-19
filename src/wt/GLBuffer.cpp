#include "wt/stdafx.h"

#include "wt/GLBuffer.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "GLBuffer"

namespace wt{

namespace Gl{

Buffer::Buffer(GLenum buffer_type): mType(buffer_type), mGenerated(false),
	mIsBufferMapped(false), mSize(0){
}

Buffer::~Buffer(){
	destroy();
}

GLuint Buffer::getHandle() const{
	return mHandle;
}

void* Buffer::map(Access access){

	mIsBufferMapped=true;
	bind();
	return glMapBuffer(mType, access);
}

void Buffer::unmap(){
	mIsBufferMapped=false;
	bind();
	glUnmapBuffer(mType);
}

void Buffer::bind() const{
	glBindBuffer(mType, mHandle);
}

void Buffer::setSubData(uint32_t offset, const void* data, uint32_t size){
#ifdef WT_CHECKED
	WT_ASSERT(offset+size <= mSize, "Buffer sub data size out of range (%d + %d > %d",
		offset, size, mSize);
#endif

	bind();
	glBufferSubData(mType, offset, size, data);
}

void Buffer::getSubData(uint32_t offset, void* dst, uint32_t size) const{
#ifdef WT_CHECKED
	WT_ASSERT(offset+size <= mSize, "Buffer sub data size out of range (%d + %d > %d",
		offset, size, mSize);
#endif

	bind();
	glGetBufferSubData(mType, offset, size, dst);
}
	
void Buffer::unbind() const{
	glBindBuffer(mType, 0);
}

uint32_t Buffer::getSize() const{
	return mSize;
}

void Buffer::destroy(){
	if(mGenerated){
		glDeleteBuffers(1, &mHandle);
	}
}

void Buffer::create(){
	if(!mGenerated){
		mGenerated=true;
		glGenBuffers(1, &mHandle);
	}
}

void Buffer::setData(const void* data, uint32_t size, GLenum usage){
	if(!mGenerated){
		create();
	}

	bind();
	glBufferData(mType, size, data, usage);
	mSize = size;
	unbind();
}

}; // </GL>

}; // </wt>
