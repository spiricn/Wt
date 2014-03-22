#include "wt/stdafx.h"
#include "wt/FontAtlas.h"

#define TD_TRACE_TAG "FontAtlas"

namespace wt
{

FontAtlas::FontAtlas(){
}

FontAtlas::~FontAtlas(){
}

Texture2D* FontAtlas::getTexture(){
	return &mTexture;
}

const FontAtlas::CharInfo& FontAtlas::getCharInfo(char c) const{
	return mCharInfo[c];
}

void FontAtlas::create(FT_Face face){
	mFace = face;

	FT_GlyphSlot slot = mFace->glyph;

	// determine the width/height required to store all the glyphs
	int32_t width = 0;
	int32_t height = 0;

	for(int32_t i=0; i<127; i++){
		WT_ASSERT(
			!FT_Load_Char(mFace, i, FT_LOAD_RENDER), "Error loading glyph");

		width += slot->bitmap.width + 1;
		height = std::max(height, slot->bitmap.rows);
	}

	// create atlas texture
	mTexture.create();
	mTexture.bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	mTexture.setData(
		width, height, GL_ALPHA, GL_ALPHA, 0, GL_UNSIGNED_BYTE);

	int32_t x=0;
	for(int32_t i=0; i<127; i++){
		WT_ASSERT(
			!FT_Load_Char(mFace, i, FT_LOAD_RENDER), "Error loading glyph");

		mTexture.setSubData(
			x, 0, slot->bitmap.width, slot->bitmap.rows, slot->bitmap.buffer);

		mCharInfo[i].advanceX = slot->advance.x >> 6;
		mCharInfo[i].advanceY = slot->advance.y >> 6;
		mCharInfo[i].width = slot->bitmap.width;
		mCharInfo[i].height = slot->bitmap.rows;
		mCharInfo[i].bearingX = slot->bitmap_left;
		mCharInfo[i].bearingY = slot->bitmap_top;

		// Normalized texture coordinates
		mCharInfo[i].coord.top = 0.0f;
		mCharInfo[i].coord.bottom = static_cast<float>(mCharInfo[i].height) / height;
		mCharInfo[i].coord.left = static_cast<float>(x)/width;
		mCharInfo[i].coord.right = static_cast<float>(x  + mCharInfo[i].width) / width;

		x += slot->bitmap.width + 1;
	}
}

void FontAtlas::measureText(const char* text, float* w, float* h){
	float width = 0;
	float height = 0;

	while(*text){
		CharInfo& info = mCharInfo[*text];
		height = std::max(height, static_cast<float>(info.height));
		width += info.advanceX;
		text++;
	}

	*w = width;
	*h = height;
}
	
} // </wt>

