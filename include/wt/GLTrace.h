#ifndef WT_GLTRACE_H
#define WT_GLTRACE_H

namespace wt{
	void printGlErrors(const char* file, long line, const char* expr);
}; // </wt>

#ifdef WT_GL_DEBUGGING_ENABLED
	#define gl(exp) do{ gl ## exp; wt::printGlErrors(__FILE__, __LINE__, "gl" #exp); }while(0)
#else
	#define gl(exp) gl ## exp
#endif

#endif // </WT_GLTRACE_H>