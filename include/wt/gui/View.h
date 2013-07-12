#ifndef WT_VIEW_H
#define WT_VIEW_H


#include "wt/stdafx.h"

#include "wt/Gui/Canvas.h"
#include "wt/HashedString.h"
#include "wt/EventManager.h"
#include "wt/AGameInput.h"

namespace wt{

namespace Gui{

using namespace Gl;

class UIWindow;

#define DIRTY setDirty(true);

class ViewClickedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	ViewClickedEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </ViewClickedEvent>


class View{
friend class UIWindow;
public:

	struct GridLocation{
		Uint32 row;
		Uint32 column;
		Uint32 rowSpan;
		Uint32 columnSpan;

		GridLocation() : row(0), column(0), rowSpan(0), columnSpan(0){
		}

	}; // </GridLocation>

	struct ViewBackground{
		Color color;
		Texture2D* texture;

		ViewBackground() : color(Color::white()), texture(NULL){
		}

	}; // </ViewBackground>

	enum ScalingMode{
		eFIXED,
		eGRID
	};

private:
	glm::vec2 mPosition;
	glm::vec2 mSize;
	Uint32 mId;
	ViewBackground mBackground;
	bool mIsVisible;
	String mName;
	EventManager* mEventManager;
	Texture2D mTexture;
	Font* mFont;
	bool mDirty;
	GridLocation mGridLocation;

	ScalingMode mScalingMode;
	bool mNeedsScale;

protected:

	void setId(Uint32 id){
		mId = id;
	}

	void setName(const String& name){
		mName = name;
	}

	void setEventManager(EventManager* manager){
		mEventManager = manager;
		onHook(manager);
	}

	void setDirty(bool dirty){
		mDirty = dirty;
	}

	bool isDirty() const{
		return mDirty;
	}

	bool needsRescaling(){
		return mNeedsScale;
	}

	void setNeedsScaling(bool state){
		mNeedsScale = state;
	}

	Texture2D& getTexture(){
		return mTexture;
	}

public:
	View() : mSize(0,0), mPosition(0,0), mId(0),
		mIsVisible(true), mDirty(true), mFont(NULL), mScalingMode(eFIXED), mNeedsScale(false){

		mTexture.create();
	}

	const GridLocation& getGridLoation() const{
		return mGridLocation;
	}

	void setGridLocation(Uint32 row, Uint32 column, Uint32 rowSpan=1, Uint32 columnSpan=1){
		mGridLocation.row = row;
		mGridLocation.column = column;
		mGridLocation.rowSpan = rowSpan;
		mGridLocation.columnSpan = columnSpan;

		DIRTY;
		mNeedsScale = true;
	}

	void setScalingMode(ScalingMode mode){
		if(mScalingMode != mode){
			if(mode == eGRID){
				DIRTY;
				mNeedsScale = true;
			}

			mScalingMode = mode;
		}
	}

	ScalingMode getScalingMode() const{
		return mScalingMode;
	}

	virtual void setFont(Font* font){ DIRTY
		mFont = font;
	}

	Font* getFont() const{
		return mFont;
	}

	virtual void onHook(EventManager* manager){
	}

	virtual	void onClicked(){
	}

	EventManager* getEventManager() const{
		return mEventManager;
	}

	virtual ~View(){
	}

	bool isVisible() const{
		return mIsVisible;
	}

	void setVisible(bool state){
		mIsVisible = state;
	}

	glm::vec2& getPosition(){
		return mPosition;
	}

	glm::vec2 toLocalCoords(float x, float y) const{
		return glm::vec2(x-mPosition.x, y-mPosition.y);
	}

	glm::vec2 toGlobalCoords(float x, float y) const{
		return glm::vec2(mPosition.x+x, mPosition.y+y);
	}

	virtual void onMouseDrag(const MouseMotionEvent* evt){
	}

	virtual void onMouseDown(const MousePressEvent* evt){
	}

	virtual void onMouseUp(const MousePressEvent* evt){
	}

	virtual void onMouseMotion(const MouseMotionEvent* evt){
		WT_UNUSED(evt);
	}

	virtual void onMouseEnter(const MouseMotionEvent* evt){
	}

	virtual void onMouseLeave(const MouseMotionEvent* evt){
	}

	virtual void onKeyDown(VirtualKey key){
		WT_UNUSED(key);
	}

	Uint32 getId() const{
		return mId;
	}

	const String& getName() const{
		return mName;
	}

	void setPosition(float x, float y){
		mPosition.x = x;
		mPosition.y = y;
	}

	void emitEvent(Event* e){
		e->setEmitterData( mId );
		mEventManager->queueEvent(e);
	}

	void setPosition(const glm::vec2& position){
		mPosition = position;
	}

	glm::vec2& getSize(){
		return mSize;
	}

	void setSize(const glm::vec2& size){
		setSize(size.x, size.y);
	}

	float getWidth() const{
		return mSize.x;
	}

	float getHeight() const{
		return mSize.y;
	}

	bool contains(float x, float y) const{
		return x >= mPosition.x && x < mPosition.x+mSize.x &&
			y >= mPosition.y && y < mPosition.y+mSize.y;
	}

	void setBackgroundColor(const Color& clr){
		DIRTY;

		mBackground.color = clr;
	}

	void setBackgroundTexture(Texture2D* texture){
		DIRTY;

		mBackground.texture = texture;
	}

	void setSize(float w, float h){
		DIRTY;

		mSize.x = w;
		mSize.y = h;

		// resize texture
		mTexture.bind();
		mTexture.setData(w, h, GL_RGBA, GL_RGBA8, NULL, GL_FLOAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	virtual void draw(Canvas& c){
		if(mBackground.texture){
			c.drawTexture(mBackground.texture, 0, 0, mSize.x, mSize.y, mBackground.color);
		}
		else{
			c.drawRect(0, 0, mSize.x, mSize.y, mBackground.color);
		}
	}

}; // </View>

}; // </Gui>

}; // </wt>

#endif
