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
	};

	ListView(Layout* parent);

	virtual ~ListView();

	void addItem(const String& name, void* data=NULL);

	void draw(Canvas& c);

	void onMouseDrag(const MouseMotionEvent* evt);

protected:

private:
	typedef std::vector<Item*> ItemList;

	ItemList mItems;
	SliderView* mVerticalSlider;

}; // </ListView>

} // </gui>

} // </wt>

#endif // </WT_LISTVIEW_H>
