#include "wt/stdafx.h"

#include "wt/GLShader.h"
#include "wt/Utils.h"

#include <sstream>

#define TD_TRACE_TAG "GLShader"

namespace wt{

namespace Gl{

void Shader::destroy(){
	glDeleteShader(mHandle);
}

GLuint Shader::getHandle(){
	return mHandle;
}

void Shader::createFromSource(const String& source){
	create();

	String sourceProcessed;
	preprocess(source, sourceProcessed);

	const GLchar* s[1] = {sourceProcessed.c_str()};// openGL requres us to use array of strings as an argument for glShaderSource
	glShaderSource(mHandle, 1, s, NULL);
}



void Shader::createFromFile(const String& path){
	String source;
	Utils::readFile(path, source);

	createFromSource(source);
}

void Shader::compile(){
	glCompileShader(mHandle);
	if(!isCompiled()){
		WT_THROW("Error compiling shader");
	}
}

bool Shader::isCompiled(){
	GLint success;
	glGetShaderiv(mHandle, GL_COMPILE_STATUS, &success);

	return success==GL_FALSE?false:true;
}

bool Shader::hasLog(){
	GLint log_size;
	glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &log_size);
	return log_size>1?true:false;
}

String Shader::getLog(){
	String log;

	// get shader log size
	GLint log_size;
	glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &log_size);

	// store the log in a buffer
	char* log_bfr = new char[log_size];
	glGetShaderInfoLog(mHandle, log_size, NULL, log_bfr);

	// transfer the buffer to a string object
	log = log_bfr;

	// release memory
	delete[] log_bfr;

	return log;
}

}; // </Gl>

}; // </wt>
