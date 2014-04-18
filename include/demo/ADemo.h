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
#include "wt/CameraController.h"
#include "wt/SkyBox.h"
#include "wt/OwnPtr.h"
#include "wt/Buffer.h"
#include "wt/Animation.h"
#include "wt/Font.h"
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
#include "wt/gui/WindowManager.h"
#include "wt/gui/TextView.h"
#include "wt/ProcessManager.h"
#include "wt/lua/State.h"

#include "wt/AEngineFramework.h"

namespace wt
{

class DemoManager;

class ADemo : public AEngineFramework, public IEventListener{
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

	virtual bool handleEvent(const EventPtr e);

	virtual void onDemoStart(const LuaObject&);

	virtual void onDemoStop();

	CameraController* getCameraControl();

	void onKeyUp(wt::VirtualKey code);

	void update(float dt);

private:
	String mName;
	DemoManager* mDemoManager;
	lua::ScriptPtr mMainScript;
	bool mRunning;
	CameraController* mCamCtrl;
	gui::WindowManager mWindowManager;
}; // </ADemo>

#define WT_DECLARE_DEMO(name) ADemo* createDemoFunc_ ## name();

#define WT_DECLARE_DEMO_IMPL(name) ADemo* createDemoFunc_ ## name(){ return new name; }

}; // </wt>

#endif // </WT_ADEMO_H>