#include "wt/stdafx.h"

#include "wt/VertexArray.h"
#include "wt/Exception.h"
#include "wt/GLTrace.h"

#define TD_TRACE_TAG "VertexArray"

namespace wt{

namespace gl{

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

	gl( BindVertexArray(mHandle) );
}

void VertexArray::unbind(){
	gl( BindVertexArray(0) );
}

void VertexArray::create(){
	#if WT_CHECKED
	if(mIsGenerated){
		WT_THROW("Called create() on VertexArray object more than once");
	}
	#endif

	if(!mIsGenerated){
		mIsGenerated=true;
		gl( GenVertexArrays(1, &mHandle) );
	}
}

void VertexArray::destroy(){
	if(mIsGenerated){
		gl( DeleteVertexArrays(1, &mHandle) );
		mIsGenerated=false;
	}
}

} // </gl>

} // </wt>
