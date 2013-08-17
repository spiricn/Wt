#ifndef WT_GLSHADERPROGRAM_H
#define WT_GLSHADERPROGRAM_H


#include "wt/stdafx.h"

#include "wt/GLShader.h"
#include "wt/Exception.h"
#include "wt/Defines.h"
#include "wt/Color.h"

namespace wt{

namespace gl{

class ShaderProgram{
private:
	WT_DISALLOW_COPY(ShaderProgram)

	Shader mVertexShader, mFragmentShader, mGeometryShader;
	String mLogs, mLog;
	GLuint mProgHandle;
	bool mHasGeometryShader;
	bool mHasFragmentShader;

	void create();

	void destroy();

	void detach(Shader& shader);

	void attach(Shader& shader);

public:
	ShaderProgram() : mVertexShader(GL_VERTEX_SHADER),
		mFragmentShader(GL_FRAGMENT_SHADER), mGeometryShader(GL_GEOMETRY_SHADER), mHasGeometryShader(false), mHasFragmentShader(false){
	}

	~ShaderProgram(){
		destroy();
	}

	void setTransformFeedbackVaryings(uint32_t count, ...);

	void getUniformIndices(uint32_t numUniforms, GLuint* indices, ...);

	void createFromSources(const String& vertex_source, const String& fragment_source="", const String& geometrySource="");

	void getActiveUniforms(uint32_t numIndices, GLuint* indices, GLenum name, GLint* res);

	void createFromFiles(const String& vertexShaderFile,
		const String& fragmentShaderFile="", const String& geometryShaderFile="");

	bool isLinked();

	bool hasLog();

	String getLog();

	GLuint getUniformBlockIndex(const String& name) const;

	void createUniformBlockBindPoint(const String& blockName, uint32_t point);

	template<typename T>
	void setUniformValFmt(const T& val, const char* nameFmt, ...);

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


}; // </GLShaderPRogram>


template<class T>
class ShaderUniform{
private:
	ShaderProgram* mProgram;
	GLint mLocation;
	T mValue;
public:
	ShaderUniform(ShaderProgram* program, const String& name) : mProgram(program){
		mLocation = mProgram->getUniformLocation(name);
	}

	ShaderUniform& operator=(const T& value){
		mProgram->setUniformVal(mLocation, value);
		return *this;
	}

};

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

}; // </gl>


}; // </wt>
#endif
