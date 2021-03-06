#include "wt/stdafx.h"

#include "wt/Utils.h"
#include "wt/Color.h"
#include "wt/Buffer.h"
#include "wt/GLBatch.h"

#define TD_TRACE_TAG "Utils"

namespace wt
{

namespace utils
{
	
bool compareFiles(const String& file1Path, const String& file2Path){
	bool res = true;

	FILE* file1 = fopen(file1Path.c_str(), "rb");
	WT_ASSERT(file1, "Error oppening file1 \"%s\"", file1Path.c_str());

	FILE* file2 = fopen(file2Path.c_str(), "rb");
	WT_ASSERT(file2, "Error oppening file2 \"%s\"", file2Path.c_str());

	fseek(file1, 0, SEEK_END);
	int32_t size1 = ftell(file1);
	rewind(file1);

	fseek(file2, 0, SEEK_END);
	int32_t size2 = ftell(file2);
	rewind(file2);

	if(size1 != size2){
		res = false;
		goto cleanup;
	}

	static const int32_t kBUFFER_SIZE = 512;
	char bfr1[kBUFFER_SIZE];
	char bfr2[kBUFFER_SIZE];

	while(!feof(file1)){
		int32_t read1 = fread(bfr1, 1, kBUFFER_SIZE, file1);

		int32_t read2 = fread(bfr2, 1, kBUFFER_SIZE, file2);

		if(read1 != read2){
			res = false;
			goto cleanup;
		}

		if(read1 > 0){
			if(memcmp(bfr1, bfr2, read1) != 0){
				res = false;
				goto cleanup;
			}
		}
	}

cleanup:
	if(file1){
		fclose(file1);
	}

	if(file2){
		fclose(file2);
	}

	return res;
}

bool endsWith(const String& fullString, const String& ending){
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

String print(const char* fmt, ...){
	int bfrSize = 64;

	char* bfr = static_cast<char*>( malloc(bfrSize) );
	if(bfr == NULL){
		WT_THROW("Out of memory");
	}

	va_list args;
	va_start(args, fmt);

	String res;
	int rc = 0;

	do{
		rc = vsnprintf(bfr, bfrSize, fmt, args);

		if(rc >= 0 && rc < bfrSize){
			res = bfr;
		}
		else{
			bfrSize += bfrSize;
			char* tmp = static_cast<char*>( realloc(bfr, bfrSize) );
			if(tmp == NULL){
				WT_THROW("Out of memory");
			}
			else{
				bfr = tmp;
			}
		}
	}while(rc<0 || rc >= bfrSize);

	free(bfr);

	va_end(args);

	return res;
}

String formatSize(uint64_t size){
	char res[64];

	if(size < 1024){
		sprintf(res, "%ld B", size);
	}
	else if(size >= 1024 && size < 1024*1024){
		sprintf(res, "%ld KiB", size/(1024));
	}
	else if(size >= 1024*1024 && size < 1024*1024*1024){
		sprintf(res, "%ld MiB", size/(1024*1024));
	}
	else{
		sprintf(res, "%ld GiB", size/(1024*1024*1024));
	}

	return res;
}

String getFileName(const String& inPath){
	String path = inPath;

	// Extract shader name
	String name = path;
	path = replacePathSplitters(path, '/');

	size_t extPos = path.rfind(".");
	if(extPos != String::npos){
		path = path.substr(0, extPos);
	}

	size_t splitterPos = path.rfind("/");
	if(splitterPos != String::npos){
		path = path.substr(splitterPos + 1);
	}
	
	return path;
}

String getFileExt(const String& inPath){
	String path = inPath;
	path = replacePathSplitters(path, '/');

	size_t extPos = path.rfind(".");
	if(extPos != String::npos){
		path = path.substr(extPos + 1);
	}

	return path;
}

String replacePathSplitters(const String& src, char splitter){
	String res = src;

	char original = splitter=='/' ? '\\' : '/';

	for(uint32_t i=0; i<src.size(); i++){
		if(res[i] == original){
			res[i] = splitter;
		}
	}

	return res;
}

std::ostream& operator<<(std::ostream& dst, const Indent& indent){
	for(uint32_t i=0; i<indent.mDepth; i++){
		dst << '\t';
	}
	return dst;
}

String getEnv(const String& name){
	return getenv(name.c_str());
}

String formatSeconds(float seconds, const String& fmt){
	uint32_t sec = (uint32_t)seconds;

	struct tm t;
	t.tm_sec = sec % 60;
	t.tm_min = ((uint32_t)(sec/60.0f)) % 60;
	t.tm_hour = (uint32_t)(seconds/3600.0);

	char bfr[1024];
	strftime(bfr, 1024, fmt.c_str(), &t);

	return bfr;
}
void makeCheckboard(Buffer<unsigned char>& bfr, uint32_t w, uint32_t h,
	uint32_t vf, uint32_t hf, const Color& clr1, const Color& clr2){

	bfr.create(w*h*3);

	bool clr=false;
	for(uint32_t i=0; i<h; i++){
		for(uint32_t j=0; j<w; j++){
			int base = i*w*3 + j*3;
			if(j%hf==0){
				clr=!clr;
			}
			const Color& c = clr?clr1:clr2;
			bfr[base] = c.red*255.0;
			bfr[base+1] = c.green*255.0;
			bfr[base+2] = c.blue*255.0;
		}

		if(i%vf==0){
			clr=!clr;
		}
	}
}

String getCurrentTime(const String& format){
	time_t rawTime;
	struct tm* timeInfo;
	char bfr[128];

	time(&rawTime);
	timeInfo  = localtime(&rawTime);

	strftime(bfr, 128, format.c_str(), timeInfo);

	return bfr;
}

void makeGrid(gl::Batch& batch, float dx, float dz, uint32_t x, uint32_t z){
	float mWidth =(x-1)+(x-1)*dx;
	float mDepth =(z-1)+(z-1)*dz;

	//////////////////////////////////////////////////////////////////////
	uint32_t mNumVertices = x*z;
	uint32_t numQuads = (x-1)*(z-1);
	//////////////////////////////////////////////////////////////////////
	// Vertices
	float* vertices = new float[mNumVertices*3];

	GLuint* indices = new GLuint[numQuads*4];
	GLuint* index = indices;

	for(unsigned int i=0; i<x; i++){
		for(unsigned int j=0; j<z; j++){
			float* vertex = &vertices[(i*x+j)*3];

			vertex[0] = i+i*dx - mWidth/2;
			vertex[1] = 0.0f;
			vertex[2] = j+j*dz - mDepth/2;


			if(i==(x-1) || j==(z-1))
				continue;


			// Counter-clockwise wound for backface culling
			index[0] = i*x     + j;	
			index[1] = i*x     + j+1;
			index[2] = (i+1)*x + j+1;
			index[3] = (i+1)*x + j;

			index += 4;
		}
	}
		

	batch.create(
		GL_QUADS,
		vertices, mNumVertices, 3*sizeof(float),
		indices, numQuads*4, sizeof(GLuint),
		GL_UNSIGNED_INT);

	batch.setVertexAttribute(0, 3, GL_FLOAT, 0);

	delete[] vertices;
	delete[] indices;
}

void readFile(const String& path, String& dst){
	// open file stream for reading
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open()){
		WT_THROW("Error opening file for reading \"%s\"", path.c_str());
	}

	// determine file size & move source to buffer
	Uint64 fileSize;
	file.seekg(0, std::ios::end);
	fileSize = (Uint64)file.tellg();
	file.seekg(0);

	char* bfr = new char[fileSize+1];
	file.read(bfr, fileSize);
	file.close();

	// null terminator
	bfr[fileSize] = 0;

	dst = bfr;

	delete[] bfr;
}

String getString(){
	fflush(stdin);
	char bfr[256];
	bfr[0] = 0;
	gets(bfr);
	return bfr;
}

void renderAxis(const glm::mat4x4& modelView, const glm::mat4x4& projection){
	glUseProgram(0);

	// Modelview
	glm::mat4x4 mv;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(modelView));

