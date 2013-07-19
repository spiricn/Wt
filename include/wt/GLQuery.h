#ifndef WT_GLQUERY_H
#define WT_GLQUERY_H

#include "wt/stdafx.h"

namespace wt{

namespace gl{

class Query{
public:
	Query();

	~Query();

	void begin(GLenum target);

	void end();

	void getResult(GLint* params);

	void getResult(GLuint* params);

	void getResult(GLint64* params);

	void getResult(GLuint64* params);

private:
	GLuint mHandle;
	bool mQueryBegan;
	GLenum mTarget;
};

}; // </gl>

}; // </wt>

#endif // </WT_GLQUERY_H>