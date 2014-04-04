#ifndef WT_GUI_H
#define WT_GUI_H


#include "wt/stdafx.h"
#include "wt/FrameBuffer.h"

#include "wt/gui/View.h"
#include "wt/gui/ProgressView.h"
#include "wt/gui/Button.h"
#include "wt/gui/RectView.h"
#include "wt/gui/SliderView.h"
#include "wt/gui/CircleView.h"
#include "wt/gui/Checkbox.h"

#include "wt/EventManager.h"
#include "wt/EventEmitter.h"

namespace wt
{

namespace gui
{

class Window : public IEventListener{
public:
	Window();

	~Window();

	void setVisible(bool visible);

	bool isVisible() const;

	void setVerticalCellSpacing(float spacing);

	void setHorizontalCellSpacing(float spacing);

	void setDefaultFont(Font* font);

	void setGridSize(uint32_t numRows, uint32_t numColumns);

	void setInput(AGameInput* input);

	Canvas& getCanvas();

	void hook(EventManager* evtManager);

	void removeView(View* view);

	void setDefaultScaleMode(View::ScalingMode mode);

	template<class T>
	T* createView(const String& name="");

	View* findView(const String& name);

	template<class T>
	T* findView(const String& name);

	bool handleEvent(const Sp<AEvent> evt);

	View* viewAt(float x, float y);

	void create(uint32_t w, uint32_t h);

	void draw();

private:
	View* mFocus;
	EventEmitter mEventEmitter;
	typedef std::map<uint32_t, View*> ViewMap;
	ViewMap mViews;
	Canvas mCanvas;
	EventManager* mEventManager;
	View* mHoverView;
	View* mDragView;
	View* mClickView;
	Font* mDefaultFont;
	AGameInput* mInput;
	uint32_t mNumGridRows, mNumGridColumns;
	View::ScalingMode mDefaultScaleMode;
	bool mNeedsRescale;
	float mVerticalCellSpacing, mHorizontalCellSpacing;
	bool mVisible;


}; // </Window>

template<class T>
T* Window::createView(const String& name){
	uint32_t id = mViews.size();
	while(true){
		if(mViews.find(id) == mViews.end()){
			break;
		}
		id++;
	}

	T* tView = new T(this);
	View* view = static_cast<View*>(tView);
	view->setScalingMode(mDefaultScaleMode);
	view->setId(id);
	view->setName(name);
	view->setEventManager(mEventManager);

	if(view->getFont() == NULL){
		view->setFont(mDefaultFont);
	}

	mViews.insert(std::make_pair(id, view));

	return tView;
}

template<class T>
T* Window::findView(const String& name){

	for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
		if(i->second->getName().compare(name)==0){
			return static_cast<T*>(i->second);
		}
	}

	return NULL;
}

}; // </gui>

}; // </wt>



#endif
