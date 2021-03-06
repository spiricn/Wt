#include "wt/stdafx.h"

#include "wt/gui/View.h"
#include "wt/Font.h"
#include "wt/gui/Canvas.h"

#define TD_TRACE_TAG "View"

namespace wt
{

namespace gui
{

View::View(View* parent, EventManager* eventManager, AGameInput* input) : mRect(0, 0, 1, 1), mId(0),
	mIsVisible(true), mDirty(true), mFont(NULL), mScalingMode(eSCALE_MODE_FIXED),
	mNeedsScale(false), mParent(parent), mBackgroundColor(Color::White()), mEventManager(eventManager), mInput(input), mCanvas(new Canvas){
}

void View::setId(uint32_t id){
	mId = id;
}

void View::setName(const String& name){
	mName = name;
}

void View::dirty(){
	mDirty = true;

	if(mParent){
		mParent->dirty();
	}
}

void View::clean(){
	mDirty = false;
}

bool View::isDirty() const{
	return mDirty;
}

bool View::needsRescaling(){
	return mNeedsScale;
}

void View::setNeedsScaling(bool state){
	mNeedsScale = state;
}

Texture2D& View::getTexture(){
	return *mCanvas->getTexture();
}

const View::GridLocation& View::getGridLoation() const{
	return mGridLocation;
}

void View::setGridLocation(uint32_t row, uint32_t column, uint32_t rowSpan, uint32_t columnSpan){
	mGridLocation.row = row;
	mGridLocation.column = column;
	mGridLocation.rowSpan = rowSpan;
	mGridLocation.columnSpan = columnSpan;

	dirty();

	mNeedsScale = true;
}

void View::setScalingMode(ScalingMode mode){
	if(mScalingMode != mode){
		if(mode == eSCALE_MODE_GRID){
			dirty();

			mNeedsScale = true;
		}

		mScalingMode = mode;
	}
}

View::ScalingMode View::getScalingMode() const{
	return mScalingMode;
}

void View::setFont(Font* font){
	dirty();

	mFont = font;
}

Font* View::getFont() const{
	return mFont;
}

void View::onHook(EventManager* manager){
}

void View::onClicked(){
}

EventManager* View::getEventManager() const{
	return mEventManager;
}

View::~View(){
}

bool View::isVisible() const{
	return mIsVisible;
}

void View::setVisible(bool state){
	mIsVisible = state;
}

glm::vec2 View::getPosition() const{
	return mRect.getPosition();
}

glm::vec2 View::toLocalCoords(float x, float y) const{
	glm::vec2 local(x-mRect.x, y-mRect.y);

	if(mParent){
		return mParent->toLocalCoords(local.x, local.y);
	}
	else{
		return local;
	}
}

glm::vec2 View::toGlobalCoords(float x, float y) const{
	glm::vec2 global = glm::vec2(mRect.x + x, mRect.y + y);

	if(mParent){
		return mParent->toGlobalCoords(global.x, global.y);
	}
	else{
		return global;
	}
}

void View::onMouseDrag(const MouseMotionEvent* evt){
}

void View::onMouseDown(const MousePressEvent* evt){
}

void View::onMouseUp(const MousePressEvent* evt){
}

void View::onMouseMotion(const MouseMotionEvent* evt){
}

void View::onMouseEnter(const MouseMotionEvent* evt){
}

void View::onMouseLeave(const MouseMotionEvent* evt){
}

void View::onKeyDown(VirtualKey key){
	WT_UNUSED(key);
}

uint32_t View::getId() const{
	return mId;
}

const String& View::getName() const{
	return mName;
}

void View::setPosition(const glm::vec2& position){
	mRect.x = position.x;
	mRect.y = position.y;

	mNeedsScale = true;
}

void View::emitEvent(AEvent* e){
	mEventManager->emitEvent(e, this);
}

glm::vec2 View::getSize() const{
	return mRect.getSize();
}

float View::getWidth() const{
	return mRect.width;
}

float View::getHeight() const{
	return mRect.height;
}

bool View::contains(float x, float y) const{
	return mRect.contains(glm::vec2(x, y));
}

void View::setBackgroundColor(const Color& clr){
	dirty();

	mBackgroundColor = clr;
}

void View::setSize(const glm::vec2& size){
	dirty();

	mRect.width = size.x;
	mRect.height = size.y;

	mCanvas->setSize(mRect.width, mRect.height);
}

void View::redraw(){
	if(!mDirty){
		return;
	}

	mCanvas->bind();

	mCanvas->clear();

	draw(*mCanvas);
	
	clean();
}

View* View::getParent() const{
	return mParent;
}

const Rect& View::getRect() const{
	return mRect;
}

const Color& View::getBackgroundColor() const{
	return mBackgroundColor;
}

ICanvas& View::getCanvas(){
	return *mCanvas;
}

AGameInput* View::getInput() const{
	return mInput;
}

void View::debugDraw(){
	mCanvas->setClearColor(Color::Cyan());
	mCanvas->clear();

	Paint p;

	float w = getWidth()/2;
	float h = getHeight()/2;

	p.setFillColor(Color::Red());
	mCanvas->drawRect(0, 0, w, h, &p);

	p.setFillColor(Color::Green());
	mCanvas->drawRect(w, 0, w, h,  &p);

	p.setFillColor(Color::Blue());
	mCanvas->drawRect(0, h, w, h, &p);

	p.setFillColor(Color::Yellow());
	mCanvas->drawRect(w, h, w, h, &p);
}

const EventType ViewClickedEvent::TYPE = "ViewClicked";

} // </gui>

} // </wt>
