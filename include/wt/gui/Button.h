#ifndef WT_BUTTON_H
#define WT_BUTTON_H

#include "wt/gui/View.h"
#include "wt/gui/RectView.h"
#include "wt/IEventEmitter.h"

namespace wt{

namespace gui{

class Button : public RectView{
public:
	Button(Layout* parent);

	void onClicked();

	void setText(const String& text);

	void draw(Canvas& c);

private:
	void onMouseEnter(const MouseMotionEvent* evt);

	void onMouseLeave(const MouseMotionEvent* evt);

private:
	String mText;
	glm::vec2 mTextSize;

}; // </Button>


class ButtonClickedEvent : public AEvent{
public:
	static const EventType TYPE;

	Button* button;

	ButtonClickedEvent(){
	}

	const EventType& getType() const{
		return TYPE;
	}
}; // </ButtonClickedEvent>

}; // </gui>

}; // </wt>

#endif // </WT_BUTTON_H>