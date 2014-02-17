#ifndef WT_AGAMEWINDOW_H
#define WT_AGAMEWINDOW_H


#include "wt/stdafx.h"

#include "wt/EventManager.h"
#include "wt/ASerializable.h"

namespace wt{

class WindowSizeChange : public Event{
public:
	static const EvtType TYPE;

	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	int oldWidth, oldHeight;
	int newWidth, newHeight;

	WindowSizeChange(int ow, int oh, int nw, int nh) : oldWidth(ow), oldHeight(oh),
		newWidth(nw), newHeight(nh){
	}

	const EvtType& getType() const { return TYPE; }
}; // </WindowSizeChange>

class AGameWindow{
public:
	enum Mode{
		eMODE_INVALID,

		eMODE_FULLSCREEN,
		eMODE_WINDOWED,
		eMODE_WINDOWED_BORDERLESS,

	}; // </Mode>

	struct Desc : public lua::ASerializable{
		// Screen geometry
		uint32_t screenWidth;
		uint32_t screenHeight;

		// Screen position
		uint32_t screenPosX;
		uint32_t screenPosY;
	
		// Pixel depth
		uint8_t pixelDepth;

		// Screen title
		String windowTitle;

		bool vsyncEnabled;

		Mode mode;
		
		void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

		void deserialize(lua::State* luaState, const LuaPlus::LuaObject& src);

		Desc(uint32_t screenWidth=640, uint32_t screenHeight=480, const String& windowTitle="",
			bool vsyncEnabled=true, uint8_t pixelDepth=32, Mode mode = eMODE_WINDOWED);

		virtual ~Desc(){
		}

	}; // <Desc>

	virtual ~AGameWindow();

	uint32_t getWidth() const;

	uint32_t getHeight() const;

	float getAspectRatio() const;

	virtual void hook(EventManager* evtManager) = 0;

	virtual void create(const Desc& mode) = 0;

	virtual const Desc& getDesc() const = 0;

	virtual void swapBuffers() = 0;

	virtual void setScreenSize(uint32_t width, uint32_t height) = 0;

	virtual void setWindowMode(Mode mode) = 0;

	virtual void setVsyncEnabled(bool state) = 0;

	virtual void setWindowTitle(const String& title) = 0;

}; // </GameWindow>

}; // </wt>

#endif
