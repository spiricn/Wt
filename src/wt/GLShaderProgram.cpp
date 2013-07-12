#include "wt/stdafx.h"
#include "wt/GLShaderProgram.h"
#include "wt/Log.h"
#include "wt/Exception.h"
#include "wt/Utils.h"

namespace wt{

namespace Gl{

const String ShaderProgram::TAG = "ShaderProgram";

void ShaderProgram::create(){
	mProgHandle = glCreateProgram();
}

void ShaderProgram::detach(Shader& shader){
	glDetachShader(mProgHandle, shader.getHandle());
}

void ShaderProgram::destroy(){
	glUseProgram(0);
	detach(mVertexShader);
	detach(mFragmentShader);
	if(mHasGeometryShader){
		detach(mGeometryShader);
	}

	glDeleteProgram(mProgHandle);
}

void ShaderProgram::createFromSources(const String& vertexSource, const String& fragmentSource, const String& geometrySource){
	create();

	mVertexShader.createFromSource(vertexSource);
	mFragmentShader.createFromSource(fragmentSource);

	if(geometrySource.size() != 0){
		mGeometryShader.createFromSource(geometrySource);
		mHasGeometryShader=true;
	}

	try{
		mVertexShader.compile();
		mFragmentShader.compile();

		attach(mVertexShader);
		attach(mFragmentShader);

		if(mHasGeometryShader){
			mGeometryShader.compile();
			attach(mGeometryShader);
		}

		if(mVertexShader.hasLog()){
			LOGW("GLShaderProgram", "Vertex shader compiled with warnings:\n%s", mVertexShader.getLog().c_str());
		}
		if(mFragmentShader.hasLog()){
			LOGW("GLShaderProgram", "Fragment shader compiled with warnings:\n%s", mFragmentShader.getLog().c_str());
		}
		if(mHasGeometryShader && mGeometryShader.hasLog()){
			LOGW("GLShaderProgram", "Geometry shader compiled with warnings:\n%s", mGeometryShader.getLog().c_str());
		}
	}
	catch(std::exception&){
		std::stringstream msg;
		msg << "Error compiling shader program";
		if(mVertexShader.hasLog()){
			msg << "\nVertex shader error:\n" << mVertexShader.getLog().c_str();
		}
		if(mFragmentShader.hasLog()){
			msg << "\nFragment shader error:\n" << mFragmentShader.getLog().c_str();
		}
		if(mHasGeometryShader && mGeometryShader.hasLog()){
			msg << "\nGeometry shader error:\n" << mGeometryShader.getLog().c_str();
		}

		WT_EXCEPT("GLShaderProgram", msg.str().c_str());
	}
}

void ShaderProgram::attach(Shader& shader){
	glAttachShader(mProgHandle, shader.getHandle());
}

void ShaderProgram::setTransformFeedbackVaryings(Uint32 count, ...){
	va_list argList;
	va_start(argList, count);

	const char** names = new const char*[count];

	for(Uint32 i=0; i<count; i++){
		names[i] = va_arg(argList, const char*);
	}

	glTransformFeedbackVaryings(mProgHandle, count, names, GL_INTERLEAVED_ATTRIBS);

	delete[] names;

	va_end(argList);
}

void ShaderProgram::createFromFiles(const String& vertexPath, const String& fragmentPath,  const String& geometryPath){
	String vertexSource, fragmentSource, geometrySource;
	
	Utils::readFile(vertexPath, vertexSource);
	Utils::readFile(fragmentPath, fragmentSource);
	if(geometryPath.size() != 0){
		Utils::readFile(geometryPath, geometrySource);
	}

		{
			String sourceProcessed;
			Shader::preprocess(fragmentSource, sourceProcessed);
			std::stringstream ss;
			ss << fragmentPath << ".preprocessed_tmp";
			std::ofstream out(ss.str().c_str());
			out << sourceProcessed;
			out.close();
		}


	createFromSources(vertexSource, fragmentSource, geometrySource);
}

bool ShaderProgram::isLinked(){
	GLint success;
	glGetProgramiv(mProgHandle, GL_LINK_STATUS, &success);

	return success==GL_FALSE?false:true;
}

bool ShaderProgram::hasLog(){
	GLint log_size=0;
	glGetProgramiv(mProgHandle, GL_INFO_LOG_LENGTH, &log_size);

	return log_size>1?true:false;
}

String ShaderProgram::getLog(){
	String program_log;

	if(hasLog()){
		// get log size
		GLint log_size=0;
		glGetProgramiv(mProgHandle, GL_INFO_LOG_LENGTH, &log_size);

		// copy log to a buffer
		char* log_bfr = new char[log_size];
		glGetProgramInfoLog(mProgHandle, log_size, NULL, log_bfr);

		// move log to member string & release memory
		program_log.append(log_bfr);
		program_log.append("\n");
		delete[] log_bfr;
	}

	return program_log;
}

/** int */
void ShaderProgram::setUniformVal(const String& name, int i){
	setUniformVal(getUniformLocation(name), i);
}

void ShaderProgram::setUniformVal(GLint location,  int i){
	glUniform1i(location, i);
}

/** float */
void ShaderProgram::setUniformVal(const String& name, float f){
	setUniformVal(getUniformLocation(name), f);
}

void ShaderProgram::setUniformVal(GLint location,  float f){
	glUniform1f(location, f);
}

/** glm::mat4x4& */
void ShaderProgram::setUniformVal(const String& name, const glm::mat4x4& matrix){ 
	setUniformVal(getUniformLocation(name), matrix); 
}

void ShaderProgram::setUniformVal(GLint location, const glm::mat4x4& matrix){
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

/** const glm::mat3x3 */
void ShaderProgram::setUniformVal(const String& name, const glm::mat3x3& matrix){
	setUniformVal(getUniformLocation(name), matrix);
}

void ShaderProgram::setUniformVal(GLint location, const glm::mat3x3& matrix){ 
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

/** glm::vec3 */
void ShaderProgram::setUniformVal(const String& name, const glm::vec3& vec){
	setUniformVal(getUniformLocation(name), vec);
}

void ShaderProgram::setUniformVal(GLint location, const glm::vec3& vec){
	glUniform3fv(location, 1, glm::value_ptr(vec));
}

/** glm::vec2 */
void ShaderProgram::setUniformVal(const String& name, const glm::vec2& vec){ 
	setUniformVal(getUniformLocation(name), vec);
}

void ShaderProgram::setUniformVal(GLint location, const glm::vec2& vec){ 
	glUniform2fv(location, 1, &vec.x); 
}

/** bool */
void ShaderProgram::setUniformVal(const String& name, bool val){
	setUniformVal(getUniformLocation(name), val?1:0);
}

/** glm::mat4x4 */
void ShaderProgram::setUniformVal(const String& name, const glm::mat4x4* matrices, Uint32 count){
	setUniformVal( getUniformLocation(name), matrices, count);
}

void ShaderProgram::setUniformVal(GLint location, const glm::mat4x4* matrices, Uint32 count){
	glUniformMatrix4fv(location, count, false, glm::value_ptr(matrices[0]));
}

/** wt::Color */
void ShaderProgram::setUniformVal(const String& name, const Color& val){
	setUniformVal( getUniformLocation(name), val);
}

void ShaderProgram::setUniformVal(GLint location, const Color& val){
	glUniform4fv(location, 1, val.getDataPtr());
}

GLint ShaderProgram::getUniformLocation(const String& name){
	int loc = glGetUniformLocation(mProgHandle, name.c_str());
	if(loc==-1){
		WT_EXCEPT(TAG,
			"Invalid uniform name \"%s\" in program \"%s\"\n", name.c_str(), ""
			);
	}

	return loc;
}

void ShaderProgram::bindAttribLocation(GLuint index, const String& name){
	glBindAttribLocation(mProgHandle, index, name.c_str());
}

GLuint ShaderProgram::getProgramHandle() const{
	return mProgHandle;
}

void ShaderProgram::link(){
	glLinkProgram(mProgHandle);

	if(!isLinked()){
		WT_EXCEPT(TAG,
			"Error linking shader program \"%s\" : \"%s\"", "", getLog().c_str()
			);
	}
}

void ShaderProgram::use(){
	glUseProgram(mProgHandle);
}

}; // </Gl>

}; // </wt>
