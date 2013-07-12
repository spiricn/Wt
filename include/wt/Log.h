#ifndef WT_LOG_H
#define WT_LOG_H


#include "wt/stdafx.h"

#include "wt/Mutex.h"
#include "wt/Console.h"

namespace wt{

class ALogListener{
public:
	virtual bool log(int level, const String& tag, const String& text)=0;
}; // </ALogListener>

class Log{
public:
	enum Level {
		VERBOSE= ('V'<<8)|0,
		DEBUG= ('D'<<8)|1,
		INFO= ('I'<<8)|2,
		WARNING= ('W'<<8)|3,
		ERR= ('E'<<8)|4
	};

	

private:
	static const Uint32 LOG_BFR_SIZE = 2048;
	static Level mLevel;
	static FILE* mOutput;
	static ALogListener* mListener;
	static Mutex mMutex;

public:
	static void setListener(ALogListener* listener){
		mListener = listener;
	}

	static void setLevel(Level lvl){
		mLevel = lvl;
	}

	static void setOutput(FILE* file){
		mOutput = file;
	}

	static void log(Level lvl, const String& tag, const char* format, ...);

	static void printGlErrors(const char* file, long line, const char* expr);

}; // </Log>

}; // </wt>

#ifdef WT_DISABLE_LOGGING
	#define LOG(descFmt, ...)
	#define LOGV(TAG, descFmt, ...)
	#define LOGD(TAG, descFmt, ...)
	#define LOGI(TAG, descFmt, ...)
	#define LOGW(TAG, descFmt, ...)
	#define LOGE(TAG, descFmt, ...)
#else

	#define GLDBG(t) wt::Log::printGlErrors(t)
	#define LOG(descFmt, ...) wt::Log::log(wt::Log::VERBOSE, __FUNCTION__, descFmt, __VA_ARGS__)
	#define LOGIF(cond, descFmt, ...) do{ if(cond){ LOG(descFmt, __VA_ARGS__); } }while(0)
	#define LOGV(TAG, descFmt, ...) wt::Log::log(wt::Log::VERBOSE, TAG, descFmt, __VA_ARGS__)
	#define LOGD(TAG, descFmt, ...) wt::Log::log(wt::Log::DEBUG, TAG, descFmt, __VA_ARGS__)
	#define LOGI(TAG, descFmt, ...) wt::Log::log(wt::Log::INFO, TAG, descFmt, __VA_ARGS__)
	#define LOGW(TAG, descFmt, ...) wt::Log::log(wt::Log::WARNING, TAG, descFmt, __VA_ARGS__)
	#define LOGE(TAG, descFmt, ...) wt::Log::log(wt::Log::ERR, TAG, descFmt, __VA_ARGS__)
	#define DBGLOG wt::Log::log(wt::Log::DEBUG, "DEBUG", "%d: %s", __LINE__, __FUNCTION__)
#endif

#ifdef WT_GL_DEBUGGING_ENABLED
	#define gl(exp) do{ gl ## exp; wt::Log::printGlErrors(__FILE__, __LINE__, "gl" #exp); }while(0)
#else
	#define gl(exp) gl ## exp
#endif

#endif
