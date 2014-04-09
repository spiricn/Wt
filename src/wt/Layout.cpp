#include "wt/stdafx.h"
#include "wt/gui/Layout.h"

#include "wt/gui/ListView.h"

#define TD_TRACE_TAG "Layout"

namespace wt
{

using namespace gl;

namespace gui
{

Layout::Layout(EventManager* eventManager) : mFocus(NULL), mHoverView(NULL), mClickView(NULL), 
	mDefaultFont(NULL), mDragView(NULL), mNumGridRows(5), mNumGridColumns(5),
	mDefaultScaleMode(View::eSCALE_MODE_FIXED), mNeedsRescale(false), mVerticalCellSpacing(5.0f),
	mHorizontalCellSpacing(5.0f), mVisible(true), mEventManager(eventManager){

	mCanvas.create(640, 480);
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

void Layout::setInput(AGameInput* input){
	mInput = input;
}

Texture2D* Layout::getTexture() const{
	// TODO
	return const_cast<Layout*>(this)->mCanvas.getTexture();
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


View* Layout::viewAt(float x, float y){
		
	for(ViewMap::reverse_iterator i=mViews.rbegin(); i!=mViews.rend(); i++){
		if(i->second->isVisible() && i->second->contains(x, y)){
			return i->second;
		}
	}

	return NULL;
}

void Layout::draw(){
	mCanvas.getFBO().bind(FrameBuffer::eMODE_DRAW);

	mCanvas.getFBO().unbind(FrameBuffer::eMODE_READ);

	mCanvas.clear();

	static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, bfrMap);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( glm::value_ptr(mCanvas.getProjMat()) );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glm::vec2 cellSize = mCanvas.getSize() / glm::vec2(mNumGridColumns, mNumGridRows);

	gl( ActiveTexture(GL_TEXTURE0) );

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
			mCanvas.setTranslation( glm::vec2(0, 0) );
			glLoadMatrixf( glm::value_ptr(mCanvas.getModelViewMat()) );

			mCanvas.setOutput(&i->second->getTexture());
			mCanvas.clear();

			i->second->draw(mCanvas);

			mCanvas.setOutput(NULL);
			i->second->clean();
		}

		// Render the view
		mCanvas.setTranslation( i->second->getPosition() );
		glLoadMatrixf( glm::value_ptr(mCanvas.getModelViewMat()) );

		mCanvas.drawTexture(&i->second->getTexture(), 
			0, 0,
			i->second->getSize().x, i->second->getSize().y, i->second->getBackgroundColor()
		);
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

	mCanvas.getFBO().unbind();
}

Layout::~Layout(){
	for(ViewMap::iterator i=mViews.begin(); i!=mViews.end(); i++){
		delete i->second;
	}

	mViews.clear();
}

const Rect& Layout::getRect() const{
	return mRect;
}

void Layout::setPosition(const glm::vec2& pos){
	mRect.x = pos.x;
	mRect.y = pos.y;
}

void Layout::setSize(const glm::vec2& size){
	mRect.width = size.x;
	mRect.height = size.y;

	mCanvas.resize(size.x, size.y);
}

} // </gui>

} // </wt>
