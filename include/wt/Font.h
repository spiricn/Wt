#ifndef WT_FONT_H
#define WT_FONT_H


#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/Singleton.h"

//#pragma comment(lib, "freetype.lib")

namespace wt{

class FontManager;



class FontAtlas{
public:
	typedef struct CharInfo{
		/*** advance.x */
		float mAx;

		/** advance.y */
		float mAy; 

		/** bitmap.width */
		float mBw;

		/** bitmap.rows */
		float mBh;

		/** bitmap.left */
		float mBl;

		/** bitmap.top */
		float mBt;

		/** x offset of glyph in texture coordinates */
		float mTx;
	};

private:
	FT_Face mFace;;
	Texture2D mTexture;

	

	CharInfo mCharInfo[127];

public:

	Texture2D* getTexture(){
		return &mTexture;
	}

	const CharInfo& getCharInfo(char c) const{
		return mCharInfo[c];
	}

	void create(FT_Face face){
		mFace = face;

		FT_GlyphSlot slot = mFace->glyph;

		// determine the width/height required to store all the glyphs
		Int32 width = 0;
		Int32 height = 0;

		for(Int32 i=0; i<127; i++){
			WT_ASSERT(
			 !FT_Load_Char(mFace, i, FT_LOAD_RENDER), "Error loading glyph");

			width += slot->bitmap.width;
			height = std::max(height, slot->bitmap.rows);
		}

		// create atlas texture
		mTexture.create();
		mTexture.bind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		mTexture.setData(
			width, height, GL_ALPHA, GL_ALPHA, 0, GL_UNSIGNED_BYTE);

		Int32 x=0;
		for(Int32 i=0; i<127; i++){
			WT_ASSERT(
			 !FT_Load_Char(mFace, i, FT_LOAD_RENDER), "Error loading glyph");

			//memset(slot->bitmap.buffer, 0x30, slot->bitmap.width*slot->bitmap.rows);

			mTexture.setSubData(
				x, 0, slot->bitmap.width, slot->bitmap.rows, slot->bitmap.buffer);

			mCharInfo[i].mAx = slot->advance.x >> 6;
			mCharInfo[i].mAy = slot->advance.y >> 6;
			mCharInfo[i].mBw = slot->bitmap.width;
			mCharInfo[i].mBh = slot->bitmap.rows;
			mCharInfo[i].mBl = slot->bitmap_left;
			mCharInfo[i].mBt = slot->bitmap_top;
			mCharInfo[i].mTx = (float)x/(float)width; // normalized texture coord

			x += slot->bitmap.width;
		}
	}

	void measureText(const char* text, float* w, float* h){
		*w=0;
		*h=0;

		while(*text){
			CharInfo& info = mCharInfo[*text];
			*h =  std::max(*h, (float)info.mBt);
			*w += info.mAx;
			text++;
		}
	}

}; // </FontAtlas>

class Font : public AResource<Font>{
friend class FontManager;
private:
	FT_Face mFace;
	FT_Library* mLib;
	Texture2D mTexture;
	
protected:
	

	void setLib(FT_Library* lib){
		mLib = lib;
	}
public:
	FontAtlas mAtlas;

	Font(ResourceHandle handle=0, const std::string& name="") : AResource(handle, name){
		mTexture.create();
	}

	FT_Face& getFace(){
		return mFace;
	}

	unsigned char* getBmpBuffer(){
		return mFace->glyph->bitmap.buffer;
	}

	Texture2D* getTexture(){
		return &mTexture;
	}

	Uint32 getBmpWidth(){
		return mFace->glyph->bitmap.width;
	}

	Uint32 getBmpHeight(){
		return mFace->glyph->bitmap.rows;
	}

	void load(const String& path, Uint32 size){
		if(FT_New_Face(*mLib, path.c_str(), 0, &mFace)){
			WT_EXCEPT("Font", "Error loading font \"%s\"", path.c_str());
		}
		setSize(size);
		mAtlas.create(mFace);
	}

	glm::vec2 measureString(const String& string){
		float w,h;
		mAtlas.measureText(string.c_str(), &w, &h);

		return glm::vec2(w,h);
	}

	void renderChar(char c){
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if(FT_Load_Char(mFace, c, FT_LOAD_RENDER)){
			WT_EXCEPT("Font", "Error rendering char");
		}

		mTexture.setData(getBmpWidth(), getBmpHeight(),
			GL_ALPHA, GL_ALPHA, (const GLbyte*)getBmpBuffer());
	}

	FT_GlyphSlot& getGlyph(){
		return mFace->glyph;

	}

	FT_Glyph_Metrics& getMetrics(){
		return mFace->glyph->metrics;
	}

	void setSize(Uint32 size){
		FT_Set_Pixel_Sizes(mFace, 0, size);
	}

};




class FontManager : public Singleton<FontManager>, public AResourceManager<Font>{
private:
	FT_Library mFt;
	
public:
	FontManager(){
		if(FT_Init_FreeType(&mFt)){
			WT_EXCEPT("FontFactory", "Error initializing font library");
		}
		//FTC_Manager_New(mFt, 1, 1, 1024*1024, 
	}

	Font* create(const std::string& name){
		Font* res = AResourceManager<Font>::create(name);
		res->setLib(&mFt);
		return res;
	}
};


}; // </wt>

#endif
