#include "wt/stdafx.h"

#include "wt/GLShaderProgram.h"
#include "wt/Exception.h"
#include "wt/Utils.h"
#include "wt/GLTrace.h"
#include "wt/ShaderPreprocessor.h"


#define TD_TRACE_TAG "ShaderProgram"

namespace wt{

namespace gl{

void ShaderProgram::create(){
	mProgHandle = glCreateProgram();
}

GLint ShaderProgram::getUniformLocationFmt(const String& nameFmt, ...){
	va_list args;
	va_start(args, nameFmt);

	GLint res = getUniformLocationVFmt(nameFmt, args);

	va_end(args);

	return res;
}

GLint ShaderProgram::getUniformLocationVFmt(const String& nameFmt, va_list args){
	char bfr[256] = {0};

	vsnprintf(bfr, 256, nameFmt.c_str(), args);

	return getUniformLocation(bfr);
}


void ShaderProgram::detach(Shader& shader){
	gl( DetachShader(mProgHandle, shader.getHandle()) );
}

GLuint ShaderProgram::getUniformBlockIndex(const String& name) const{
	return glGetUniformBlockIndex(mProgHandle, name.c_str());
}

void ShaderProgram::createUniformBlockBindPoint(const String& blockName, uint32_t point){
	GLuint blockIndex = getUniformBlockIndex(blockName);

	gl( UniformBlockBinding(mProgHandle, blockIndex, point) );
}

void ShaderProgram::destroy(){
	gl( UseProgram(0) );
	detach(mVertexShader);

	if(mHasFragmentShader){
		detach(mFragmentShader);
	}

	if(mHasGeometryShader){
		detach(mGeometryShader);
	}

	gl( DeleteProgram(mProgHandle) );
}

void ShaderProgram::getUniformIndices(uint32_t numUniforms, GLuint* indices, ...){
	va_list args;
	va_start(args, indices);

	const char** names = new const char*[numUniforms];

	for(int i=0; i<numUniforms; i++){
		const char* name = va_arg(args, const char*);
		names[i] = name;
	}

	gl( GetUniformIndices(mProgHandle, numUniforms, names, indices) );

	delete[] names;

	va_end(args);
}

void ShaderProgram::getActiveUniforms(uint32_t numIndices, GLuint* indices, GLenum name, GLint* res){
	gl( GetActiveUniformsiv(mProgHandle, numIndices, indices, name, res) );
}

void ShaderProgram::createFromSources(const String& vertexSource, const String& fragmentSource, const String& geometrySource, ShaderPreprocessor::ModuleProviderFnc moduleProvider){
	create();

	mVertexShader.createFromSource(vertexSource);

	if(fragmentSource.size() != 0){
		mFragmentShader.createFromSource(fragmentSource);
		mHasFragmentShader = true;
	}

	if(geometrySource.size() != 0){
		mGeometryShader.createFromSource(geometrySource);
		mHasGeometryShader=true;
	}

	try{
		// Vertex shader
		mVertexShader.compile();
		attach(mVertexShader);

		// Fragment shader
		if(mHasFragmentShader){
			mFragmentShader.compile();
			attach(mFragmentShader);
		}

		if(mHasGeometryShader){
			mGeometryShader.compile();
			attach(mGeometryShader);
		}

		if(mVertexShader.hasLog()){
			LOGW("GLShaderProgram", "Vertex shader compiled with warnings:\n%s", mVertexShader.getLog().c_str());
		}
		if(mHasFragmentShader && mFragmentShader.hasLog()){
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
		if(mHasFragmentShader && mFragmentShader.hasLog()){
			msg << "\nFragment shader error:\n" << mFragmentShader.getLog().c_str();
		}
		if(mHasGeometryShader && mGeometryShader.hasLog()){
			msg << "\nGeometry shader error:\n" << mGeometryShader.getLog().c_str();
		}

		WT_THROW("%s", msg.str().c_str());
	}
}

void ShaderProgram::attach(Shader& shader){
	gl( AttachShader(mProgHandle, shader.getHandle()) );
}

void ShaderProgram::setTransformFeedbackVaryings(uint32_t count, ...){
	va_list argList;
	va_start(argList, count);

	const char** names = new const char*[count];

	for(uint32_t i=0; i<count; i++){
		names[i] = va_arg(argList, const char*);
	}

	gl( TransformFeedbackVaryings(mProgHandle, count, names, GL_INTERLEAVED_ATTRIBS) );

	delete[] names;

	va_end(argList);
}

void ShaderProgram::createFromFiles(const String& vertexPath, const String& fragmentPath,  const String& geometryPath, ShaderPreprocessor::ModuleProviderFnc moduleProvider){
	LOGV("Creating from files vp=%s; fp=%s; gp=%", vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str());

	ShaderPreprocessor preprocessor(moduleProvider);

	String vertexSource, fragmentSource, geometrySource;
	String vertexProcessed, fragmentProcessed, geometryProcessed;

	// Read vertex source into a buffer and preprocess it
	utils::readFile(vertexPath, vertexSource);
	preprocessor.process(vertexSource, vertexProcessed);

#if 1
	std::ofstream out( "shaders/tmp/" + utils::getFileName(vertexPath) + "_processed.vp" );
	out << vertexProcessed;
	out.close();
#endif

	// Read fragment source into a buffer and preprocess it
	if(fragmentPath.size() != 0){
		utils::readFile(fragmentPath, fragmentSource);
		preprocessor.process(fragmentSource, fragmentProcessed);

#if 1
		std::ofstream out( "shaders/tmp/" + utils::getFileName(fragmentPath) + "_processed.fp" );
		out << fragmentProcessed;
		out.close();
#endif
	}

	// Read geometry source into a buffer and preprocess it
	if(geometryPath.size() != 0){
		utils::readFile(geometryPath, geometrySource);
		preprocessor.process(geometrySource, geometryProcessed);
#if 1
		std::ofstream out( "shaders/tmp/" + utils::getFileName(geometryPath) + "_processed.gp" );
		out << geometryProcessed;
		out.close();
#endif
	}

	createFromSources(vertexProcessed, fragmentProcessed, geometryProcessed);
}

bool ShaderProgram::isLinked(){
	GLint success;
	gl( GetProgramiv(mProgHandle, GL_LINK_STATUS, &success) );

	return success==GL_FALSE?false:true;
}

bool ShaderProgram::hasLog(){
	GLint log_size=0;
	gl( GetProgramiv(mProgHandle, GL_INFO_LOG_LENGTH, &log_size) );

	return log_size>1?true:false;
}

String ShaderProgram::getLog(){
	String program_log;

	if(hasLog()){
		// get log size
		GLint log_size=0;
		gl( GetProgramiv(mProgHandle, GL_INFO_LOG_LENGTH, &log_size) );

		// copy log to a buffer
		char* log_bfr = new char[log_size];
		gl(GetProgramInfoLog(mProgHandle, log_size, NULL, log_bfr));

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
	gl( Uniform1i(location, i) );
}

/** float */
void ShaderProgram::setUniformVal(const String& name, float f){
	setUniformVal(getUniformLocation(name), f);
}

void ShaderProgram::setUniformVal(GLint location,  float f){
	gl( Uniform1f(location, f) );
}

/** glm::mat4x4& */
void ShaderProgram::setUniformVal(const String& name, const glm::mat4x4& matrix){ 
	setUniformVal(getUniformLocation(name), matrix); 
}

void ShaderProgram::setUniformVal(GLint location, const glm::mat4x4& matrix){
	gl( UniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)) );
}

/** const glm::mat3x3 */
void ShaderProgram::setUniformVal(const String& name, const glm::mat3x3& matrix){
	setUniformVal(getUniformLocation(name), matrix);
}

void ShaderProgram::setUniformVal(GLint location, const glm::mat3x3& matrix){ 
	gl( UniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)) );
}

