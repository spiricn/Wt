#ifndef WT_CHECKBOX_H
#define WT_CHECKBOX_H

#include "wt/gui/View.h"

namespace wt
{

namespace gui
{

class Checkbox : public View{
public:
	Checkbox();

	Checkbox* setText(const String& text);

	Checkbox* setChecked(bool checked);

	bool isChecked() const;

	void draw(Canvas& c);

	void onClicked();

private:
	String mText;
	bool mChecked;

}; // </Checkbox>

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


}; // </gui>

}; // </wt>

#endif // </WT_CHECKBOX_H>