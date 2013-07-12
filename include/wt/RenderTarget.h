#ifndef WT_RENDERTARGET_H
#define WT_RENDERTARGET_H

#include "wt/stdafx.h"

#include "wt/FrameBuffer.h"

namespace wt{

class RenderTarget{
private:
	GLenum mBuffers[GL_MAX_DRAW_BUFFERS];
	Uint32 mNumBuffers;
	Gl::FrameBuffer mFrameBuffer;

public:
	RenderTarget() : mNumBuffers(0){
		memset(mBuffers, 0x00, GL_MAX_DRAW_BUFFERS*sizeof(GLenum));
	}

	void addBuffer(GLenum buffer){
		mBuffers[mNumBuffers++] = buffer;
	}

	Gl::FrameBuffer& getFrameBuffer(){
		return mFrameBuffer;
	}

	void bind(){
		mFrameBuffer.bind(wt::Gl::FrameBuffer::DRAW);
		glDrawBuffers(mNumBuffers, mBuffers);
	}

	void unbind(){
		mFrameBuffer.unbind(wt::Gl::FrameBuffer::DRAW);
	}
}; // </RenderTarget>


}; // </wt>

#endif // </WT_RENDERTARGET_H>