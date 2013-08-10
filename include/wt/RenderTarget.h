#ifndef WT_RENDERTARGET_H
#define WT_RENDERTARGET_H

#include "wt/stdafx.h"

#include "wt/FrameBuffer.h"

namespace wt{

class RenderTarget{
private:
	GLenum mBuffers[GL_MAX_DRAW_BUFFERS];
	uint32_t mNumBuffers;
	gl::FrameBuffer mFrameBuffer;

public:
	RenderTarget() : mNumBuffers(0){
		memset(mBuffers, 0x00, GL_MAX_DRAW_BUFFERS*sizeof(GLenum));
	}

	void addBuffer(GLenum buffer){
		mBuffers[mNumBuffers++] = buffer;
	}

	gl::FrameBuffer& getFrameBuffer(){
		return mFrameBuffer;
	}

	void bind(){
		mFrameBuffer.bind(wt::gl::FrameBuffer::eMODE_DRAW);
		glDrawBuffers(mNumBuffers, mBuffers);
	}

	void unbind(){
		mFrameBuffer.unbind(wt::gl::FrameBuffer::eMODE_DRAW);
	}
}; // </RenderTarget>


}; // </wt>

#endif // </WT_RENDERTARGET_H>