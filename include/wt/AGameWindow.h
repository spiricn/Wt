#ifndef WT_AGAMEWINDOW_H
#define WT_AGAMEWINDOW_H


#include "wt/stdafx.h"


#include "wt/EventManager.h"

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

	/**
	Struct containing information about the game window
	*/
	struct VideoMode{
		// Screen geometry
		uint32_t mScreenWidth, mScreenHeigth;

		// Screen position
		uint32_t mPositionX, mPositionY;
	
		// Pixel depth
		uint32_t mBPP;

		// Screen title
		String mTitle;

		bool mBorderless;

		bool mFullscreenEnabled, mVerticalSyncEnabled;

		VideoMode(uint32_t width=1024, uint32_t heigth=768, const String& title="Game Window",
			bool vsync=true, bool fullscreen=false, uint32_t bpp=32, bool borderless=false) :
			mScreenWidth(width), mScreenHeigth(heigth), mTitle(title), 
				mFullscreenEnabled(fullscreen), mVerticalSyncEnabled(vsync), mBorderless(borderless), mBPP(bpp){
		}
	};

	typedef struct VideoMode VideoMode;

protected:
	VideoMode mVideoMode;

public:
	AGameWindow(){
	}

	AGameWindow(const AGameWindow& other){
		*this = other;
	}

	virtual ~AGameWindow(){
	}

	uint32_t getWidth() const{
		return mVideoMode.mScreenWidth;
	}

	uint32_t getHeight() const{
		return mVideoMode.mScreenHeigth;
	}

	virtual void hook(EventManager* evtManager)=0;

	virtual bool create(const VideoMode& mode)=0;

	virtual bool destroy()=0;

	virtual void swapBuffers() = 0;

	virtual bool isFullscreen() const=0;

	virtual void setScreenSize(uint32_t width, uint32_t height)=0;

	virtual void setFullscreenEnabled(bool state)=0;

	virtual void setVsyncEnabled(bool state)=0;

	virtual void setWindowTitle(const String& title)=0;

	const VideoMode& getVideoMode() const{
		return mVideoMode;
	}

	float getAspectRatio(){
		return (float)mVideoMode.mScreenWidth/(float)mVideoMode.mScreenHeigth;

	}

}; // </GameWindow>

}; // </wt>

#endif
