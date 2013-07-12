#include "wt/stdafx.h"

#include "wt/FrameBuffer.h"
#include "wt/RenderBuffer.h"

namespace wt{

namespace Gl{

FrameBuffer::FrameBuffer() : mHandle(0), mIsCreated(false){
}

FrameBuffer::~FrameBuffer(){
	if(mIsCreated){
		destroy();
	}
}

void FrameBuffer::addAttachment(GLenum attachment, const RenderBuffer& target){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	bind(DRAW);

	gl( FramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
		attachment, GL_RENDERBUFFER, target.getHandle()) );
}

void FrameBuffer::addAttachment(GLenum attachment, GLuint texture){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	gl( BindTexture(GL_TEXTURE_RECTANGLE, texture) );

	gl( FramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
		attachment, GL_TEXTURE_RECTANGLE,
		texture, 0) );

}

void FrameBuffer::addAttachment(GLenum attachment, const Texture2D* target){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	bind(DRAW);

	target->bind();

	gl( FramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
		attachment, target->getType(),
		target->getTexHandle(), 0) );
}

void FrameBuffer::bind(Mode mode){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	gl( BindFramebuffer(mode, mHandle) );
}

void FrameBuffer::unbind(Mode mode){
	gl( BindFramebuffer(mode, 0) );
}

void FrameBuffer::create(){
	WT_ASSERT(!mIsCreated, "Framebuffer already initialized");

	if(!mIsCreated){
		gl( GenFramebuffers(1, &mHandle) );
		mIsCreated = true;
	}
}

GLenum FrameBuffer::getStatus(){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	bind(DRAW);

	return glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
}

bool FrameBuffer::isComplete(){
	return mIsCreated ? getStatus()==GL_FRAMEBUFFER_COMPLETE : false;
}

void FrameBuffer::destroy(){
	WT_ASSERT(mIsCreated, "Attempting to destroy uninitialized framebuffer");

	gl( DeleteFramebuffers(1, &mHandle) );

	mHandle = 0;

	mIsCreated = false;
}


}; // </Gl>

}; // </wt>
