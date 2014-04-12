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

Checkbox::Checkbox(Layout* parent, EventManager* eventManager, AGameInput* input) : mChecked(false), View(parent, eventManager, input){
	setBackgroundColor(Color::Gray());
}

Checkbox* Checkbox::setText(const String& text){
	dirty();

	mText = text;

	glm::vec2 size = getFont()->measureString(mText);
	size.y = ((getFont()->getFace()->bbox.yMax-getFont()->getFace()->bbox.yMin)>>6)/2;

	size += glm::vec2(size.y, 0);

	setSize(size);

	return this;
}

Checkbox* Checkbox::setChecked(bool checked){
	if(checked != mChecked){
		dirty();

		mChecked = checked;
	}

	return this;
}

bool Checkbox::isChecked() const{
	return mChecked;
}

void Checkbox::draw(ICanvas& c){
	Paint paint;
	paint.setStyle( Paint::eSTYLE_FILL );
	
	// Base rect
	c.drawRect(0, 0, getWidth(), getHeight(), &paint);

	// Outer circle
	paint.setFillColor( Color::White() );
	c.drawCircle(getSize().y/2, getSize().y/2, getSize().y/4, &paint);

	// Inner circle
	if(mChecked){
		paint.setFillColor( Color::Black() );
		c.drawCircle(getSize().y/2, getSize().y/2, getSize().y/8, &paint);
	}

	glm::vec2 textSize = getFont()->measureString(mText);

	c.drawText(getFont(), mText, getSize().y, getSize().y/2.0f - textSize.y/2.0f, Color::Black(), 1.0f);
}

const EventType CheckboxClickedEvent::TYPE = "CheckboxClicked";

} // </gui>

} // </wt>
