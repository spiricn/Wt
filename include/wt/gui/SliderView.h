#ifndef WT_SLIDERVIEW_H
#define WT_SLIDERVIEW_H

#include "wt/stdafx.h"

#include "wt/Gui/View.h"

namespace wt{

namespace Gui{

class SliderView;

class SliderValueChangedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	SliderView* view;
	float value;

	SliderValueChangedEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </SliderValueChangedEvent>

class SliderView : public View{
private:
	float mValue, mBarWidth;

public:

	SliderView() : mValue(30.0f), mBarWidth(25.0f){
	}


	void setValue(float val){
		if(mValue != val){
			mValue = val;
			DIRTY
		}
	}

	float getValue() const{
		return mValue;
	}

	virtual void onMouseDrag(const MouseMotionEvent* evt){
		glm::vec2 pos = toLocalCoords(evt->mX, evt->mY);

		setValue( (pos.x/getSize().x)*100.0f );
		
		SliderValueChangedEvent* e = new SliderValueChangedEvent;
		e->value = mValue;
		e->view = this;
		
		emitEvent(e);
	}

	void draw(Canvas& c){
		// base rect
		c.drawRect(0, 0,
			getSize().x, getSize().y, Color(127.0/255.0, 127.0/255.0, 127.0/255.0));

		// progress rect
		float pos = getSize().x * (mValue/100.0f);

		c.drawRect(pos-mBarWidth/2.0f, 0,
			mBarWidth, getSize().y, Color(181.0/255.0, 230.0/255.0, 29.0/255.0));

		// border rect
		c.drawRect(0, 0,
			getSize().x, getSize().y, Color(195.0/255.0, 195.0/255.0, 195.0/255.0), Canvas::eRECT_LINE);
	}

}; // </SliderView>

}; // </Gui>

}; // </wt>

#endif // </WT_SLIDERVIEW_H>