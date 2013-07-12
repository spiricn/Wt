#ifndef WT_EXCEPTION_H
#define WT_EXCEPTION_H

#include "wt/stdafx.h"

namespace wt{
	
class Exception : public std::exception{
private:
	long mLineNumber;
	String mDesc;
	String mFunctionName;
	String mFile;

public:
	Exception(const char* functionName, const char* fileName, long lineNumber, const char* descFmt, ...);
	
	const String& getDescription() const;

	const String getFullDescription() const;

	const String& getFileName() const;

	long getLineNumber() const;

	const String& getFunctionName() const;

}; // </Exception>

}; // </wt>

#define WT_THROW(fmt, ...) do{ throw wt::Exception(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); }while(0)

#define WT_ABORT(fmt, ...) do{ LOGE(fmt, ##__VA_ARGS__); abort(); }while(0)

#define WT_ASSERT(val, fmt, ...) do{ if(!(val)){ WT_THROW(fmt, __VA_ARGS__);} }while(0)

#endif
