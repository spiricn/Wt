#include "wt/stdafx.h"

#include "wt/ShaderPreprocessor.h"
#include "wt/Utils.h"
#include "wt/Exception.h"

#define INCLUDE_PRAGMA "#pragma include "

namespace wt
{

ShaderPreprocessor::ShaderPreprocessor(ModuleProviderFnc moduleProvider) : mModuleProvider(moduleProvider){
	if(mModuleProvider == NULL){
		mModuleProvider = &ShaderPreprocessor::defaultProvider;
	}
}

void ShaderPreprocessor::process(const String& source, String& result){
	processBuffer(source);

	result = mResult.str();

	mResult.clear();

	mResult = std::stringstream();
}

void ShaderPreprocessor::processBuffer(const String& buffer){
	std::stringstream stream(buffer);

	String line;
	do{
		std::getline(stream, line, '\n');
		processLine(line);
	}while(!stream.eof());
}

bool ShaderPreprocessor::defaultProvider(const String& src, String& res){
	try{
		utils::readFile(src, res);
		return true;
	}catch(...){
		return false;
	}
}

void ShaderPreprocessor::processLine(const String& l){
	// We need might need to alter it
	String line = l;

	// Remove \r if there is one
	if(line.size() && line[line.size() - 1] == '\r'){
		line.erase(line.size()-1);
	}

	if(strncmp(line.c_str(), INCLUDE_PRAGMA, strlen(INCLUDE_PRAGMA))==0){
		// transfer the entire file into a buffer and insert it into an existing source
		String includeFile = line.substr(strlen(INCLUDE_PRAGMA));

		String source;
		if(!mModuleProvider(includeFile, source)){
			WT_THROW("Unable to open shader include file \"%s\"", includeFile.c_str());
		}

		processBuffer(source);
	}
	else{
		mResult << line << "\n";
	}
}

} // </wt>