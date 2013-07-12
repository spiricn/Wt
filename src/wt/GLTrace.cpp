#include "wt/stdafx.h"

#include "wt/GLTrace.h"

#define TD_TRACE_TAG "GLTracer"

namespace wt{

void printGlErrors(const char* file, long line, const char* expr){
	#ifdef __gl_h_
	GLenum glError;
	bool errOccured=false;
	do{
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			LOGE("Error %d (%s) occured at \"%s\" (line %d) during a call to \"%s\"",
				(int)glError, (const char*)gluErrorString(glError), file, line, expr);

			errOccured=true;
		}
	}while(glError != GL_NO_ERROR);

	#endif
}

}; // </wt>