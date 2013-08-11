#ifndef WT_GLSHADER_H
#define WT_GLSHADER_H

#include "wt/stdafx.h"

#include "wt/Exception.h"
#include "wt/Utils.h"
#include "wt/Defines.h"

namespace wt{

namespace gl{

class Shader{
private:
	GLuint mHandle;
	GLenum mType;
	String mLog;

	void create(){
		mHandle = glCreateShader(mType);
	}

	WT_DISALLOW_COPY(Shader);

public:
	Shader(GLenum type) : mType(type){
	}

	~Shader(){
	}

	GLuint getHandle();

	void destroy();

	void createFromSource(const String& source);

	void createFromFile(const String& file_path);

	void compile();

	bool isCompiled();

	bool hasLog();

	String getLog();

 }; // </GLShader>

}; // </gl>

}; // </wt>
#endif
