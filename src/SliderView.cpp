#include "wt/stdafx.h"

#include "wt/gui/SliderView.h"

#define TD_TRACE_TAG "SliderView"

namespace wt
{

namespace gui
{

SliderView::SliderView(Layout* parent) : mValue(30.0f), mBarWidth(25.0f), mBaseColor(127.0/255.0, 127.0/255.0, 127.0/255.0),
	mSliderColor(181.0/255.0, 230.0/255.0, 29.0/255.0), mBorderColor(195.0/255.0, 195.0/255.0, 195.0/255.0), mMode(eMODE_HORIZONTAL), View(parent){
}

void SliderView::setValue(float val){
	if(mValue != val){
		dirty();
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
	Canvas::Paint paint;

	paint.style = Canvas::Paint::eSTYLE_FILL_AND_STROKE;
	paint.fillColor = mBaseColor;
	paint.strokeColor = mBorderColor;

	// Base rect
	c.drawRect(0, 0, getSize().x, getSize().y, &paint);
	
	// Slider
	paint.style = Canvas::Paint::eSTYLE_FILL;
	paint.fillColor = mSliderColor;

	if(mMode == eMODE_HORIZONTAL){
		// Progress rect
		float pos = glm::mix(0.0f, getSize().x - mBarWidth, mValue/100.0f);

		c.drawRect(pos, 0, mBarWidth, getSize().y, &paint);
	}
	else{
		float pos = glm::mix(0.0f, getSize().y - mBarWidth, mValue/100.0f);

		c.drawRect(0, pos, mBarWidth, getSize().x, &paint);
	}
}

void SliderView::setMode(Mode mode){
	mMode = mode;
}

const EventType SliderValueChangedEvent::TYPE = "SliderValueChanged";

} // </gui>

} // </wt>
