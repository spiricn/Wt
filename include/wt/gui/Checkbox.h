#ifndef WT_CHECKBOX_H
#define WT_CHECKBOX_H

#include "wt/Gui/View.h"

namespace wt{

namespace Gui{

class Checkbox;

class CheckboxClickedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	Checkbox* view;

	CheckboxClickedEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </CheckboxClickedEvent>


class Checkbox : public View{
private:
	String mText;
	bool mChecked;

protected:
	void onClicked(){
		setChecked(!mChecked);

		CheckboxClickedEvent* e = new CheckboxClickedEvent;
		e->view = this;

		emitEvent(e);
	}

public:
	Checkbox() : mChecked(false){
		setBackgroundColor(Color::Gray());
	}

	Checkbox* setText(const String& text){
		DIRTY;
		mText = text;

		glm::vec2 size = getFont()->measureString(mText);
		size.y = ((getFont()->getFace()->bbox.yMax-getFont()->getFace()->bbox.yMin)>>6)/2;

		size += glm::vec2(size.y, 0);

		setSize(size);

		return this;
	}

	Checkbox* setChecked(bool checked){
		if(checked != mChecked){
			DIRTY;

			mChecked = checked;
		}

		return this;
	}

	bool isChecked() const{
		return mChecked;
	}

	void draw(Canvas& c){
		View::draw(c);

		c.drawCircle(getSize().y/2, getSize().y/2, getSize().y/2, Color::White());
		if(mChecked){
			c.drawCircle(getSize().y/2, getSize().y/2, getSize().y/4, Color::Black());
		}

		c.drawText(getFont(), mText, getSize().y, 0, Color::Black());
	}
};

}; // </Gui>

}; // </wt>

#endif // </WT_CHECKBOX_H>