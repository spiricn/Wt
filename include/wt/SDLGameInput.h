#ifndef WT_SDLGAMEINPUT_H
#define WT_SDLGAMEINPUT_H


#include "wt/stdafx.h"

#include "wt/AGameInput.h"
#include "wt/EventManager.h"

namespace wt{

class SDLGameInput : public AGameInput{
public:
	uint32_t getMousePosX();

	uint32_t getMousePosY();

	SDLGameInput();

	bool isMouseButtonDown(MouseButton button);

	bool isKeyDown(VirtualKey code);

	int getMouseDeltaX();

	int getMouseDeltaY();

	int getMouseDeltaZ();

	void pollAndDispatch();

	void hook(EventManager* evtManager);

	void setMouseGrabbed(bool state);

	bool isMouseGrabbed();

private:
	bool mIgnoreNextDelta;
	bool mMouseGrabbed;
	int mMouseDelta[2];
	EventManager* mEventManager;

	SDLKey toSDLKey(VirtualKey code) const;

	VirtualKey fromSDLKey(SDLKey code) const;

}; // </SDLGameInput>

}; // </wt>

#endif
