#ifndef WT_SHADERPREPROCESSOR_H
#define WT_SHADERPREPROCESSOR_H

namespace wt
{

class ShaderPreprocessor{
public:
	typedef bool (*ModuleProviderFnc)(const String& src, String& res);

	ShaderPreprocessor(ModuleProviderFnc moduleProvider=NULL);

	void process(const String& source, String& result);

private:

	static bool defaultProvider(const String& src, String& res);

	void processBuffer(const String& buffer);

	void processLine(const String& l);

	std::stringstream mResult;
	ModuleProviderFnc mModuleProvider;

}; // </ShaderPreprocessor>

} // </wt>
#endif