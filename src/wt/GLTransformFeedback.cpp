#include "wt/stdafx.h"

#include "wt/Exception.h"
#include "wt/GLTransformFeedback.h"
#include "wt/GLTrace.h"
#include "wt/GLBuffer.h"

#define TD_TRACE_TAG "TransformFeedback"

namespace wt{

namespace Gl{

TransformFeedback::TransformFeedback() : mCreated(false), mHandle(0){
	WT_ASSERT(glewIsSupported("GL_ARB_transform_feedback2"),
		"TransformFeedback object not supported by current version of OpenGL");
}

TransformFeedback::~TransformFeedback(){
	if(mCreated){
		destroy();
	}
}

void TransformFeedback::create(){
	WT_ASSERT(!mCreated, "Object alraedy created");

	glGenTransformFeedbacks(1, &mHandle);

	mCreated = true;
}

void TransformFeedback::bindBufferBase(Buffer* buffer, uint32_t index){
	bind();

	// TODO probably not necessary
	buffer->bind();

	gl( BindBufferBase(GL_TRANSFORM_FEEDBACK, index, buffer->getHandle()) );
}

void TransformFeedback::destroy(){
	WT_ASSERT(mCreated, "Object not created");

	gl( DeleteTransformFeedbacks(1, &mHandle) );

	mHandle = 0;
	mCreated = false;
}

void TransformFeedback::bind(){
	gl( BindTransformFeedback(GL_TRANSFORM_FEEDBACK, mHandle) );
}

void TransformFeedback::unbind(){
	gl( BindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0) );
}

}; // </Gl>

}; // <wt>