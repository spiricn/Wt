#ifndef WT_TEXTUREARRAY_H
#define WT_TEXTUREARRAY_H


#include "wt/stdafx.h"

#include "wt/Texture.h"
#include "wt/Image.h"
#include "wt/AResource.h"

namespace wt{

class TextureArray : public Texture{
private:
	Uint32 mNumTextures, mWidth, mHeight, mCurrTex;
	GLenum mFormat;
	bool mIsInitialized, mIsValid;

public:
	TextureArray() : Texture(GL_TEXTURE_2D_ARRAY), mWidth(0),
		mHeight(0), mNumTextures(0), mIsInitialized(false), mIsValid(false), mCurrTex(0){
	}

	virtual ~TextureArray(){
		destroy();
	}

	/** initializes the texture array (must be called prior to adding textures manually */
	void create(Uint32 width, Uint32 height, Uint32 num_textures, GLenum format);

	/** adds a texture to the array (must be called after initializing the array with create */
	void addTexture(const Image* image);

	/** cretes & adds all the necessary textures to the arary (uses the first image for format/dimensions info) */
	void create(Uint32 num, ...);

	Uint32 getNumTextures() const;

}; // </TextureArray>

}; // </wt>
#endif
