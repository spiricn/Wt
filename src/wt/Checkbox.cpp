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

Checkbox::Checkbox(View* parent, EventManager* eventManager, AGameInput* input) : mChecked(false), View(parent, eventManager, input){
	setBackgroundColor(Color::Gray());
}

void Checkbox::toggle(){
	setChecked( !isChecked() );
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
	
	// Base rect
	paint.setFillColor(Color::Gray());
	c.drawRect(0, 0, getWidth(), getHeight(), &paint);

	// Check box
	paint.setFillColor( Color::Cyan() );
	paint.setStyle( mChecked ? Paint::eSTYLE_FILL_AND_STROKE : Paint::eSTYLE_STROKE );
	c.drawRect(getSize().y/4, getSize().y/4, getSize().y/2, getSize().y/2, &paint);

	// Text
	glm::vec2 textSize = getFont()->measureString(mText);
	c.drawText(getFont(), mText, getSize().y, getSize().y/2.0f - textSize.y/2.0f, Color::Black(), 1.0f);
}

const EventType CheckboxClickedEvent::TYPE = "CheckboxClicked";

} // </gui>

} // </wt>
