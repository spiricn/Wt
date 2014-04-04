#ifndef WT_SLIDERVIEW_H
#define WT_SLIDERVIEW_H

#include "wt/stdafx.h"

#include "wt/gui/View.h"

namespace wt{

namespace gui{

class SliderView : public View{
public:
	enum Mode{
		eMODE_HORIZONTAL,
		eMODE_VERTICAL
	}; // </Mode>

	SliderView(Window* parent);

	void setValue(float val);

	float getValue() const;

	virtual void onMouseDrag(const MouseMotionEvent* evt);

	void draw(Canvas& c);

	void setMode(Mode mode);

private:
	float mValue;
	float mBarWidth;
	Color mBaseColor;
	Color mSliderColor;
	Color mBorderColor;
	Mode mMode;
}; // </SliderView>


class SliderValueChangedEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EventType TYPE;

	SliderView* view;
	float value;

	SliderValueChangedEvent(){
	}

	const EventType& getType() const{
		return TYPE;
	}

}; // </SliderValueChangedEvent>

} // </gui>

} // </wt>

#endif // </WT_SLIDERVIEW_H>
