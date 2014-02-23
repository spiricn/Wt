#ifndef WT_SHADER_FACTORY_H
#define WT_SHADER_FACTORY_H

#include <wt/GLShaderProgram.h>

namespace wt
{

class ShaderFactory{
public:

	static void createShader(gl::ShaderProgram& program, const String& vertexUri="", const String& fragmentUri="", const String& geometryUri="");

private:

	static bool embeddedModuleProvider(const String& src, String& res);

}; // </ShaderFactory>

} // </wt>

#endif // </WT_SHADER_FACTORY_H>
