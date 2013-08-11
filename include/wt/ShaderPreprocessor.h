#ifndef WT_SHADERPREPROCESSOR_H
#define WT_SHADERPREPROCESSOR_H

#include "wt/stdafx.h"

namespace wt
{

class ShaderPreprocessor{
public:
	ShaderPreprocessor(){
	}

	void process(const String& source, String& result){
		processBuffer(source);

		result = mResult.str();

		mResult.clear();

		mResult = std::stringstream();
	}

private:
	void processBuffer(const String& buffer){
		std::stringstream stream(buffer);

		String line;
		do{
			std::getline(stream, line, '\n');
			processLine(line);
		}while(!stream.eof());
	}

	void processLine(const String& l){
		static const char* kINCLUDE_PRAGMA = "#pragma include ";

		// We need might need to alter it
		String line=l;

		if(strncmp(line.c_str(), kINCLUDE_PRAGMA, strlen(kINCLUDE_PRAGMA))==0){
			/* transfer the entire file into a buffer and insert it into an existing source */ 
			String includeFile = line.substr(strlen(kINCLUDE_PRAGMA));

			includeFile[includeFile.size()-1] = 0;
			String bfr;
			utils::readFile(includeFile, bfr);

			processBuffer(bfr);
		}
		else{
			if(line.size() && line[line.size()-1] == '\r'){
				line.erase(line.size()-1);
			}

			mResult << line << "\n";
		}
	}

	std::stringstream mResult;
}; // </ShaderPreprocessor>

} // </wt>
#endif