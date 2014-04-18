#include "wt/stdafx.h"
#include "wt/gui/TextView.h"
#include "wt/Font.h"
#include "wt/gui/Paint.h"

#define TD_TRACE_TAG "TextView"

namespace wt
{

namespace gui
{

TextView::TextView(View* parent, EventManager* eventManager, AGameInput* input) : mTextColor(Color::White()),
	mFontScale(1.0f), mScalingMode(eSCALE_FIXED), View(parent, eventManager, input), mPosFlags(0){
}

void TextView::setTextScaleMode(ScalingMode mode){
	dirty();

	mScalingMode = mode;
}

void TextView::setFontScale(float size){
	dirty();

	mFontScale = size;
}

void TextView::setText(const String& text){
	dirty();

	WT_ASSERT(getFont(), "No font specified for button instance");

	mText = text;
	if(mScalingMode == eSCALE_AUTO){
		glm::vec2 size = getFont()->measureString(text);
		size.y = ((getFont()->getFace()->bbox.yMax-getFont()->getFace()->bbox.yMin)>>6)/2;
		setSize(size);
	}
}

void TextView::setPositionFlags(PosFlag flags){
	mPosFlags = flags;
}

String TextView::getText() const{
	return mText;
}

void TextView::setTextColor(const Color& color){
	dirty();

	mTextColor = color;
}

void TextView::draw(ICanvas& c){
	WT_ASSERT(getFont(), "No font specified for TextView instance");

	// TODO hardcoded
	Paint backgroundPaint;
	backgroundPaint
		.setStyle( Paint::eSTYLE_FILL_AND_STROKE )
		.setFillColor( Color::White() )
		;

	c.drawRect(0, 0, getSize().x, getSize().y, &backgroundPaint);

	glm::vec2 textPos(0, 0);
	glm::vec2 textSize = getFont()->measureString(mText, mFontScale);

	if(mPosFlags & ePOS_CENTER_HORIZONTAL){
		textPos.x = ( getWidth() / 2.0f ) - ( textSize.x / 2.0f );
	}

	if(mPosFlags & ePOS_CENTER_VERTICAL){
		textPos.x = ( getHeight() / 2.0f ) - ( textSize.y / 2.0f );
	}
	
	c.drawTextFmt(getFont(), mText, textPos, getSize(), mTextColor, mFontScale);
}

} // </gui>
	
} // </wt>

