#ifndef WT_UNIFORMBUFFEROBJECT_H
#define WT_UNIFORMBUFFEROBJECT_H

#include "wt/stdafx.h"

#include "wt/GLBuffer.h"

namespace wt
{

namespace gl
{

class UniformBufferObject{
public:
	UniformBufferObject() : mBuffer(GL_UNIFORM_BUFFER){
		mBuffer.create();
	}

	void* map(){
		return mBuffer.map(gl::Buffer::eREAD_WRITE);
	}

	void setData(void* data, uint32_t dataSize){
		mBuffer.setData(data, dataSize, GL_DYNAMIC_DRAW);
	}

	void unmap(){
		mBuffer.unmap();
	}

	void setBindPoint(uint32_t point){
		mBuffer.bind();
		glBindBufferBase(GL_UNIFORM_BUFFER, point, mBuffer.getHandle());
	}

private:
	gl::Buffer mBuffer;
}; // </UniformBufferObject>


} // </gl>

} // </wt>

#endif // </WT_UNIFORMBUFFEROBJECT_H>