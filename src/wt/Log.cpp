#include "wt/stdafx.h"

#include "wt/Log.h"
#include "wt/Utils.h"

namespace wt{

void Log::log(Level lvl, const String& tag, const char* format, ...){
	mMutex.lock();

	if((lvl&0x00FF) < (mLevel&0x00FF)){
		mMutex.unlock();
		return;
	}

	static char bfr[LOG_BFR_SIZE];
	bfr[0] = 0;

	va_list argList;
	va_start(argList, format);

	vsnprintf(bfr, LOG_BFR_SIZE, format, argList);
	va_end(argList);


	String message = bfr;
	String line;
	do{
		// Split the message by lines
		Uint32 n = message .find("\n");
		if(n == String::npos){
			line = message;
			message.clear();
		}
		else{
			line = message.substr(0, n);
			String tmp = message.substr(n+1);
			message = tmp;
		}
		
		// Print the actual message
		if(mListener){
			// Warning: Possible deadlock if another LOG function is called from log callback
			if(!mListener->log(lvl, tag, line)){
				mMutex.unlock();
				return;
			}
		}

		

		Console::ConsoleColor clr;
		switch(lvl){
			case DEBUG:
			case VERBOSE:
				clr = Console::eWHITE;
				break;
			case INFO:
				clr = Console::eGREEN;
				break;
			case WARNING:
				clr = Console::eYELLOW;
				break;
			case ERR:
				clr = Console::eRED;
				break;
			default:
				clr = Console::eWHITE;
				break;
		}

		String timeStr = Utils::getCurrentTime("%H:%M:%S");

		Console::cpritnf(Console::eWHITE, Console::eBLACK, "[%s ", timeStr.c_str());

		Console::cpritnf(Console::eWHITE|Console::eINTENSE, clr, " %c ", (char)(lvl>>8));
		Console::cpritnf(Console::eWHITE, Console::eBLACK, " %s]:", tag.c_str());
		Console::cpritnf(clr|Console::eINTENSE, Console::eBLACK, " %s\r\n", line.c_str());
		fflush(stdout);

		if(mOutput != NULL){
			fprintf(mOutput, "[%s] (%c) %s: %s\n", timeStr.c_str(), (char)(lvl>>8), tag.c_str(), line.c_str());
			fflush(mOutput);
		}

		Console::setColor(Console::eWHITE, Console::eBLACK);
	}while(message.size());

	mMutex.unlock();
}

void Log::printGlErrors(const char* file, long line, const char* expr){
	#ifdef __gl_h_
	GLenum glError;
	bool errOccured=false;
	do{
		glError = glGetError();
		if(glError != GL_NO_ERROR){
			log(ERR, "OpenGL", "Error %d (%s) occured at \"%s\" (line %d) during a call to \"%s\"",
				(int)glError, (const char*)gluErrorString(glError), file, line, expr);
			errOccured=true;
		}
	}while(glError != GL_NO_ERROR);

	#endif
}

Log::Level Log::mLevel = Log::VERBOSE;
FILE* Log::mOutput= NULL;
ALogListener* Log::mListener = NULL;
Mutex Log::mMutex = Mutex();

}; // </wt>
