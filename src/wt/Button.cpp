#include "wt/stdafx.h"

#include "wt/gui/Button.h"

#define TD_TRACE_TAG "Button"

namespace wt
{

namespace gui
{

void Button::onMouseEnter(const MouseMotionEvent* evt){
	dirty();

	setBackgroundColor(Color(195/255.0, 195/255.0, 195/255.0));
}

void Button::onMouseLeave(const MouseMotionEvent* evt){
	dirty();

	setBackgroundColor(Color(127/255.0, 127/255.0, 127/255.0));
}

Button::Button(Layout* parent) : RectView(parent){
	setBackgroundColor(Color(127/255.0, 127/255.0, 127/255.0));

	setDrawBorder(true);
	setBorderWidth(3.0f);
	setBorderColor(Color(112/255.0, 146/255.0, 190/255.0));
}


void Button::onClicked(){
	ButtonClickedEvent* evt = new ButtonClickedEvent;
	evt->button = this;

	getEventManager()->emitEvent(evt, this);
}

void Button::setText(const String& text){
	dirty();

	WT_ASSERT(getFont(), "No font specified for button instance");

	mText = text;

	mTextSize = getFont()->measureString(text);
}

void Button::draw(Canvas& c){
	WT_ASSERT(getFont(), "No font specified for button instance");

	RectView::draw(c);

	glm::vec2 pos = (getSize()/2.0f) - (mTextSize/2.0f);

	c.drawText(getFont(),
		mText, pos.x, pos.y, Color::Black(), 1.0f);
}

const EventType ButtonClickedEvent::TYPE = "ButtonClicked";

} // </gui>

} // </wt>
