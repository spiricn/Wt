#ifndef WT_CHECKBOX_H
#define WT_CHECKBOX_H

#include "wt/gui/View.h"

namespace wt
{

namespace gui
{

class Checkbox : public View{
public:
	Checkbox(View* parent, EventManager* eventManager, AGameInput* input);

	Checkbox* setText(const String& text);

	Checkbox* setChecked(bool checked);

	void toggle();

	bool isChecked() const;

	void draw(ICanvas& c);

	void onClicked();

private:
	String mText;
	bool mChecked;

}; // </Checkbox>

class CheckboxClickedEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EventType TYPE;

	Checkbox* view;

	CheckboxClickedEvent(){
	}

	const EventType& getType() const{
		return TYPE;
	}

}; // </CheckboxClickedEvent>


}; // </gui>

}; // </wt>

#endif // </WT_CHECKBOX_H>