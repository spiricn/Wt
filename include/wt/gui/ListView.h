#ifndef WT_LISTVIEW_H
#define WT_LISTVIEW_H

#include "wt/gui/View.h"

#include <wt/gui/SliderView.h>

namespace wt
{

namespace gui
{

class ListView : public View{
public:
	struct Item{
		void* userData;
		String name;
	}; //  </Item>

public:
	ListView(Layout* parent, EventManager* eventManager, AGameInput* input);

	virtual ~ListView();

	void addItem(const String& name, void* data=NULL);

	void draw(ICanvas& c);

	void onMouseDown(const MousePressEvent* evt);

	Item* getCurrentItem();

	void setActiveItem(Item* item);

private:
	typedef std::vector<Item*> ItemList;

private:
	void setActiveItem(uint32_t index);

	float getArrowWidth() const;

private:
	
	ItemList mItems;
	uint32_t mCurrentItem;
}; // </ListView>

class ListItemSelectedEvent : public AEvent{
public:
	ListView::Item* item;

public:
	ListItemSelectedEvent(ListView::Item* item) : item(item){
	}

	const EventType& getType() const{
		return TYPE;
	}
	
private:
	void serialize(LuaObject& dst){
		AEvent::serialize(dst);
	}

	void deserialize(LuaObject& src){
		AEvent::deserialize(src);
	}
	
public:
	static const EventType TYPE;
	
}; // </ListItemSelectedEvent>

} // </gui>

} // </wt>

#endif // </WT_LISTVIEW_H>
