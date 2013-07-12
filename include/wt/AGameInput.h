#ifndef WT_AGAMEINPUT_H
#define WT_AGAMEINPUT_H



#include "wt/Event.h"
#include "wt/EventManager.h"
#include "wt/AGameWindow.h"

namespace wt{

enum VirtualKey {
	KEY_a='a', KEY_b, KEY_c, KEY_d, KEY_e, KEY_f, KEY_g, KEY_h, KEY_i, KEY_j, KEY_k, KEY_l, KEY_m, KEY_n, KEY_o, KEY_p, KEY_q, KEY_r, KEY_s, KEY_t, KEY_u, KEY_v, KEY_w, KEY_x, KEY_y, KEY_z,
	KEY_0='0', KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	KEY_SPACE=' ', KEY_ESC = 27,
	KEY_ENTER, KEY_BACKSPACE, KEY_DEL, KEY_HOME, KEY_END, KEY_LSHIFT, KEY_RSHIFT, KEY_LCTRL, KEY_RCTRL, KEY_LALT=308, KET_RALT, KEY_PAGEDOWN, KEY_PAGEUP, KEY_TILDE=96, KEY_TAB,
	KEY_UP=273, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	KEY_F1=282, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12
};

enum MouseButton {
	BTN_LEFT=1, BTN_RIGHT=2, BTN_MIDDLE=3
};


class KeyPressEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.SetInteger("keyCode", mCode);
		dst.SetInteger("action", mAction);
	}

	void deserialize(LuaObject& src){
		mCode = (VirtualKey)src.Get("keyCode").ToInteger();
		mAction = (Action)src.Get("action").ToInteger();
	}

public:
	enum Action {DOWN=0, UP=1};

	static const EvtType TYPE;

	VirtualKey mCode;
	Action mAction;

	KeyPressEvent(VirtualKey code=KEY_0, Action action=DOWN) : mCode(code),
		mAction(action){
	}

	const EvtType& getType() const {
		return TYPE;
	}

}; // </KeyPressEvent>

class MouseMotionEvent : public Event{
protected:

	void serialize(LuaObject& dst){
		dst.SetInteger("x", mX);
		dst.SetInteger("y", mY);
		dst.SetInteger("dx", mDx);
		dst.SetInteger("dy", mDy);
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	Uint32 mX, mY;
	Int32 mDx, mDy;

	MouseMotionEvent(Uint32 x, Uint32 y, Int32 dx, Int32 dy) : mX(x), mY(y),
		mDx(dx), mDy(dy){
	}

	const EvtType& getType() const {
		return TYPE;
	}

}; // </MouseMotionEvent>

class MousePressEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.SetInteger("x", mX);
		dst.SetInteger("y", mY);
		dst.SetInteger("button", mButton);
	}

	void deserialize(LuaObject& src){
	}

public:
	enum Action{
		eBUTTON_UP,
		eBUTTON_DOWN
	};


	static const EvtType TYPE;

	Action mAction;
	MouseButton mButton;
	Uint32 mX, mY;

	MousePressEvent(MouseButton btn, Uint32 x, Uint32 y, Action action) : mButton(btn), mX(x), mY(y), mAction(action){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </MousePressEvent>


class AppQuitEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	const EvtType& getType() const {
		return TYPE;

	}
}; // </AppQuitEvent>


////////////////////////////////////////////////////////////////////

class AGameInput{
public:


	static bool isKeyDown(int code){
#if defined(WIN32)
		return 0x8000&GetKeyState(code) ? true : false;
#else
	#error Not implemented on this platform
#endif
	}

	virtual bool isKeyDown(VirtualKey code)=0;

	virtual Int32 getMouseDeltaX()=0;

	virtual Int32 getMouseDeltaY()=0;

	virtual Int32 getMouseDeltaZ()=0;

	virtual Uint32 getMousePosX()=0;

	virtual Uint32 getMousePosY()=0;
	
	virtual void pollAndDispatch()=0;

	virtual bool isMouseButtonDown(MouseButton button)=0;

	virtual void setMouseGrabbed(bool state)=0;

	virtual bool isMouseGrabbed()=0;

	virtual void hook(EventManager* evtManager){
		evtManager->registerInternalEvent(KeyPressEvent::TYPE);
		evtManager->registerInternalEvent(MousePressEvent::TYPE);
		evtManager->registerInternalEvent(MouseMotionEvent::TYPE);
		evtManager->registerInternalEvent(AppQuitEvent::TYPE);
	}

}; // </GameInput>


}; // </wt>

#endif
