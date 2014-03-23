#ifndef WT_BUTTON_H
#define WT_BUTTON_H

#include "wt/gui/View.h"
#include "wt/gui/RectView.h"

namespace wt{

namespace gui{

class Button;

class ButtonClickedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	Button* button;

	ButtonClickedEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </ButtonClickedEvent>


class Button : public RectView{
private:
	String mText;
	glm::vec2 mTextSize;

protected:
	void onMouseEnter(const MouseMotionEvent* evt){
		DIRTY;

		setBackgroundColor(Color(195/255.0, 195/255.0, 195/255.0));
	}

	void onMouseLeave(const MouseMotionEvent* evt){
		DIRTY;

		setBackgroundColor(Color(127/255.0, 127/255.0, 127/255.0));
	}

public:
	Button(Window* parent) : RectView(parent){
		setBackgroundColor(Color(127/255.0, 127/255.0, 127/255.0));

		setDrawBorder(true);
		setBorderWidth(3.0f);
		setBorderColor(Color(112/255.0, 146/255.0, 190/255.0));
	}


	void onClicked(){
		ButtonClickedEvent* evt = new ButtonClickedEvent;
		evt->button = this;
		evt->setEmitterData(getId());

		getEventManager()->queueEvent(evt);
	}

	void setText(const String& text){
		DIRTY;

		WT_ASSERT(getFont(), "No font specified for button instance");

		mText = text;

		mTextSize = getFont()->measureString(text);
	}

	void draw(Canvas& c){
		WT_ASSERT(getFont(), "No font specified for button instance");

		RectView::draw(c);

		glm::vec2 pos = (getSize()/2.0f) - (mTextSize/2.0f);

		c.drawText(getFont(),
			mText, pos.x, pos.y, Color::Black(), 1.0f);
	}
}; // </Button>

}; // </gui>

}; // </wt>

#endif // </WT_BUTTON_H>