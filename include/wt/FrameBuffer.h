#ifndef WT_FRAMEBUFFER_H
#define WT_FRAMEBUFFER_H


#include "wt/stdafx.h"

#include "wt/Buffer.h"
#include "wt/Texture2D.h"

namespace wt{

namespace Gl{

class RenderBuffer;

class FrameBuffer{
public:
	enum Mode{
		READ = GL_READ_FRAMEBUFFER,
		DRAW = GL_DRAW_FRAMEBUFFER
	};

private:

	bool mIsCreated;
	GLuint mHandle;
	wt::Buffer<GLenum> mAttachments;

public:
	FrameBuffer();

	~FrameBuffer();

	void addAttachment(GLenum attachment, const RenderBuffer& target);

	void addAttachment(GLenum attachment, const Texture2D* target);

	void addAttachment(GLenum attachment, GLuint texture);

	bool isCreated() const{
		return mIsCreated;
	}

	void bind(Mode mode);

	static void unbind(Mode mode);

	void create();

	GLenum getStatus();

	bool isComplete();

	void destroy();

}; // </FrameBuffer>


}; // </Gl>

}; // </wt>

#endif