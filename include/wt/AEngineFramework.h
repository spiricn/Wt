#ifndef WT_AENGINEFRAMEWORK_H
#define WT_AENGINEFRAMEWORK_H

#include "wt/GameInputFactory.h"
#include "wt/GameWindowFactory.h"
#include "wt/TimeAccumulator.h"
#include "wt/lua/State.h"
#include "wt/ProcessManager.h"
#include "wt/Utils.h"
#include "wt/IEngineServiceLocator.h"
#include "wt/Physics.h"
#include "wt/Scene.h"
#include "wt/Renderer.h"

namespace wt
{

class AEngineFramework : public IEngineServiceLocator{
public:
	struct Desc;

	AEngineFramework();

	virtual ~AEngineFramework();

	virtual void onUpdate(float dt) = 0;

	void stopMainLoop();

	void startMainLoop();

	void processEvent(const wt::Sp<wt::Event> evt);

	void initializeFramework(const Desc& desc);


	virtual void onWindowSizeChanged(uint32_t w, uint32_t h);

	virtual void onKeyUp(wt::VirtualKey code);

	virtual void onKeyDown(wt::VirtualKey code);
	
	virtual void onMouseDown(float x, float y, wt::MouseButton btn);

	virtual void onMouseUp(float x, float y, wt::MouseButton btn);

	virtual void onMouseMotion(const wt::MouseMotionEvent* evt);

	virtual void onAppQuit();

	virtual AGameWindow* getWindow() const;

	virtual AGameInput* getInput() const;

	virtual EventManager* getEventManager() const;

	virtual ProcessManager* getProcessManager() const;

	virtual lua::State* getLuaState() const;

	virtual Scene* getScene() const;
	
	virtual Physics* getPhysics() const;

	virtual AResourceSystem* getAssets() const;

	virtual Renderer* getRenderer() const;

	struct Desc{
		struct Logging{
			String filePath;
			bool enabled;

			Logging() : filePath("log.txt"), enabled(true){
			}
		} logging;

		AGameWindow::Desc window;

		String homeDir;

		float mainLoopStep;

		AResourceSystem::Desc assets;

		AFileSystem::Desc fileSystem;

		struct PhysicsDebugging{
			String remoteDebuggerURI;
			uint32_t remoteDebuggerPort;

			bool remoteDebuggingEnabled;

			PhysicsDebugging() : remoteDebuggerURI("127.0.0.1"), remoteDebuggerPort(5425), remoteDebuggingEnabled(false){
			}
		} physicsDebugging;

		Desc() : mainLoopStep(1/60.0f), homeDir("."){
		}
	}; // </Desc>

private:
	void mainLoop();

	AGameWindow* mWindow;
	AGameInput* mInput;
	EventManager* mEventManager;
	ProcessManager* mProcessManager;
	lua::State* mLuaState;
	FILE* mLogFile;
	Desc mDesc;
	bool mInitialized;
	bool mRunning;

	AResourceSystem* mAssets;
	Scene* mScene;
	Renderer* mRenderer;
	Physics* mPhysics;
}; // </AEngineFramework>

} // </wt>

#endif // </WT_AENGINEFRAMEWORK_H>