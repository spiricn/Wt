#include "wt/stdafx.h"

#include "wt/FrameBuffer.h"
#include "wt/RenderBuffer.h"
#include "wt/GLTrace.h"

#define TD_TRACE_TAG "FrameBuffer"

namespace wt{

namespace gl{

FrameBuffer::FrameBuffer() : mHandle(0), mIsCreated(false){
}

FrameBuffer::~FrameBuffer(){
	if(mIsCreated){
		destroy();
	}
}

void FrameBuffer::blit(const glm::vec4& src, const glm::vec4& dst, GLenum attachment, GLint mask, GLenum filter){
	bindRead();

	gl( ReadBuffer(attachment) );

	gl( BlitFramebuffer(
		src.x, src.y, src.z, src.w,
		dst.x, dst.y, dst.z, dst.w,
		mask, filter
	));
}


void FrameBuffer::addAttachment(GLenum attachment, const RenderBuffer& target){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	// TODO why draw? probably should be eMODE_FRAMEBUFFER
	bind(eMODE_DRAW);

	gl( FramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
		attachment, GL_RENDERBUFFER, target.getHandle()) );
}

void FrameBuffer::addAttachment(GLenum attachment, GLuint texture, GLenum textureType){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	bind(eMODE_DRAW);

	// TODO not sure if binding it here is necessary
	if(texture != 0){
		gl( BindTexture(textureType, texture) );
	}

	gl( FramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
		attachment, textureType,
		texture, 0) );
}

void FrameBuffer::addAttachment(GLenum attachment, const Texture2D* target){
	addAttachment(attachment, target->getTexHandle(), target->getType());
}

void FrameBuffer::bind(Mode mode){
	WT_ASSERT(mIsCreated, "Framebuffer not initialized");

	gl( BindFramebuffer(mode, mHandle) );
}

void FrameBuffer::bindRead(){
	bind(eMODE_READ);
}

void FrameBuffer::bindDraw(){
	bind(eMODE_DRAW);
}

void FrameBuffer::bind(){
	bind(eMODE_FRAMEBUFFER);
}

void FrameBuffer::unbind(Mode mode){
	gl( BindFramebuffer(mode, 0) );
}

void FrameBuffer::unbind(){
	unbind(eMODE_FRAMEBUFFER);
}

void FrameBuffer::unbindDraw(){
	unbind(eMODE_DRAW);
}

void FrameBuffer::unbindRead(){
	unbind(eMODE_READ);
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

	bind(eMODE_DRAW);

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


}; // </gl>

}; // </wt>