	// Projection
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(projection));

	// Lines
	glEnable( GL_LINE_SMOOTH );
	//glTranslatef(0, 0, -5);
	//glLineWidth(5.0f);
	//glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
		glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0);
		glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0);
		glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1);
	glEnd();
	/*glEnable(GL_DEPTH_TEST);*/
}

void makeCube(gl::Batch& batch, float rad, GLuint posStream, GLuint texStream, GLuint normalStream){
	// 24 vertices, x,y,z, nx,ny, s,t
	const float vertices[] = {
		// Cube bottom
		-rad,-rad,-rad, 0.0f,-1.0f,0.0f, 0.0f,0.0f,
		rad,-rad,-rad, 0.0f,-1.0f,0.0f, 1.0f,0.0f,
		rad,-rad,rad, 0.0f,-1.0f,0.0f, 1.0f,1.0f,
		-rad,-rad,rad, 0.0f,-1.0f,0.0f, 0.0f,1.0f,
		// Cube top
		-rad,rad,-rad, 0.0f,1.0f,0.0f, 0.0f,1.0f,
		-rad,rad,rad, 0.0f,1.0f,0.0f, 0.0f,0.0f,
		rad,rad,rad, 0.0f,1.0f,0.0f, 1.0f,0.0f,
		rad,rad,-rad, 0.0f,1.0f,0.0f, 1.0f,1.0f,
		// Cube left side
		-rad,-rad,-rad, -1.0f,0.0f,0.0f, 0.0f,0.0f,
		-rad,-rad,rad, -1.0f,0.0f,0.0f, 1.0f,0.0f,
		-rad,rad,rad, -1.0f,0.0f,0.0f, 1.0f,1.0f,
		-rad,rad,-rad, -1.0f,0.0f,0.0f, 0.0f,1.0f,
		// Cube right side
		rad,-rad,rad, 1.0f,0.0f,0.0f, 0.0f,0.0f,
		rad,-rad,-rad, 1.0f,0.0f,0.0f, 1.0f,0.0f,
		rad,rad,-rad, 1.0f,0.0f,0.0f, 1.0f,1.0f,
		rad,rad,rad, 1.0f,0.0f,0.0f, 0.0f,1.0f,
		// Cube front side
		-rad,-rad,rad, 0.0f,0.0f,1.0f, 0.0f,0.0f,
		rad,-rad,rad, 0.0f,0.0f,1.0f, 1.0f,0.0f,
		rad,rad,rad, 0.0f,0.0f,1.0f, 1.0f,1.0f,
		-rad,rad,rad, 0.0f,0.0f,1.0f, 0.0f,1.0f,
		// Cube back side
		-rad,-rad,-rad, 0.0f,0.0f,-1.0f, 1.0f,0.0f,
		-rad,rad,-rad, 0.0f,0.0f,-1.0f, 1.0f,1.0f,
		rad,rad,-rad, 0.0f,0.0f,-1.0f, 0.0f,1.0f,
		rad,-rad,-rad, 0.0f,0.0f,-1.0f, 0.0f,0.0f
		// 0 1 2    3 4 5    6 7
	};
			
	// 24 indices
	const static GLushort indices[] = {
		// bottom
		0,1,2,3,
		// top
		4,5,6,7,
		// left
		8,9,10,11,
		// right
		12,13,14,15,
		// front
		16,17,18,19,
		// back
		20,21,22,23
	};

	batch.create(
			GL_QUADS,
			vertices, 24, 8*sizeof(float),
			indices, 24, sizeof(GLushort),
			GL_UNSIGNED_SHORT);

	// Position
	batch.setVertexAttribute(posStream, 3, GL_FLOAT, 0);

	// Texture
	batch.setVertexAttribute(texStream, 2, GL_FLOAT, 6*sizeof(float));

	// Normal
	batch.setVertexAttribute(normalStream, 3, GL_FLOAT, 4*sizeof(float));
}

