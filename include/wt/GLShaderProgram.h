#ifndef WT_GLSHADERPROGRAM_H
#define WT_GLSHADERPROGRAM_H

#include "wt/stdafx.h"

#include "wt/GLShader.h"
#include "wt/Exception.h"
#include "wt/Defines.h"
#include "wt/Color.h"
#include "wt/ShaderPreprocessor.h"

namespace wt
{

namespace gl
{

class ShaderProgram{
public:
	ShaderProgram();

	~ShaderProgram();

	void setTransformFeedbackVaryings(uint32_t count, ...);

	void getUniformIndices(uint32_t numUniforms, GLuint* indices, ...);

	void createFromSources(const String& vertex_source, 
		const String& fragment_source="", const String& geometrySource="", ShaderPreprocessor::ModuleProviderFnc moduleProvider=NULL);

	void createFromFiles(const String& vertexShaderFile,
		const String& fragmentShaderFile="", const String& geometryShaderFile="", ShaderPreprocessor::ModuleProviderFnc moduleProvider=NULL);

	void getActiveUniforms(uint32_t numIndices, GLuint* indices, GLenum name, GLint* res);

	bool isLinked();

	bool hasLog();

	String getLog();

	GLuint getUniformBlockIndex(const String& name) const;

	void createUniformBlockBindPoint(const String& blockName, uint32_t point);

	GLint getUniformLocationFmt(const String& nameFmt, ...);

	GLint getUniformLocationVFmt(const String& nameFmt, va_list);

	// int
	void setUniformVal(const String& name, int i);

	void setUniformVal(GLint location,  int i);

	// float
	void setUniformVal(const String& name, float f);

	void setUniformVal(GLint location,  float f);

	// glm::mat4x4
	void setUniformVal(const String& name, const glm::mat4x4& matrix);

	void setUniformVal(GLint location, const glm::mat4x4& matrix);

	// glm::mat3x3
	void setUniformVal(const String& name, const glm::mat3x3& matrix);

	void setUniformVal(GLint location, const glm::mat3x3& matrix);

	// glm::vec3
	void setUniformVal(const String& name, const glm::vec3& vec);

	void setUniformVal(GLint location, const glm::vec3& vec);

	// glm::vec2
	void setUniformVal(const String& name, const glm::vec2& vec);

	void setUniformVal(GLint location, const glm::vec2& vec);

	// glm::mat4x4 array
	void setUniformVal(const String& name, const glm::mat4x4* matrices, uint32_t count);

	void setUniformVal(GLint location, const glm::mat4x4* matrices, uint32_t count);

	// bool
	void setUniformVal(const String& name, bool val);

	// wt::Color
	void setUniformVal(const String& name, const Color& val);

	void setUniformVal(GLint location, const Color& val);

	GLint getUniformLocation(const String& name);

	void bindAttribLocation(GLuint index, const String& name);

	GLuint getProgramHandle() const;

	void link();

	void use();

	template<typename T>
	void setUniformValFmt(const T& val, const char* nameFmt, ...);

private:
	WT_DISALLOW_COPY(ShaderProgram)

	void create();

	void destroy();

	void detach(Shader& shader);

	void attach(Shader& shader);

private:
	Shader mVertexShader;
	Shader mFragmentShader;
	Shader mGeometryShader;
	String mLogs;
	String mLog;
	GLuint mProgHandle;
	bool mHasGeometryShader;
	bool mHasFragmentShader;
	bool mCreated;
}; // </GLShaderPRogram>

template<typename T>
void ShaderProgram::setUniformValFmt(const T& val, const char* nameFmt, ...){
	va_list args;
	va_start(args, nameFmt);

	char bfr[512] = {0};
	vsnprintf(bfr, 512, nameFmt, args);

	GLint loc = getUniformLocation(bfr);

	va_end(args);

	setUniformVal(loc, val);
}

} // </gl>

} // </wt>

#endif // </WT_GLSHADERPROGRAM_H>
