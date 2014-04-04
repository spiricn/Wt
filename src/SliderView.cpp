#include "wt/stdafx.h"

#include "wt/gui/SliderView.h"

#define TD_TRACE_TAG "SliderView"

namespace wt
{

namespace gui
{

SliderView::SliderView(Window* parent) : mValue(30.0f), mBarWidth(25.0f), mBaseColor(127.0/255.0, 127.0/255.0, 127.0/255.0),
	mSliderColor(181.0/255.0, 230.0/255.0, 29.0/255.0), mBorderColor(195.0/255.0, 195.0/255.0, 195.0/255.0), mMode(eMODE_HORIZONTAL), View(parent){
}

void SliderView::setValue(float val){
	if(mValue != val){
		DIRTY;
		mValue = val;
	}
}

float SliderView::getValue() const{
	return mValue;
}

void SliderView::onMouseDrag(const MouseMotionEvent* evt){
	glm::vec2 pos = toLocalCoords(evt->mX, evt->mY);

	if(mMode == eMODE_HORIZONTAL){
		setValue( (pos.x/getSize().x)*100.0f );
	}
	else{
		setValue( (pos.y/getSize().y)*100.0f );
	}
		
	SliderValueChangedEvent* e = new SliderValueChangedEvent;
	e->value = mValue;
	e->view = this;
		
	emitEvent(e);
}

void SliderView::draw(Canvas& c){
	// Base rect
	c.drawRect(0, 0, getSize().x, getSize().y, mBaseColor);

	if(mMode == eMODE_HORIZONTAL){
		// Progress rect
		float pos = glm::mix(0.0f, getSize().x - mBarWidth, mValue/100.0f);

		c.drawRect(pos, 0, mBarWidth, getSize().y, mSliderColor);
	}
	else{
		float pos = glm::mix(0.0f, getSize().y - mBarWidth, mValue/100.0f);

		c.drawRect(0, pos, mBarWidth, getSize().x, mSliderColor);
	}

	// Border rect
	c.drawRect(0, 0, getSize().x, getSize().y, mBorderColor, Canvas::eRECT_LINE);
}

void SliderView::setMode(Mode mode){
	mMode = mode;
}

const EventType SliderValueChangedEvent::TYPE = "SliderValueChanged";

} // </gui>

} // </wt>