void renderGrid(const glm::mat4x4& modelView, const glm::mat4x4& projection){
	static bool batchInitialized=false;
	static gl::Batch batch;

	if(!batchInitialized){
		makeGrid(batch, 2.0f, 2.0f, 15, 15);
		batchInitialized=true;
	}

	glUseProgram(0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);

	// Push modelview
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(glm::value_ptr(modelView)); 

	// Push projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(glm::value_ptr(projection));


	glPushAttrib(GL_POLYGON_BIT);

	// Color layer
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);
	glColor4f(.3f, .3f, .3f, 0.5f);
	batch.render();
	glDepthMask(true);

	glDisable(GL_BLEND);
	glColor3f(0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	batch.render();

	// Restore matrices & attributes
	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void setHomeDir(const String& path){
	#if defined(WIN32)
		WT_ASSERT(_chdir(path.c_str())==0, "Error setting home directory to \"%s\"", path.c_str());
	#else
		#error "setHomeDir" not yet implemented
	#endif
}

void pause(){
#if defined(WIN32)
	_getch();
#else

	#error Not implemented on this platform

#endif
}

void sleep(float time){
#if defined(WIN32)
		Sleep((unsigned long)(time*1000));
#else

	#error Not implemented on this platform

#endif
}

String toRelative(const String& base, const String& path){
#if defined(WIN32)

	if(base.empty()){
		return path;
	}

	String b = base;
	b = utils::replacePathSplitters(b, '/');
	if(b[b.size()-1] != '/'){
		b.append("/");
	}
	std::transform(b.begin(), b.end(), b.begin(), ::tolower);

	String p = path;
	p = utils::replacePathSplitters(p, '/');
	std::transform(p.begin(), p.end(), p.begin(), ::tolower);

	if(p.size() > b.size()){
		String t = p.substr(0, b.size());
		if(t.compare(b)==0){
			// it's relative
			return p.substr(b.size());
		}
		else{
			return path;
		}
	}
	else{
		return path;
	}

#else

	#error Not implemented on this platform

#endif
}

String getPathBaseName(const String& path){
	String tmp = utils::replacePathSplitters(path, '/');
	size_t pos = tmp.rfind('/');
	if(pos == String::npos){
		return path;
	}
	else{
		return tmp.substr(pos + 1);
	}
}


String getHomeDir(){
#if defined(WIN32)
	char bfr[MAX_PATH];
	getcwd(bfr, MAX_PATH);
	return bfr;
#else

	#error Not implemented on this platform

#endif
}

} // </Utils>

} // </wt>
