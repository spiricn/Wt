#ifndef WT_GUI_H
#define WT_GUI_H


#include "wt/stdafx.h"
#include "wt/FrameBuffer.h"

#include "wt/gui/View.h"
#include "wt/gui/ProgressView.h"
#include "wt/gui/Button.h"
#include "wt/gui/SliderView.h"
#include "wt/gui/CircleView.h"
#include "wt/gui/Checkbox.h"
#include "wt/gui/ListView.h"
#include "wt/EventManager.h"
#include "wt/EventEmitter.h"

namespace wt
{

namespace gui
{

class WindowManager;

class Layout : public View{
public:
	Layout(Layout* parent, EventManager* eventManager, AGameInput* input);

	~Layout();

	void setVisible(bool visible);

	bool isVisible() const;

	void setVerticalCellSpacing(float spacing);

	void setHorizontalCellSpacing(float spacing);

	void setDefaultFont(Font* font);

	void setGridSize(uint32_t numRows, uint32_t numColumns);

	void removeView(View* view);

	void setColor(const Color& color);

	void setDefaultScaleMode(View::ScalingMode mode);

	template<class T>
	T* createView(const String& name="");

	View* findView(const String& name);

	template<class T>
	T* findView(const String& name);

	View* viewAt(float x, float y);

	void draw(ICanvas& c);

	bool handleEvent(const EventPtr evt);

	void addView(View* view);

	void setSize(const glm::vec2& size);

	void setPosition(const glm::vec2& position);

private:
	typedef std::map<uint32_t, View*> ViewMap;

private:
	View* mFocus;
	ViewMap mViews;
	View* mHoverView;
	View* mDragView;
	View* mClickView;
	Font* mDefaultFont;
	uint32_t mNumGridRows;
	uint32_t mNumGridColumns;
	View::ScalingMode mDefaultScaleMode;
	bool mNeedsRescale;
	float mVerticalCellSpacing;
	float mHorizontalCellSpacing;
	bool mVisible;
}; // </Layout>

template<class T>
T* Layout::createView(const String& name){
	T* res = NULL;
	addView( res = new T(this, getEventManager(), getInput()) );
	res->setName(name);
	return res;
}

template<class T>
T* Layout::findView(const String& name){

	for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
		if(i->second->getName().compare(name)==0){
			return static_cast<T*>(i->second);
		}
	}

	return NULL;
}

} // </gui>

} // </wt>

#endif // </WT_LAYOUT_H>
