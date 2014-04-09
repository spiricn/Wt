#include "wt/stdafx.h"
#include "wt/gui/ListView.h"

#define TD_TRACE_TAG "ListView"

#include <wt/gui/Layout.h>

namespace wt
{

namespace gui
{

ListView::ListView(Layout* parent) : View(parent), mCurrentItem(0){
}

ListView::~ListView(){
}

void ListView::addItem(const String& name, void* data){
	dirty();

	Item* item = new Item;

	item->name = name;
	item->userData = data;

	mItems.push_back(item);
}

void ListView::draw(Canvas& c){
	Canvas::Paint paint;

	// Background
	paint.style = Canvas::Paint::eSTYLE_FILL_AND_STROKE;
	c.drawRect(0, 0, getSize().x, getSize().y, &paint);

	float arrowWidth = getArrowWidth();
	
	// Left arrow
	paint.strokeColor = Color::Blue();
	c.drawRect(0, 0, arrowWidth, getHeight(), &paint);

	// Right arrow
	c.drawRect(getWidth() - arrowWidth, 0, arrowWidth, getHeight(), &paint);

	if(!mItems.empty()){
		Item* item = mItems[mCurrentItem];

		glm::vec2 textSize = getFont()->measureString(item->name);

		c.drawText(getFont(), item->name, getWidth()/2.0f - textSize.x/2.0f, getHeight()/2.0f - textSize.y/2.0f, Color::Black());
	}
}

void ListView::onMouseDown(const MousePressEvent* evt){
	glm::vec2 pos = toLocalCoords(evt->mX, evt->mY);

	float arrowWidth = getArrowWidth();

	if(pos.x <= arrowWidth){
		uint32_t index = mCurrentItem;

		if(index == 0){
			index = mItems.size() - 1;
		}
		else{
			index = mCurrentItem -1;
		}

		setActiveItem( index );
	}
	else if(pos.x >= getWidth() - arrowWidth){
		setActiveItem( ( mCurrentItem + 1 ) % mItems.size() );
	}
}

ListView::Item* ListView::getCurrentItem(){
	return mItems[mCurrentItem];
}

void ListView::setActiveItem(Item* item){
	uint32_t idx = (std::find(mItems.begin(), mItems.end(), item) - mItems.begin() );
	setActiveItem(idx);
}

void ListView::setActiveItem(uint32_t index){
	mCurrentItem = index;

	dirty();

	emitEvent( new ListItemSelectedEvent(mItems[mCurrentItem]) );
}

float ListView::getArrowWidth() const{
	return getWidth() * 0.1;
}

const EventType ListItemSelectedEvent::TYPE = "ListItemSelectedEvent";


} // </gui>

} // </wt>

