#ifndef WT_EXCEPTION_H
#define WT_EXCEPTION_H

#include "wt/stdafx.h"

namespace wt{
	
class Exception : public std::exception{
private:
	long mLineNumber;
	String mDesc;
	String mTag;
	String mFunctionName;
	String mFile;

public:
	Exception(const String& tag, const char* functionName,
		const char* fileName, long lineNumber, const char* descFmt, ...
		);

	const String& getDescription() const{
		return mDesc;
	}

	const String getFullDescription() const;

	const String& getType() const;

	static void fatal(const char* fmt, ...){
		va_list argList;
		va_start(argList, fmt);
		char bfr[1024];
		vsnprintf(bfr, 1024, fmt, argList);
		va_end(argList);

		LOGW("Abort", fmt);
		abort();
	}

}; // </Exception>

}; // </wt>

#ifndef WT_ABORT
#define WT_ABORT(descFmt, ...) \
	do{ LOGE("Abort", descFmt, __VA_ARGS__); \
		abort(); ) while(0)
#endif

#ifndef WT_EXCEPT
#define WT_EXCEPT(tag, descFmt, ...) \
	do{ throw wt::Exception(tag, __FUNCTION__, __FILE__, __LINE__, \
			descFmt, __VA_ARGS__); }while(0)
#endif

#ifndef WT_THROW
#define WT_THROW(fmt, ...) \
	do{ throw wt::Exception("Exception", __FUNCTION__, __FILE__, __LINE__, \
			fmt, __VA_ARGS__); }while(0)
#endif

#define WT_QEXCEPT(descFmt, ...) WT_EXCEPT("Exception", descFmt, __VA_ARGS__)

#ifndef WT_ASSERT
#define WT_ASSERT(val, descFmt, ...) \
	do{ \
		if(!(val)){ \
			WT_QEXCEPT(descFmt, __VA_ARGS__); \
		} \
	}while(0)
#endif

#ifndef WT_ASSERT_EQ
#define WT_ASSERT_EQ(val1, val2) \
	do{ \
		if((val1) != (val2)){ \
			WT_QEXCEPT("Equality assertion failed %d != %d", val1, val2); \
		} \
	} \
	while(0)
#endif

#define WT_NOT_IMPLEMENTED WT_EXCEPT("NOT_YET_IMPLEMENTED", "%s %s at %d", __FILE__, __FUNCTION__, __LINE__)

#endif
