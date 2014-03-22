#ifndef WT_FONTATLAS_H
#define WT_FONTATLAS_H

#include "wt/Texture2D.h"

namespace wt
{

class FontAtlas{
public:
	struct CharInfo{
		/** Number of pixels before the next character should be drawn (i.e. nextX = x + advanceX) */
		float advanceX;

		float advanceY; 

		/** Character texture width */
		float width;

		/** Character texture height */
		float height;

		/** The distance from the origin to the left-most glyph scanline (i.e. x -> first pixel of the glyph) */
		float bearingX;

		/** The distance from the baseline to the top-most glyph scanline (i.e. y -> last pixel of the glyph upwards) */
		float bearingY;

		/** Normalized atlas texture coordinates of the character */
		struct{
			float left;
			float right;
			float top;
			float bottom;
		} coord;
	}; // </CharInfo>

	FontAtlas();

	virtual ~FontAtlas();

	Texture2D* getTexture();

	const CharInfo& getCharInfo(char c) const;

	void create(FT_Face face);

	void measureText(const char* text, float* w, float* h);

private:
	FT_Face mFace;;
	Texture2D mTexture;
	CharInfo mCharInfo[127];
}; // </FontAtlas>

} // </wt>

#endif // </WT_FONTATLAS_H>
