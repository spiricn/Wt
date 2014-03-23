#ifndef WT_VIEW_H
#define WT_VIEW_H


#include "wt/stdafx.h"

#include "wt/gui/Canvas.h"
#include "wt/HashedString.h"
#include "wt/EventManager.h"
#include "wt/AGameInput.h"

namespace wt
{

namespace gui
{

using namespace gl;

#define DIRTY do{ setDirty(true); }while(0)

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
friend class Window;
public:
	struct GridLocation{
		uint32_t row;
		uint32_t column;
		uint32_t rowSpan;
		uint32_t columnSpan;

		GridLocation() : row(0), column(0), rowSpan(0), columnSpan(0){
		}

	}; // </GridLocation>

	struct ViewBackground{
		Color color;
		Texture2D* texture;

		ViewBackground() : color(Color::White()), texture(NULL){
		}

	}; // </ViewBackground>

	enum ScalingMode{
		eSCALE_MODE_FIXED,
		eSCALE_MODE_GRID
	}; // </ScalingMode>

	View(Window* parent);

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

	glm::vec2& getPosition();

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

	void setPosition(float x, float y);

	void emitEvent(Event* e);

	void setPosition(const glm::vec2& position);

	glm::vec2& getSize();

	void setSize(const glm::vec2& size);

	float getWidth() const;

	float getHeight() const;

	bool contains(float x, float y) const;

	void setBackgroundColor(const Color& clr);

	void setBackgroundTexture(Texture2D* texture);

	void setSize(float w, float h);

	virtual void draw(Canvas& c);

	Window* getParent() const;

protected:
	void setId(uint32_t id);

	void setName(const String& name);

	void setEventManager(EventManager* manager);

	void setDirty(bool dirty);

	bool isDirty() const;

	bool needsRescaling();

	void setNeedsScaling(bool state);

	Texture2D& getTexture();

private:
	glm::vec2 mPosition;
	glm::vec2 mSize;
	uint32_t mId;
	ViewBackground mBackground;
	bool mIsVisible;
	String mName;
	EventManager* mEventManager;
	Texture2D mTexture;
	Font* mFont;
	bool mDirty;
	GridLocation mGridLocation;
	Window* mParent;
	ScalingMode mScalingMode;
	bool mNeedsScale;


}; // </View>

}; // </gui>

}; // </wt>

#endif
