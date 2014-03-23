#include "wt/stdafx.h"

#include "wt/Font.h"
#include "wt/FontAtlas.h"

#define TD_TRACE_TAG "Font"

namespace wt
{

Font::Font(AResourceManager<Font>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name), mAtlas(NULL){
	if(!sFtLibInitialized){
		if(FT_Init_FreeType(&sFtLib)){
			WT_THROW("Error initializing font library");
		}
		else{
			sFtLibInitialized = true;
		}
	}

	mAtlas = new FontAtlas;
}

Font::~Font(){
	delete mAtlas;
}

FT_Face& Font::getFace(){
	return mFace;
}

void Font::load(const String& path, uint32_t size){
	if(FT_New_Face(sFtLib, path.c_str(), 0, &mFace)){
		WT_THROW("Error loading font \"%s\"", path.c_str());
	}
	setSize(size);
	mAtlas->create(mFace);
}

glm::vec2 Font::measureString(const String& string, float fontSize){
	float w,h;
	mAtlas->measureText(string.c_str(), &w, &h);

	return glm::vec2(w*fontSize, h*fontSize);
}

FT_GlyphSlot& Font::getGlyph(){
	return mFace->glyph;
}

FT_Glyph_Metrics& Font::getMetrics(){
	return mFace->glyph->metrics;
}

void Font::setSize(uint32_t size){
	FT_Set_Pixel_Sizes(mFace, 0, size);
}

FontAtlas& Font::getFontAtlas(){
	return *mAtlas;
}

float Font::getLineHeight() const{
	return ( ( (mFace->bbox.yMax-mFace->bbox.yMin)>>6)/2 );
}

bool Font::sFtLibInitialized = false;

FT_Library Font::sFtLib = NULL;

} // </wt>
