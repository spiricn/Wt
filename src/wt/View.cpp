#include "wt/stdafx.h"

#include "wt/gui/View.h"
#include "wt/Font.h"

#define TD_TRACE_TAG "View"

namespace wt
{

namespace gui
{

const EvtType ViewClickedEvent::TYPE = "ViewClicked";


void View::setId(uint32_t id){
	mId = id;
}

void View::setName(const String& name){
	mName = name;
}

void View::setEventManager(EventManager* manager){
	mEventManager = manager;
	onHook(manager);
}

void View::setDirty(bool dirty){
	mDirty = dirty;
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
	return mTexture;
}

View::View(Window* parent) : mSize(0,0), mPosition(0,0), mId(0),
	mIsVisible(true), mDirty(true), mFont(NULL), mScalingMode(eSCALE_MODE_FIXED),
	mNeedsScale(false), mParent(parent){

	mTexture.create();
}

const View::GridLocation& View::getGridLoation() const{
	return mGridLocation;
}

void View::setGridLocation(uint32_t row, uint32_t column, uint32_t rowSpan, uint32_t columnSpan){
	mGridLocation.row = row;
	mGridLocation.column = column;
	mGridLocation.rowSpan = rowSpan;
	mGridLocation.columnSpan = columnSpan;

	DIRTY;
	mNeedsScale = true;
}

void View::setScalingMode(ScalingMode mode){
	if(mScalingMode != mode){
		if(mode == eSCALE_MODE_GRID){
			DIRTY;
			mNeedsScale = true;
		}

		mScalingMode = mode;
	}
}

View::ScalingMode View::getScalingMode() const{
	return mScalingMode;
}

void View::setFont(Font* font){
	DIRTY;

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

glm::vec2& View::getPosition(){
	return mPosition;
}

glm::vec2 View::toLocalCoords(float x, float y) const{
	return glm::vec2(x-mPosition.x, y-mPosition.y);
}

glm::vec2 View::toGlobalCoords(float x, float y) const{
	return glm::vec2(mPosition.x+x, mPosition.y+y);
}

void View::onMouseDrag(const MouseMotionEvent* evt){
}

void View::onMouseDown(const MousePressEvent* evt){
}

void View::onMouseUp(const MousePressEvent* evt){
}

void View::onMouseMotion(const MouseMotionEvent* evt){
	WT_UNUSED(evt);
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

void View::setPosition(float x, float y){
	mPosition.x = x;
	mPosition.y = y;
}

void View::emitEvent(Event* e){
	e->setEmitterData( mId );
	mEventManager->queueEvent(e);
}

void View::setPosition(const glm::vec2& position){
	mPosition = position;
}

glm::vec2& View::getSize(){
	return mSize;
}

void View::setSize(const glm::vec2& size){
	setSize(size.x, size.y);
}

float View::getWidth() const{
	return mSize.x;
}

float View::getHeight() const{
	return mSize.y;
}

bool View::contains(float x, float y) const{
	return x >= mPosition.x && x < mPosition.x+mSize.x &&
		y >= mPosition.y && y < mPosition.y+mSize.y;
}

void View::setBackgroundColor(const Color& clr){
	DIRTY;

	mBackground.color = clr;
}

void View::setBackgroundTexture(Texture2D* texture){
	DIRTY;

	mBackground.texture = texture;
}

void View::setSize(float w, float h){
	DIRTY;

	mSize.x = w;
	mSize.y = h;

	// resize texture
	mTexture.bind();
	mTexture.setData(w, h, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void View::draw(Canvas& c){
	if(mBackground.texture){
		c.drawTexture(mBackground.texture, 0, 0, mSize.x, mSize.y, mBackground.color);
	}
	else{
		c.drawRect(0, 0, mSize.x, mSize.y, mBackground.color);
	}
}

Window* View::getParent() const{
	return mParent;
}

} // </gui>

} // </wt>
