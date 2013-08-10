#ifndef WT_FRAMEBUFFER_H
#define WT_FRAMEBUFFER_H


#include "wt/stdafx.h"

#include "wt/Buffer.h"
#include "wt/Texture2D.h"

namespace wt{

namespace gl{

class RenderBuffer;

class FrameBuffer{
public:
	enum Mode{
		eMODE_READ = GL_READ_FRAMEBUFFER,
		eMODE_DRAW = GL_DRAW_FRAMEBUFFER,
		eMODE_FRAMEBUFFER = GL_FRAMEBUFFER
	}; // </Mode>

private:

	bool mIsCreated;
	GLuint mHandle;
	wt::Buffer<GLenum> mAttachments;

public:
	FrameBuffer();

	~FrameBuffer();

	void addAttachment(GLenum attachment, const RenderBuffer& target);

	void addAttachment(GLenum attachment, const Texture2D* target);

	void addAttachment(GLenum attachment, GLuint texture, GLenum textureType);

	bool isCreated() const{
		return mIsCreated;
	}

	// Bind functions
	void bind();

	void bind(Mode mode);

	void bindRead();

	void bindDraw();

	// Unbind functions

	static void unbind(Mode mode);

	static void unbind();

	static void unbindDraw();

	static void unbindRead();

	void create();

	GLenum getStatus();

	bool isComplete();

	void destroy();

	void blit(const glm::vec4& src, const glm::vec4& dst, GLenum attachment=GL_COLOR_ATTACHMENT0, GLint mask=GL_COLOR_BUFFER_BIT, GLenum filter=GL_LINEAR);

}; // </FrameBuffer>


}; // </gl>

}; // </wt>

#endif