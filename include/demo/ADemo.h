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
#include "wt/TerrainChunk.h"
#include "wt/ModelManager.h"
#include "wt/Model.h"
#include "wt/Assets.h"
#include "wt/SDLGameWindow.h"
#include "wt/SDLGameInput.h"
#include "wt/Font.h"
#include "wt/net/Socket.h"
#include "wt/Lua.h"
#include "wt/GUI/gui.h"
#include "wt/GUI/TextView.h"
#include "wt/ProcessManager.h"

namespace wt{

class DemoManager;

class ADemo{
friend class DemoManager;

public:
	enum CameraControlMode{
		eCAMERA_FPS,
		eCAMERA_TPS
	};

protected:
	DemoManager* getManager();
	
	void createDemo(DemoManager* manager, AGameWindow* window, AGameInput* input, EventManager* evtManager);

	void destroyDemo();

	void startDemo();

	void setName(const String& name);

	void onKeyDownPriv(VirtualKey code);

	void printHelpPriv();

private:
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
	Gui::UIWindow mUi;
	FPSCalculator mFpsCalc;
	Gui::TextView* mFpsTextView;
	PhysicsActor* mCamController;
	EventManager* mEventManager;
	LuaStateOwner mConfig;
	bool mGuiEnabled;
	
public:
	ADemo();

	virtual ~ADemo();

	EventManager* getEventManager() const{
		return mEventManager;
	}

	void update(float dt);

	Assets* getAssets() const;

	Scene* getScene() const;

	Gui::UIWindow& getUi();

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

	virtual String getConfigFile() const;

	virtual String getLevelFile() const;
};

#define WT_DECLARE_DEMO(name) static ADemo* createDemoFunc_ ## name(){ return new name; }

}; // </wt>

#endif // </WT_ADEMO_H>