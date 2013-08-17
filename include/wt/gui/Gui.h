#ifndef WT_GUI_H
#define WT_GUI_H


#include "wt/stdafx.h"
#include "wt/FrameBuffer.h"

#include "wt/Gui/View.h"
#include "wt/Gui/ProgressView.h"
#include "wt/Gui/Button.h"
#include "wt/Gui/RectView.h"
#include "wt/Gui/SliderView.h"
#include "wt/Gui/CircleView.h"
#include "wt/Gui/Checkbox.h"

#include "wt/EventManager.h"
#include "wt/EventEmitter.h"

namespace wt{

namespace Gui{

using namespace gl;

class UIWindow : public EventListener{
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


public:
	UIWindow() : mFocus(NULL), mHoverView(NULL), mClickView(NULL), 
		mDefaultFont(NULL), mDragView(NULL), mNumGridRows(5), mNumGridColumns(5),
		mDefaultScaleMode(View::eFIXED), mNeedsRescale(false), mVerticalCellSpacing(5.0f), mHorizontalCellSpacing(5.0f){
	}

	void setVerticalCellSpacing(float spacing){
		WT_ASSERT(spacing >= 0.0f, "Cell spacing can't be negative");

		mVerticalCellSpacing = spacing;

		mNeedsRescale = true;
	}

	void setHorizontalCellSpacing(float spacing){
		WT_ASSERT(spacing >= 0.0f, "Cell spacing can't be negative");

		mHorizontalCellSpacing = spacing;

		mNeedsRescale = true;
	}

	void setDefaultFont(Font* font){
		mDefaultFont = font;
	}

	void setGridSize(uint32_t numRows, uint32_t numColumns){
		mNumGridRows = numRows;
		mNumGridColumns = numColumns;

		mNeedsRescale = true;
	}

	void setInput(AGameInput* input){
		mInput = input;
	}

	Canvas& getCanvas(){
		return mCanvas;
	}

	void hook(EventManager* evtManager){
		mEventManager = evtManager;
		mEventManager->registerListener(this, MousePressEvent::TYPE);
		mEventManager->registerListener(this, WindowSizeChange::TYPE);
		mEventManager->registerListener(this, KeyPressEvent::TYPE);
		mEventManager->registerListener(this, MouseMotionEvent::TYPE);

		mEventEmitter.hook(mEventManager,
			4,
			RectViewClickedEvent::TYPE,
			ButtonClickedEvent::TYPE,
			SliderValueChangedEvent::TYPE,
			CheckboxClickedEvent::TYPE
		);
	}

	void removeView(View* view){
		mViews.erase( view->getId() );
		delete view;
	}

	void setDefaultScaleMode(View::ScalingMode mode){
		mDefaultScaleMode = mode;
	}

	template<class T>
	T* createView(const String& name=""){
		uint32_t id = mViews.size();
		while(true){
			if(mViews.find(id) == mViews.end()){
				break;
			}
			id++;
		}

		T* tView = new T;
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


	View* findView(const String& name){

		for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
			if(i->second->getName().compare(name)==0){
				return i->second;
			}
		}

		return NULL;
	}

	template<class T>
	T* findView(const String& name){

		for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
			if(i->second->getName().compare(name)==0){
				return static_cast<T*>(i->second);
			}
		}

		return NULL;
	}



	bool handleEvent(const Sp<Event> evt){
		if(evt->getType() == MousePressEvent::TYPE){
			const MousePressEvent* e = static_cast<const MousePressEvent*>(evt.get());

			View* clicked = viewAt(e->mX, e->mY);

			if(clicked){
				if(e->mAction == MousePressEvent::eBUTTON_DOWN){
					if(e->mButton == BTN_LEFT){
						mClickView = clicked;
					}

					clicked->onMouseDown(e);

					mDragView = clicked;
				}
				else{
					clicked->onMouseUp(e);

					if(mClickView && clicked == mClickView && e->mButton == BTN_LEFT){
						mClickView->onClicked();
						mClickView = NULL;
					}

					mDragView = NULL;
				}

				mFocus = clicked;

				return true;
			}
		}
		else if(evt->getType() == MouseMotionEvent::TYPE){
			const MouseMotionEvent* e = static_cast<const MouseMotionEvent*>(evt.get());

			View* view = viewAt(e->mX, e->mY);

			if(view){
				if(mInput->isMouseButtonDown(BTN_LEFT) && mDragView == view){
					mDragView->onMouseDrag(e);
				}

				if(mHoverView == NULL){
					mHoverView = view;
					mHoverView->onMouseEnter(e);
				}
				else if(mHoverView != view){
					mHoverView->onMouseLeave(e);
					mHoverView = view;
					mHoverView->onMouseEnter(e);
				}
			}
			else{
				if(mHoverView){
					mHoverView->onMouseLeave(e);
					mHoverView = NULL;
				}
			}

		}
		else if(evt->getType() == KeyPressEvent::TYPE){
			const KeyPressEvent* e = static_cast<const KeyPressEvent*>(evt.get());
			if(mFocus){
				if(e->mAction == KeyPressEvent::DOWN){
					mFocus->onKeyDown(e->mCode);
					return true;
				}
			}
		}

		else{
			const WindowSizeChange* e = static_cast<const WindowSizeChange*>(evt.get());
			mCanvas.resize(e->newWidth, e->newHeight);
			mNeedsRescale = true;
		}
		return false;
	}


