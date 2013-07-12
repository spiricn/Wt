#include "wt/stdafx.h"

#include "wt/VertexArray.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "VertexArray"

namespace wt{

namespace Gl{

VertexArray::VertexArray() : mIsGenerated(false){
}

VertexArray::~VertexArray(){
	destroy();
}

void VertexArray::bind(){
	#if WT_CHECKED
	if(!mIsGenerated){
		WT_THROW("Called bind() on VertexArray object before initializing it with create()");
	}
	#endif

	glBindVertexArray(mHandle);
}

void VertexArray::unbind(){
	glBindVertexArray(0);
}

void VertexArray::create(){
	#if WT_CHECKED
	if(mIsGenerated){
		WT_THROW("Called create() on VertexArray object more than once");
	}
	#endif

	if(!mIsGenerated){
		mIsGenerated=true;
		glGenVertexArrays(1, &mHandle);
	}
}

void VertexArray::destroy(){
	if(mIsGenerated){
		glDeleteVertexArrays(1, &mHandle);
		mIsGenerated=false;
	}
}

} // </Gl>

} // </wt>
