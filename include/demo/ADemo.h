#ifndef WT_ADEMO_H
#define WT_ADEMO_H

#include "wt/Defines.h"
#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Utils.h"
#include "wt/Camera.h"
#include "wt/Timer.h"
#include "wt/Scene.h"
#include "wt/Renderer.h"
#include "wt/AssimpModelLoader.h"
#include "wt/DevilImageLoader.h"
#include "wt/FPSCameraControler.h"
#include "wt/TPSCamControler.h"
#include "wt/CameraControler.h"
#include "wt/SkyBox.h"

#include "wt/Buffer.h"
#include "wt/Animation.h"
#include "wt/Font.h"
#include "wt/ImageManager.h"
#include "wt/Physics.h"
#include "wt/TextureArray.h"
#include "wt/ModelManager.h"
#include "wt/Model.h"
#include "wt/Assets.h"
#include "wt/SDLGameWindow.h"
#include "wt/SDLGameInput.h"
#include "wt/Font.h"
#include "wt/net/Socket.h"
#include "wt/Lua.h"
#include "wt/gui/Window.h"
#include "wt/gui/TextView.h"
#include "wt/ProcessManager.h"
#include "wt/lua/State.h"

#include "wt/AEngineFramework.h"

namespace wt{

class DemoManager;

#if 1

class ADemo : public AEngineFramework, public EventListener{
public:
	const String& getName() const;

	void createDemo(DemoManager* manager);

	void destroyDemo();

	void startDemo();

	void setName(const String& name);

	void onKeyDownPriv(VirtualKey code);

	bool isRunning() const;

	DemoManager* getManager();

	virtual String getScriptPath() const;

	virtual bool handleEvent(const Sp<Event> e);

	virtual void onDemoStart(const LuaObject&);

	virtual void onDemoStop();

	math::CameraControler* getCameraControl();

	void onKeyUp(wt::VirtualKey code);

private:
	String mName;
	DemoManager* mDemoManager;
	lua::ScriptPtr mMainScript;
	bool mRunning;
	math::FPSCameraControler mFpsCam;
};

#else
class ADemo  : public AEngineFramework{
friend class DemoManager;

public:
	enum CameraControlMode{
		eCAMERA_FPS,
		eCAMERA_TPS
	};

	ADemo();

	virtual ~ADemo();

	EventManager* getEventManager() const{
		return mEventManager;
	}

	lua::State& getLuaState();

	void update(float dt);

	Assets* getAssets() const;

	Scene* getScene() const;

	gui::Window& getUi();

	Physics* getPhysics() const;

	void stopDemo();

	ProcessManager& getProcManager(){
		return mProcManager;
	}

	Renderer* getRenderer() const;

	bool isRunning() const;

	math::CameraControler* getCameraControl();

	void setCameraControlMode(CameraControlMode mode);

	const String& getName() const;

	virtual void onRender(float dt);

	virtual void onMouseDown(float x, float y, MouseButton btn);

	virtual void onMouseUp(float x, float y, MouseButton btn);

	virtual void printHelp();

	virtual void onWindowSizeChanged(Uint32 w, Uint32 h);

	virtual void onKeyUp(VirtualKey code);

	virtual void onKeyDown(VirtualKey code);

	virtual void onUpdate(float dt);

	virtual void onMouseMotion(const MouseMotionEvent* evt);

	virtual void onStart(const LuaObject&);

	virtual void onStop();

	virtual String getScriptPath() const;

protected:
	DemoManager* getManager();
	
	void createDemo(DemoManager* manager, AGameWindow* window, AGameInput* input, EventManager* evtManager);

	void destroyDemo();

	void startDemo();

	void setName(const String& name);

	void onKeyDownPriv(VirtualKey code);

	void printHelpPriv();

private:
	lua::ScriptPtr mMainScript;
	math::FPSCameraControler mFpsCam;
	math::TPSCameraControler mTpsCam;
	String mName;

	DemoManager* mDemoManager;
	Renderer* mRenderer;
	Physics* mPhysics;
	Assets* mAssets;
	Scene* mScene;

	AGameInput* mInput;
	AGameWindow* mWindow;
	CameraControlMode mCameraCtrlMode;
	bool mRunning;
	bool mShowGrid;
	ProcessManager mProcManager;
	gui::Window mUi;
	FPSCalculator mFpsCalc;
	gui::TextView* mFpsTextView;
	PhysicsActor* mCamController;
	EventManager* mEventManager;
	lua::State mLuaState;
	bool mGuiEnabled;
};
#endif
#define WT_DECLARE_DEMO(name) ADemo* createDemoFunc_ ## name();

#define WT_DECLARE_DEMO_IMPL(name) ADemo* createDemoFunc_ ## name(){ return new name; }

}; // </wt>

#endif // </WT_ADEMO_H>