#ifndef WT_FONT_H
#define WT_FONT_H

#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/Singleton.h"

namespace wt
{

class FontAtlas;

class Font : public AResource<Font>{
public:
	Font(AResourceManager<Font>* manager, ResourceHandle handle=0, const std::string& name="");

	~Font();

	FT_Face& getFace();

	void load(const String& path, uint32_t size);

	glm::vec2 measureString(const String& string, float fontSize=1.0f);

	FT_GlyphSlot& getGlyph();

	FT_Glyph_Metrics& getMetrics();

	float getLineHeight() const;

	void setSize(uint32_t size);

	FontAtlas& getFontAtlas();

private:
	FT_Face mFace;
	FontAtlas* mAtlas;

	static FT_Library sFtLib;
	static bool sFtLibInitialized;

}; // </Font>

} // </wt>

#endif // </WT_FONT_H>
