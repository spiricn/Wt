#ifndef WT_VIEW_H
#define WT_VIEW_H


#include "wt/stdafx.h"

#include "wt/gui/ICanvas.h"
#include "wt/HashedString.h"
#include "wt/EventManager.h"
#include "wt/AGameInput.h"
#include "wt/IEventEmitter.h"
#include "wt/gui/Rect.h"
#include "wt/Color.h"
#include "wt/gui/Paint.h"
#include "wt/Font.h"

namespace wt
{

namespace gui
{

class Layout;

class View : public IEventEmitter{
public:
	struct GridLocation{
		uint32_t row;
		uint32_t column;
		uint32_t rowSpan;
		uint32_t columnSpan;

		GridLocation() : row(0), column(0), rowSpan(0), columnSpan(0){
		}

	}; // </GridLocation>

	enum ScalingMode{
		eSCALE_MODE_FIXED,
		eSCALE_MODE_GRID
	}; // </ScalingMode>

public:
	View(View* parent, EventManager* eventManager, AGameInput* input);

	const GridLocation& getGridLoation() const;

	void setGridLocation(uint32_t row, uint32_t column, uint32_t rowSpan=1, uint32_t columnSpan=1);

	void setScalingMode(ScalingMode mode);

	ScalingMode getScalingMode() const;

	virtual void setFont(Font* font);

	Font* getFont() const;

	virtual void onHook(EventManager* manager);

	virtual	void onClicked();

	EventManager* getEventManager() const;

	virtual ~View();

	bool isVisible() const;

	void setVisible(bool state);

	const Rect& getRect() const;

	glm::vec2 getPosition() const;

	glm::vec2 toLocalCoords(float x, float y) const;

	glm::vec2 toGlobalCoords(float x, float y) const;

	virtual void onMouseDrag(const MouseMotionEvent* evt);

	virtual void onMouseDown(const MousePressEvent* evt);

	virtual void onMouseUp(const MousePressEvent* evt);

	virtual void onMouseMotion(const MouseMotionEvent* evt);

	virtual void onMouseEnter(const MouseMotionEvent* evt);

	virtual void onMouseLeave(const MouseMotionEvent* evt);

	virtual void onKeyDown(VirtualKey key);

	uint32_t getId() const;

	const String& getName() const;

	void emitEvent(AEvent* e);

	virtual void setPosition(const glm::vec2& position);

	glm::vec2 getSize() const;

	virtual void setSize(const glm::vec2& size);

	float getWidth() const;

	float getHeight() const;

	bool contains(float x, float y) const;

	virtual void draw(ICanvas& c) = 0;

	View* getParent() const;

	void setBackgroundColor(const Color& clr);

	const Color& getBackgroundColor() const;

	void redraw();

	AGameInput* getInput() const;

	ICanvas& getCanvas();

protected:
	void debugDraw();

	void setId(uint32_t id);

	void setName(const String& name);

	void dirty();

	void clean();

	bool isDirty() const;

	bool needsRescaling();

	void setNeedsScaling(bool state);

	Texture2D& getTexture();

private:
	Rect mRect;
	uint32_t mId;
	bool mIsVisible;
	String mName;
	EventManager* mEventManager;
	ICanvas* mCanvas;
	Font* mFont;
	bool mDirty;
	GridLocation mGridLocation;
	View* mParent;
	ScalingMode mScalingMode;
	bool mNeedsScale;
	Color mBackgroundColor;
	AGameInput* mInput;

private:
	friend class Layout;
}; // </View>

class ViewClickedEvent : public AEvent{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EventType TYPE;

	ViewClickedEvent(){
	}

	const EventType& getType() const{
		return TYPE;
	}
}; // </ViewClickedEvent>


}; // </gui>

}; // </wt>

#endif
