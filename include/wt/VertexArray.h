#ifndef WT_VERTEXARRAY_H
#define WT_VERTEXARRAY_H

namespace wt{

namespace Gl{

class VertexArray{
private:
	GLuint mHandle;
	bool mIsGenerated;

public:
	VertexArray();

	~VertexArray();

	void bind();

	void unbind();

	void create();

	void destroy();

}; // </VertexArray>

}; // </Gl>

}; // </wt>

#endif
