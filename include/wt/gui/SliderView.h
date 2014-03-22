#ifndef WT_SLIDERVIEW_H
#define WT_SLIDERVIEW_H

#include "wt/stdafx.h"

#include "wt/gui/View.h"

namespace wt{

namespace gui{

class SliderView : public View{
public:
	SliderView();

	void setValue(float val);

	float getValue() const;

	virtual void onMouseDrag(const MouseMotionEvent* evt);

	void draw(Canvas& c);

private:
	float mValue;
	float mBarWidth;
	Color mBaseColor;
	Color mSliderColor;
	Color mBorderColor;
}; // </SliderView>


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

} // </gui>

} // </wt>

#endif // </WT_SLIDERVIEW_H>