/** glm::vec3 */
void ShaderProgram::setUniformVal(const String& name, const glm::vec3& vec){
	setUniformVal(getUniformLocation(name), vec);
}

void ShaderProgram::setUniformVal(GLint location, const glm::vec3& vec){
	gl( Uniform3fv(location, 1, glm::value_ptr(vec)) );
}

/** glm::vec2 */
void ShaderProgram::setUniformVal(const String& name, const glm::vec2& vec){ 
	setUniformVal(getUniformLocation(name), vec);
}

void ShaderProgram::setUniformVal(GLint location, const glm::vec2& vec){ 
	gl( Uniform2fv(location, 1, &vec.x)) ;
}

/** bool */
void ShaderProgram::setUniformVal(const String& name, bool val){
	setUniformVal(getUniformLocation(name), val?1:0);
}

/** glm::mat4x4 */
void ShaderProgram::setUniformVal(const String& name, const glm::mat4x4* matrices, uint32_t count){
	setUniformVal( getUniformLocation(name), matrices, count);
}

void ShaderProgram::setUniformVal(GLint location, const glm::mat4x4* matrices, uint32_t count){
	gl( UniformMatrix4fv(location, count, false, glm::value_ptr(matrices[0])) );
}

/** wt::Color */
void ShaderProgram::setUniformVal(const String& name, const Color& val){
	setUniformVal( getUniformLocation(name), val);
}

void ShaderProgram::setUniformVal(GLint location, const Color& val){
	gl( Uniform4fv(location, 1, val.getDataPtr()) );
}

GLint ShaderProgram::getUniformLocation(const String& name){
	int loc = glGetUniformLocation(mProgHandle, name.c_str());
	if(loc==-1){
		LOGW("Invalid uniform name \"%s\"", name.c_str());
	}

	return loc;
}

void ShaderProgram::bindAttribLocation(GLuint index, const String& name){
	gl( BindAttribLocation(mProgHandle, index, name.c_str()) );
}

GLuint ShaderProgram::getProgramHandle() const{
	return mProgHandle;
}

void ShaderProgram::link(){
	gl( LinkProgram(mProgHandle) );

	if(!isLinked()){
		WT_THROW("Error linking shader program \"%s\"", getLog().c_str());
	}
}

void ShaderProgram::use(){
	gl( UseProgram(mProgHandle) );
}

}; // </gl>

}; // </wt>
