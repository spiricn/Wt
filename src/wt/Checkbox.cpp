#include "wt/stdafx.h"

#include "wt/gui/Checkbox.h"

#define TD_TRACE_TAG "Checkbox"

namespace wt
{

namespace gui
{

void Checkbox::onClicked(){
	setChecked(!mChecked);

	CheckboxClickedEvent* e = new CheckboxClickedEvent;
	e->view = this;

	emitEvent(e);
}

Checkbox::Checkbox(Window* parent) : mChecked(false), View(parent){
	setBackgroundColor(Color::Gray());
}

Checkbox* Checkbox::setText(const String& text){
	DIRTY;
	mText = text;

	glm::vec2 size = getFont()->measureString(mText);
	size.y = ((getFont()->getFace()->bbox.yMax-getFont()->getFace()->bbox.yMin)>>6)/2;

	size += glm::vec2(size.y, 0);

	setSize(size);

	return this;
}

Checkbox* Checkbox::setChecked(bool checked){
	if(checked != mChecked){
		DIRTY;

		mChecked = checked;
	}

	return this;
}

bool Checkbox::isChecked() const{
	return mChecked;
}

void Checkbox::draw(Canvas& c){
	View::draw(c);

	// Outer circle
	c.drawCircle(getSize().y/2, getSize().y/2, getSize().y/4, Color::White());

	// Inner circle
	if(mChecked){
		c.drawCircle(getSize().y/2, getSize().y/2, getSize().y/8, Color::Black());
	}

	glm::vec2 textSize = getFont()->measureString(mText);

	c.drawText(getFont(), mText, getSize().y, getSize().y/2.0f - textSize.y/2.0f, Color::Black());
}

const EventType CheckboxClickedEvent::TYPE = "CheckboxClicked";

} // </gui>

} // </wt>
