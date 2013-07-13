#ifndef WT_GLSHADER_H
#define WT_GLSHADER_H


#include "wt/stdafx.h"


#include "wt/Exception.h"
#include "wt/Utils.h"
#include "wt/Defines.h"

namespace wt{

namespace Gl{

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

	static void preprocess(const String& source, String& processed){
		processed = source;

		std::stringstream ss, ssource;
		ss.write(source.c_str(), source.size());
		ss.seekg(0);
		ss.seekp(0);

		String line;
		static const char* includePragma = "#pragma include ";
		do{
			std::getline(ss, line, '\n');

			/* pragma include line ? */
			if(strncmp(line.c_str(), includePragma, strlen(includePragma))==0){
				/* transfer the entire file into a buffer and insert it into an existing source */ 
				String includeFile = line.substr(strlen(includePragma));

				includeFile[includeFile.size()-1] = 0;
				String bfr;
				Utils::readFile(includeFile, bfr);

				ssource << bfr << '\n';
			}
			else{
				if(line.size() && line[line.size()-1] == '\r'){
					//line[line.size()-1] = 0;
					line.erase(line.size()-1);
				}
				ssource << line << "\n";
			}
		}while(!ss.eof());

#if 0
		{
			/* dump preprocessed shader source */
			static int i=0;
			i++;
			char bfr[30];
			sprintf(bfr, "shader_%d.i", i);

			std::ofstream t(bfr);
			t.write(ssource.str().c_str(), ssource.str().size());
			t.close();
		}
#endif

		processed = ssource.str();
	}

 }; // </GLShader>

}; // </Gl>

}; // </wt>
#endif
