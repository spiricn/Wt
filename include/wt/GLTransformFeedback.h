#pragma once
#ifndef WT_GLTRANSFORMFEEDBACK_H
#define WT_GLTRANSFORMFEEDBACK_H

#include "wt/stdafx.h"


namespace wt{
	
namespace Gl{

class Buffer;

class TransformFeedback{
public:
	TransformFeedback();

	~TransformFeedback();

	void create();

	void destroy();

	void bind();

	void bindBufferBase(Buffer* buffer, uint32_t index);

	static void unbind();

private:
	GLuint mHandle;
	bool mCreated;
}; // </TransformFeedback>

}; // </Gl>


}; // </wt>

#endif // </WT_GLTRANSFORMFEEDBACK_H>