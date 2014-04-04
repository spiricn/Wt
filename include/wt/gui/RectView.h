#ifndef WT_RECTVIEW_H
#define WT_RECTVIEW_H

#include "wt/gui/View.h"

namespace wt{

namespace gui{

class RectView;

class RectViewClickedEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EventType TYPE;

	glm::vec2 point;
	RectView* view;

	RectViewClickedEvent(){
	}

	const EventType& getType() const{
		return TYPE;
	}

}; // </RectViewClickedEvent>



class RectView : public View{
private:
	Color mBorderColor;
	bool mBorderEnabled;
	float mBorderWidth;

public:

	RectView(Window* parent) : mBorderEnabled(true), mBorderColor(Color::Black()), mBorderWidth(1.0f), View(parent){
	}

	void setDrawBorder(bool state){
		DIRTY;

		mBorderEnabled = state;
	}

	void setBorderColor(const Color& clr){
		DIRTY;

		mBorderColor = clr;
	}

	void setBorderWidth(float width){
		DIRTY;

		mBorderWidth = width;
	}

	void onMouseDown(const MousePressEvent* e){
		RectViewClickedEvent* evt = new RectViewClickedEvent;
		evt->point = glm::vec2(e->mX, e->mY);
		evt->view = this;
		//evt->setEmitterData( getId() );

		//getEventManager()->emit(evt);
		TRACEW("TODO");
	}

	void draw(Canvas& c){
		View::draw(c);

		if(mBorderEnabled){
			c.drawLine(0, 0, getSize().x, 0, mBorderColor, mBorderWidth);
			c.drawLine(0, 0, 0, getSize().y, mBorderColor, mBorderWidth);
			c.drawLine(getSize().x, 0, getSize().x, getSize().y, mBorderColor, mBorderWidth);
			c.drawLine(0, getSize().y, getSize().x, getSize().y, mBorderColor, mBorderWidth);
		}
	}
};

}; // </gui>

}; // </wt>

#endif // </WT_RECTVIEW_H>