	View* viewAt(float x, float y){
		
		for(ViewMap::reverse_iterator i=mViews.rbegin(); i!=mViews.rend(); i++){
			if(i->second->isVisible() && i->second->contains(x, y)){
				return i->second;
			}
		}

		return NULL;
	}

	void create(uint32_t w, uint32_t h){
		mCanvas.create(w, h);
	}

	void draw(){
		mCanvas.getFBO().bind(FrameBuffer::eMODE_DRAW);

		mCanvas.clear();

		static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, bfrMap);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( glm::value_ptr(mCanvas.getProjMat()) );

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glm::vec2 cellSize = mCanvas.getSize() / glm::vec2(mNumGridColumns, mNumGridRows);

#if 0
		// Cell drawing for debugging purposes
		for(uint32_t i=0; i<mNumGridColumns; i++){
			mCanvas.drawLine( i*cellSize.x, 0, i*cellSize.x, mCanvas.getSize().y, Color::green(), 1);
		}

		for(uint32_t i=0; i<mNumGridRows; i++){
			mCanvas.drawLine(0, i*cellSize.y, mCanvas.getSize().x, i*cellSize.y, Color::green(), 1);
		}
#endif

		for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
			View* view = i->second;

			if(!view->isVisible()){
				continue;
			}

			if(view->getScalingMode() == View::eGRID && (view->needsRescaling() || mNeedsRescale)){
				glm::vec2 size = cellSize * glm::vec2(view->getGridLoation().columnSpan, view->getGridLoation().rowSpan) ;
				glm::vec2 pos = cellSize * glm::vec2(view->getGridLoation().column, view->getGridLoation().row) ;

				size -= glm::vec2(mHorizontalCellSpacing/2.0f, mVerticalCellSpacing/2.0f);
				pos += glm::vec2(mHorizontalCellSpacing/2.0f, mVerticalCellSpacing/2.0f);
				
				view->setSize( size );
				view->setPosition( pos );
				view->setNeedsScaling(false);
			}

			if(i->second->isDirty()){
				mCanvas.setTranslation( glm::vec2(0, 0) );
				glLoadMatrixf( glm::value_ptr(mCanvas.getModelViewMat()) );


				mCanvas.setOutput(&i->second->getTexture());
				mCanvas.clear();

				i->second->draw(mCanvas);

				mCanvas.setOutput(NULL);
				i->second->setDirty(false);
			}

			mCanvas.setTranslation( i->second->getPosition() );
			glLoadMatrixf( glm::value_ptr(mCanvas.getModelViewMat()) );

			mCanvas.drawTexture(&i->second->getTexture(), 
				0, 0,
				i->second->getSize().x, i->second->getSize().y, Color::White());
		}

		mNeedsRescale = false;
#if 0
		// highlight focus
		if(mFocus){
			glLoadIdentity();
			mCanvas.drawRect(
				mFocus->getPosition().x, mFocus->getPosition().y,
				mFocus->getWidth(), mFocus->getHeight(), Color::blue(), Canvas::eRECT_LINE);
		}
#endif

		mCanvas.getFBO().unbind(FrameBuffer::eMODE_DRAW);
	}

	~UIWindow(){
		for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
			delete i->second;
		}

		mViews.clear();

		mEventManager->unregisterListener(this);
	}
};


}; // </Gui>

}; // </wt>



#endif
