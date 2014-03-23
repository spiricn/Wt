#include "wt/stdafx.h"
#include "wt/gui/TextView.h"

#define TD_TRACE_TAG "TextView"

namespace wt
{

namespace gui
{

TextView::TextView(Window* parent) : mTextColor(Color::White()), mFontScale(1.0f), mScalingMode(eSCALE_FIXED), View(parent){
}

void TextView::setScalingMode(ScalingMode mode){
	DIRTY;

	mScalingMode = mode;
}

void TextView::setFontScale(float size){
	DIRTY;

	mFontScale = size;
}

void TextView::setText(const String& text){
	DIRTY;

	WT_ASSERT(getFont(), "No font specified for button instance");

	mText = text;
	if(mScalingMode == eSCALE_AUTO){
		glm::vec2 size = getFont()->measureString(text);
		size.y = ((getFont()->getFace()->bbox.yMax-getFont()->getFace()->bbox.yMin)>>6)/2;
		setSize(size);
	}
}

String TextView::getText() const{
	return mText;
}

void TextView::setTextColor(const Color& color){
	DIRTY;

	mTextColor = color;
}

void TextView::draw(Canvas& c){
	WT_ASSERT(getFont(), "No font specified for TextView instance");

	View::draw(c);

	c.drawTextFmt(getFont(), mText, glm::vec2(0, 0), getSize(), mTextColor, mFontScale);
}

} // </gui>
	
} // </wt>

