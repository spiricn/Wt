#ifndef WT_UTILS_H
#define WT_UTILS_H

namespace wt
{

// <forward declaration>
class Color;

namespace gl{
	class Batch;
}

template<class T>
class Buffer;

// </forward declaration>

namespace utils{

	struct Indent{
		uint32_t mDepth;
	
		Indent(uint32_t depth) : mDepth(depth){
		}
	};


	bool compareFiles(const String& file1, const String& path2);

	String formatSeconds(float seconds, const String& fmt);

	bool endsWith(const String& fullString, const String& ending);

	std::ostream& operator<<(std::ostream& dst, const Indent& indent);

	String getFileName(const String& path);

	String getFileExt(const String& path);

	String getEnv(const String& name);

	String print(const char* fmt, ...);

	void makeCheckboard(Buffer<unsigned char>& bfr, uint32_t w, uint32_t h,
		uint32_t vf, uint32_t hf, const Color& clr1, const Color& clr2);

	String getCurrentTime(const String& format="%H-%M-%S-%d-%b-%Y");

	void makeGrid(gl::Batch& batch, float dx, float dz, uint32_t x, uint32_t z);

	void readFile(const String& path, String& dst);

	String getString();

	void renderAxis(const glm::mat4x4& modelView, const glm::mat4x4& projection);

	void makeCube(gl::Batch& batch, float rad=1, GLuint posStream=0, GLuint texStream=1, GLuint normalStream=2);

	void renderGrid(const glm::mat4x4& modelView, const glm::mat4x4& projection);

	void setHomeDir(const String& path);

	String getHomeDir();

	String replacePathSplitters(const String& src, char splitter);

	String toRelative(const String& base, const String& path);

	void pause();

	void sleep(float time);

	String formatSize(uint64_t size);

	String getPathBaseName(const String& path);

} // </Utils>

} // </wt>

#endif
