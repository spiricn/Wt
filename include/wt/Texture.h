#ifndef WT_TEXTURE_H
#define WT_TEXTURE_H


#include "wt/stdafx.h"

namespace wt{

class Texture{

public:
	static const String TAG;

private:
	GLuint mTexHandle;
	GLenum mType;
	bool mIsGenerated;

public:
	Texture(GLenum type);

	virtual ~Texture();

	bool isValid(){
		return mIsGenerated;
	}

	bool isCreated() const;

	void create();

	void destroy();

	void bind() const;

	void unbind() const;

	GLuint getTexHandle() const;

	GLenum getType() const;
};

}; // </wt>

#endif
