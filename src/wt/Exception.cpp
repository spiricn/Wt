#include "wt/stdafx.h"

#include "wt/Exception.h"

namespace wt{

Exception::Exception(const String& tag, const char* functionName,
	const char* fileName, long lineNumber, const char* descFmt, ...
	) :  mLineNumber(lineNumber), mDesc(descFmt),
	mTag(tag), mFile(fileName), mFunctionName(functionName){

		va_list argList;
		va_start(argList, descFmt);
		char bfr[1024];
		vsnprintf(bfr, 1024, descFmt, argList);
		va_end(argList);

		mDesc = bfr;

		LOGE(tag.c_str(), getFullDescription().c_str());
}

const String Exception::getFullDescription() const{
	std::stringstream s;
		s << mDesc << " in " << mFunctionName << " at " << mFile << " ( line " << mLineNumber << " )";

	return s.str();
}

}; // </wt>
