#ifndef WT_TEXTINPUT_H
#define WT_TEXTINPUT_H


#include "wt/stdafx.h"

#include "wt/Gui/View.h"

namespace wt{

namespace Gui{

class TextInput : public View{
private:
	String mText;
	Font* mFont;
public:
	TextInput(){
		mFont = FontManager::getSingleton().find("default");
	}

	void onKeyDown(VirtualKey key){
		char s[2] = {key, 0};

		mText.append(s);
		LOG("%s", mText.c_str());
	}

	const String& getText() const{
		return mText;
	}

	void clearText(){
		mText.clear();
	}

	void draw(Canvas& c){
		View::draw(c);
		c.drawText(mFont, mText,
			mPosition.x, mPosition.y, Color::Black());
	}

}; // </TextView>

}; // </Gui>

}; // </wt>

#endif
