#include "wt/stdafx.h"
#include "wt/gui/Layout.h"

#include "wt/gui/ListView.h"
#include "wt/GLTrace.h"

#define TD_TRACE_TAG "Layout"

namespace wt
{

using namespace gl;

namespace gui
{

Layout::Layout(Layout* parent, EventManager* eventManager, AGameInput* input) : View(parent, eventManager, input), mFocus(NULL), mHoverView(NULL), mClickView(NULL), mDefaultFont(NULL), mDragView(NULL), mNumGridRows(5), mNumGridColumns(5), mDefaultScaleMode(View::eSCALE_MODE_FIXED), mNeedsRescale(false), mVerticalCellSpacing(5.0f), mHorizontalCellSpacing(5.0f), mVisible(true){
}

void Layout::setVisible(bool visible){
	mVisible = visible;
}

bool Layout::isVisible() const{
	return mVisible;
}

void Layout::setVerticalCellSpacing(float spacing){
	WT_ASSERT(spacing >= 0.0f, "Cell spacing can't be negative");

	mVerticalCellSpacing = spacing;

	mNeedsRescale = true;
}

void Layout::setHorizontalCellSpacing(float spacing){
	WT_ASSERT(spacing >= 0.0f, "Cell spacing can't be negative");

	mHorizontalCellSpacing = spacing;

	mNeedsRescale = true;
}

void Layout::setDefaultFont(Font* font){
	mDefaultFont = font;
}

void Layout::setGridSize(uint32_t numRows, uint32_t numColumns){
	mNumGridRows = numRows;
	mNumGridColumns = numColumns;

	mNeedsRescale = true;
}

void Layout::removeView(View* view){
	if(mFocus == view){
		mFocus = NULL;
	}

	if(mHoverView == view){
		mHoverView = NULL;
	}

	if(mClickView == view){
		mClickView = NULL;
	}

	if(mDragView == view){
		mDragView = NULL;
	}

	mViews.erase( view->getId() );
	delete view;
}

void Layout::setDefaultScaleMode(View::ScalingMode mode){
	mDefaultScaleMode = mode;
}
View* Layout::findView(const String& name){

	for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
		if(i->second->getName().compare(name)==0){
			return i->second;
		}
	}

	return NULL;
}

bool Layout::handleEvent(const EventPtr evt){
	if(!mVisible){
		return true;
	}

	if(evt->getType() == MousePressEvent::TYPE){
		const MousePressEvent* e = static_cast<const MousePressEvent*>(evt.get());

		glm::vec2 localPos = toLocalCoords(e->mX, e->mY);
		View* clicked = viewAt(localPos.x, localPos.y);

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

		glm::vec2 localPos = toLocalCoords(e->mX, e->mY);
		View* view = viewAt(localPos.x, localPos.y);

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
		// TODO move to window manager
		TRACEW("Resize");
		/*const WindowSizeChange* e = static_cast<const WindowSizeChange*>(evt.get());
		mCanvas.setSize(e->newWidth, e->newHeight);
		mNeedsRescale = true;*/
	}
	return false;
}


View* Layout::viewAt(float x, float y){
		
	for(ViewMap::reverse_iterator i=mViews.rbegin(); i!=mViews.rend(); i++){
		if(i->second->isVisible() && i->second->contains(x, y)){
			return i->second;
		}
	}

	return NULL;
}

void Layout::draw(ICanvas& canvas){
	gl( ActiveTexture(GL_TEXTURE0) );

	glm::vec2 cellSize = getCanvas().getSize() / glm::vec2(mNumGridColumns, mNumGridRows);

	for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
		View* view = i->second;

		if(!view->isVisible()){
			continue;
		}

		// Handle view grid scaling
		if(view->getScalingMode() == View::eSCALE_MODE_GRID && (view->needsRescaling() || mNeedsRescale)){
			glm::vec2 size = cellSize * glm::vec2(view->getGridLoation().columnSpan, view->getGridLoation().rowSpan) ;
			glm::vec2 pos = cellSize * glm::vec2(view->getGridLoation().column, view->getGridLoation().row) ;

			size -= glm::vec2(mHorizontalCellSpacing/2.0f, mVerticalCellSpacing/2.0f);
			pos += glm::vec2(mHorizontalCellSpacing/2.0f, mVerticalCellSpacing/2.0f);
				
			view->setSize( size );
			view->setPosition( pos );
			view->setNeedsScaling(false);
		}

		// Redraw the view if necessary
		if(i->second->isDirty()){
			i->second->redraw();
		}

		// Render the view onto our canvas
		canvas.bind();

		canvas.drawTexture(&i->second->getTexture(), 
			i->second->getPosition().x, i->second->getPosition().y,
			i->second->getSize().x, i->second->getSize().y, i->second->getBackgroundColor()
		);
	}

	mNeedsRescale = false;
}

Layout::~Layout(){
	for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
		delete i->second;
	}

	mViews.clear();
}

void Layout::addView(View* view){
	uint32_t id = mViews.size();
	while(true){
		if(mViews.find(id) == mViews.end()){
			break;
		}
		id++;
	}
	
	view->setScalingMode(mDefaultScaleMode);
	view->setId(id);

	if(view->getFont() == NULL){
		view->setFont(mDefaultFont);
	}

	mViews.insert(std::make_pair(id, view));
}

} // </gui>

} // </wt>
