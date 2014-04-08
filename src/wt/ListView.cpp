#include "wt/stdafx.h"
#include "wt/gui/ListView.h"

#define TD_TRACE_TAG "ListView"

#include <wt/gui/Layout.h>

namespace wt
{

namespace gui
{

ListView::ListView(Layout* parent) : View(parent){
	setSize(300, 600);

	mVerticalSlider = new SliderView(NULL);

	mVerticalSlider->setSize(20, 600);
	mVerticalSlider->setMode(SliderView::eMODE_VERTICAL);
}

ListView::~ListView(){
}

void ListView::addItem(const String& name, void* data){
	Item* item = new Item;

	item->name = name;
	item->userData = data;

	mItems.push_back(item);
}

void ListView::draw(Canvas& c){
	// Background
	c.drawRect(0, 0, getSize().x, getSize().y, Color::Gray());

	uint32_t itemCount = 0;
	for(ItemList::iterator iter=mItems.begin(); iter!=mItems.end(); iter++){

		c.drawText(getFont(), (*iter)->name, 0, itemCount*getFont()->getLineHeight(), Color::Black());

		++itemCount;
	}

	mVerticalSlider->draw(c);
}

void ListView::onMouseDrag(const MouseMotionEvent* evt){
	mVerticalSlider->onMouseDrag(evt);
}

} // </gui>

} // </wt>

