#include "wt/stdafx.h"

#include "wt/gui/Button.h"

#define TD_TRACE_TAG "Button"

namespace wt
{

namespace gui
{

Button::Button(View* parent, EventManager* eventManager, AGameInput* input) : View(parent, eventManager, input), mHovering(false){
}


void Button::onMouseEnter(const MouseMotionEvent* evt){
	dirty();

	mHovering = true;
}

void Button::onMouseLeave(const MouseMotionEvent* evt){
	dirty();

	mHovering = false;
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

void Button::draw(ICanvas& c){
	WT_ASSERT(getFont(), "No font specified for button instance");

	Paint paint;
	paint.setStyle( Paint::eSTYLE_FILL_AND_STROKE );

	paint.setFillColor( mHovering ? Color(0.85, 0.85, 0.85) : Color::White() );
	paint.setStrokeColor( Color::Black() );

	// Draw base
	c.drawRect(0, 0, getWidth(), getHeight(), &paint);

	// Draw text
	glm::vec2 pos = (getSize()/2.0f) - (mTextSize/2.0f);
	c.drawText(getFont(),
		mText, pos.x, pos.y, Color::Black(), 1.0f);
}

const EventType ButtonClickedEvent::TYPE = "ButtonClicked";

} // </gui>

} // </wt>
