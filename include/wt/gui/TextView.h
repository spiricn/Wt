#ifndef _H_TEXT_VIEW
#define _H_TEXT_VIEW

#include "wt/stdafx.h"

#include "wt/gui/View.h"

namespace wt{

namespace gui{


class TextView : public View{
public:
	enum ScalingMode{
		eAUTO,
		eFIXED

	}; // </ScalingMode>

private:
	Color mTextColor;
	StringList mLines;
	String mText;
	float mFontScale;
	ScalingMode mScalingMode;

public:

	TextView() : mTextColor(Color::White()), mFontScale(1.0f), mScalingMode(eFIXED){
	}

	void setScalingMode(ScalingMode mode){ DIRTY

		mScalingMode = mode;
	}

	void setFontScale(float size){ DIRTY

		mFontScale = size;
	}

	void setText(const String& text){ DIRTY
		WT_ASSERT(getFont(), "No font specified for button instance");

		mText = text;
		if(mScalingMode == eAUTO){
			glm::vec2 size = getFont()->measureString(text);
			size.y = ((getFont()->getFace()->bbox.yMax-getFont()->getFace()->bbox.yMin)>>6)/2;
			setSize(size);
		}
	}

	String getText() const{
		return mText;
	}

	void setTextColor(const Color& color){ DIRTY
		mTextColor = color;
	}

	void draw(Canvas& c){
		WT_ASSERT(getFont(), "No font specified for TextView instance");

		View::draw(c);
		c.drawTextFmt(getFont(), mText, glm::vec2(0, 0), getSize(), mTextColor);
	}

}; // </TextView>

}; // </gui>

}; // </wt>

#endif