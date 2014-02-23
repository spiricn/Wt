#include "wt/stdafx.h"

#include "wt/ShaderFactory.h"
#include "wt/CompiledShaders.h"

// Directory containing shader files (used if WT_USE_EMBEDDEDD_SHADERS is not defined)
#define SHADER_DIR "shaders"

namespace wt
{

static String getPath(const String& uri){
	if(uri.empty()){
		return uri;
	}
	else{
		return String(SHADER_DIR) + "/" + uri;
	}
}

void ShaderFactory::createShader(gl::ShaderProgram& program, const String& vertexUri, const String& fragmentUri, const String& geometryUri){
#ifdef WT_USE_EMBEDDED_SHADERS
	const char* vertexSrc = getShaderSource(vertexUri.c_str());
	const char* fragmentSrc = getShaderSource(fragmentUri.c_str());
	const char* geometrySrc = getShaderSource(geometryUri.c_str());

	program.createFromSources(vertexSrc == NULL ? "" : vertexSrc,
		fragmentSrc == NULL ? "" : fragmentSrc, geometrySrc == NULL ? "" : geometrySrc, &ShaderFactory::embeddedModuleProvider);
#else
	program.createFromFiles(getPath(vertexUri), getPath(fragmentUri), getPath(geometryUri), &ShaderFactory::embeddedModuleProvider);
#endif
}

bool ShaderFactory::embeddedModuleProvider(const String& src, String& res){
#ifdef WT_USE_EMBEDDED_SHADERS
	const char* source = getShaderSource(src.c_str());

	if(source){
		res = source;
	}
	else{
		TRACEW("No compiled shader with uri \"%s\" found", src.c_str());
	}

	return source != NULL;
#else
	try{
			utils::readFile(String(SHADER_DIR) + "/" + src, res);
		return true;
	}catch(...){
		return false;
	}
#endif
}

} // </wt>
