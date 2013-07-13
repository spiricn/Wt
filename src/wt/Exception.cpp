#include "wt/stdafx.h"

#include "wt/Exception.h"

#define TD_TRACE_TAG "Exception"

#pragma message "what the fuck"
namespace wt{

Exception::Exception(const char* functionName, const char* fileName, long lineNumber, const char* descFmt, ...) 
	:  mLineNumber(lineNumber), mDesc(descFmt), mFile(fileName), mFunctionName(functionName){

	va_list argList;
	va_start(argList, descFmt);

	// Format the message
	char bfr[1024];
	vsnprintf(bfr, 1024, descFmt, argList);
	va_end(argList);

	mDesc = bfr;

	LOGE("%s", getFullDescription().c_str());

}

const String& Exception::getDescription() const{
	return mDesc;
}

const String Exception::getFullDescription() const{
	std::stringstream s;
	s	<< "----------------------------\n"
		<< ">> " << mDesc << " <<\n"
		<< mFunctionName << " @ " << mLineNumber << "\n"
		<< mFile << "\n"
		<< "----------------------------\n";

	return s.str();
}

const String& Exception::getFileName() const{
	return mFile;
}

long Exception::getLineNumber() const{
	return mLineNumber;
}


const String& Exception::getFunctionName() const{
	return mFunctionName;
}

}; // </wt>
