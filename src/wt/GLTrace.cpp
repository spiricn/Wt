#include "wt/stdafx.h"

#include "wt/GLTrace.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "GLTracer"

namespace wt{

void printGlErrors(bool before, const char* file, long line, const char* expr){
	#ifdef __gl_h_
	GLenum glError;
	bool errOccured=false;
	do{
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			LOGE("Error %d (%s) occured at \"%s\" (line %d) %s a call to \"%s\"",
				(int)glError, (const char*)gluErrorString(glError), file, line, before ? "before" : "after", expr);

			errOccured=true;
		}
	}while(glError != GL_NO_ERROR);


	if(errOccured){
		WT_THROW("OpenGL error ocurred, aborting..");
	}

	#endif
}

}; // </wt>