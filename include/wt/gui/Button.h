#ifndef WT_BUTTON_H
#define WT_BUTTON_H

#include "wt/gui/View.h"
#include "wt/IEventEmitter.h"

namespace wt
{

namespace gui
{

class Button : public View{
public:
	Button(Layout* parent, EventManager* eventManager, AGameInput* input);

	void onClicked();

	void setText(const String& text);

	void draw(ICanvas& c);

private:
	void onMouseEnter(const MouseMotionEvent* evt);

	void onMouseLeave(const MouseMotionEvent* evt);

private:
	String mText;
	glm::vec2 mTextSize;
	bool mHovering;

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

} // </gui>

} // </wt>

#endif // </WT_BUTTON_H>
