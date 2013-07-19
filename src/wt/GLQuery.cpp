#include "wt/stdafx.h"

#include "wt/GLQuery.h"
#include "wt/GLTrace.h"
#include "wt/Exception.h"

namespace wt{

namespace gl{

Query::Query() : mHandle(0), mQueryBegan(false){
	gl( GenQueries(1, &mHandle) );
}

Query::~Query(){
	gl( DeleteQueries(1, &mHandle) );
}

void Query::begin(GLenum target){
	WT_ASSERT(!mQueryBegan, "Query already began");

	gl( BeginQuery(target, mHandle) );

	mTarget = target;
	mQueryBegan = true;
}

void Query::end(){
	WT_ASSERT(mQueryBegan, "Query not began");

	glEndQuery(mTarget);
	mQueryBegan = false;
}

void Query::getResult(GLint* params){
	end();

	glGetQueryObjectiv(mHandle, GL_QUERY_RESULT, params);
}

void Query::getResult(GLuint* params){
	end();

	glGetQueryObjectuiv(mHandle, GL_QUERY_RESULT, params);
}

void Query::getResult(GLint64* params){
	end();

	glGetQueryObjecti64v(mHandle, GL_QUERY_RESULT, params);
}

void Query::getResult(GLuint64* params){
	end();

	glGetQueryObjectui64v(mHandle, GL_QUERY_RESULT, params);
}

}; // </gl>

}; // </wt>