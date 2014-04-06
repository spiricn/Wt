#include "wt/stdafx.h"

#include "wt/SDLGameInput.h"

#define TD_TRACE_TAG "SDLGameInput"

namespace wt
{

SDLKey SDLGameInput::toSDLKey(VirtualKey code) const{
	return (SDLKey)code;
}

VirtualKey SDLGameInput::fromSDLKey(SDLKey code) const{
	return (VirtualKey)code;
}

uint32_t SDLGameInput::getMousePosX(){
	// TODO
	return 0;
}

uint32_t SDLGameInput::getMousePosY(){
	// TODO
	return 0;
}

SDLGameInput::SDLGameInput() : mEventManager(NULL),  mMouseGrabbed(false){
		
}

bool SDLGameInput::isMouseButtonDown(MouseButton button){
	uint8_t mask=0;

	switch(button)
	{
	case BTN_LEFT:
		mask=SDL_BUTTON_LMASK;
		break;

	case BTN_MIDDLE:
		mask=SDL_BUTTON_MMASK;
		break;

	case BTN_RIGHT:
		mask=SDL_BUTTON_RMASK;
		break;
	}

	return (bool)((SDL_GetMouseState(0, 0) & mask)==0?false:true);
}

bool SDLGameInput::isKeyDown(VirtualKey code){
	uint8_t* keystate = SDL_GetKeyState(NULL);
	return keystate[ toSDLKey(code) ]==0?false:true;
}

int SDLGameInput::getMouseDeltaX(){
	return mMouseDelta[0];
}

int SDLGameInput::getMouseDeltaY(){
	return mMouseDelta[1];
}

int SDLGameInput::getMouseDeltaZ(){
	return 0;
}

void SDLGameInput::pollAndDispatch(){
	if(mEventManager==NULL){
		return;
	}

	mMouseDelta[0] = 0;
	mMouseDelta[1] = 0;

	SDL_Event evt;
	while(SDL_PollEvent(&evt))
	{
		switch(evt.type)
		{
		case SDL_VIDEORESIZE:
			{
				mEventManager->emitEvent(
					new WindowSizeChange(0, 0, evt.resize.w, evt.resize.h)
					);
			break;
			}
		case SDL_KEYUP:
			{
				KeyPressEvent* e = new KeyPressEvent(fromSDLKey(evt.key.keysym.sym),
					KeyPressEvent::UP);
				mEventManager->emitEvent(e);
				break;
			}
		case SDL_KEYDOWN:
			{
				KeyPressEvent* e = new KeyPressEvent(fromSDLKey(evt.key.keysym.sym),
					KeyPressEvent::DOWN);
				mEventManager->emitEvent(e);
				break;
			}

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			{
				int btn = evt.button.button;
				MousePressEvent* e = new MousePressEvent(
					evt.button.button==SDL_BUTTON_LEFT?BTN_LEFT:btn==SDL_BUTTON_RIGHT?BTN_RIGHT:BTN_MIDDLE, 
					evt.button.x, evt.button.y,
					evt.type == SDL_MOUSEBUTTONUP ? MousePressEvent::eBUTTON_UP : MousePressEvent::eBUTTON_DOWN
				);

				mEventManager->emitEvent(e);
				break;
			}
		case SDL_MOUSEMOTION:
			{
				MouseMotionEvent* e = new MouseMotionEvent(evt.button.x, evt.button.y, evt.motion.xrel, evt.motion.yrel);
				mEventManager->emitEvent(e);
				if(mIgnoreNextDelta){
					mIgnoreNextDelta = false;
					mMouseDelta[0] = 0;
					mMouseDelta[1] = 0;
				}
				else{
					mMouseDelta[0] = evt.motion.xrel;
					mMouseDelta[1] = evt.motion.yrel;
				}
				break;
			}
		case SDL_QUIT:
			{
				mEventManager->emitEvent(new AppQuitEvent);
				break;
			}
		}
	}
}

void SDLGameInput::hook(EventManager* evtManager){
	AGameInput::hook(evtManager);
	mEventManager=evtManager;
	mEventManager->registerEvent(WindowSizeChange::TYPE);
}

void SDLGameInput::setMouseGrabbed(bool state){
	SDL_ShowCursor(!state);
	SDL_WM_GrabInput(state==true?SDL_GRAB_ON:SDL_GRAB_OFF);
	mMouseGrabbed=state;

	if(state){
		mIgnoreNextDelta = true;
	}
}

bool SDLGameInput::isMouseGrabbed(){
	return mMouseGrabbed;
}

}; // </wt>
