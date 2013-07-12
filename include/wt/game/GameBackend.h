#ifndef WTG_GAMEBACKEND_H
#define WTG_GAMEBACKEND_H

#include "game/stdafx.h"

#include "wt/Defines.h"
#include "wt/GLShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Utils.h"
#include "wt/Camera.h"
#include "wt/Frustum.h"
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
#include "wt/LuaStateManager.h"
#include "wt/Font.h"
#include "wt/ParticleEngine.h"
#include "wt/net/Socket.h"

// Gui
#include "wt/Gui/Gui.h"
#include "wt/Gui/TextView.h"
#include "wt/Gui/ProgressView.h"
#include "wt/Gui/TextInput.h"
#include "wt/ConditionVar.h"

#include "Game.h"

//#define WT_PRERENDER_MODE

namespace wt{

class GameBackend : public EventListener, public Assets::ALoadListener{
protected:
	static const String TAG;

	Scene mScene;
	Renderer mRenderer;
	math::Camera* mCamera;
	math::Frustum mFrustum;
	math::TPSCameraControler mTpsCam;
	math::FPSCameraControler mFpsCam;
	Physics* mPhysics;
	Assets mAssets;
	AGameWindow* mWindow;
	AGameInput* mInput;
	EventManager* mEventManager;
	LuaStateManager* mLuaStateManager;
	Gui::UIWindow mUi;
	ParticleEmitter mParticles;
	ProcessManager mProcManager;

	std::map<String, float> mPerfMap;

	float mTimeMod;
	bool mRenderGui;

	//Gui::TextView* mFpsText;
	Mutex mLoadMutex;
	ConditionVar mLoadCV;

	FILE* mLogFile;
	bool mFreeFlyMode;
	bool mAppQuit;

	LuaPlus::LuaStateOwner mInitState;

	float mFps;

public:
	virtual ~GameBackend(){
	}

	GameBackend() : mCamera( new math::Camera() ), mTpsCam(mCamera), mFpsCam(mCamera),
			mFreeFlyMode(true), mPhysics(NULL), mTimeMod(1.0f), mAppQuit(false), mRenderGui(true){
						mWindow = new SDLGameWindow();
		mInput = new SDLGameInput();
		mEventManager = &EventManager::getSingleton();
		mLuaStateManager = &LuaStateManager::getSingleton();
		mInput->hook(mEventManager);
		
		mScene.setCamera(mCamera);
		
	}


	void release(){
		delete mWindow;
		delete mInput;
		delete mEventManager;
		delete mPhysics;
		delete mLuaStateManager;
	}

	void prefLog(const String& key, float val){
		WT_UNUSED(key);
		WT_UNUSED(val);
		/*mPerfMap[key] = val;

		LOGI("Performance", "\"%s\" = %f seconds", key.c_str(), val);*/
	}

	void init(){
		net::Socket::initializeLib();

		// setup logging
		// setup work dir
		const char* workDir = "d:/documents/prog/c++/workspace/wt/"; //"Z:\\";
		wt::Utils::setHomeDir(workDir);

		mLogFile = fopen("assets/log.txt", "w");
		WT_ASSERT(mLogFile != NULL, "Error openning log file");

		Log::setOutput(mLogFile);
		LOG("New log session %s", wt::Utils::getCurrentTime("%H:%M:%S %d/%b/%Y").c_str());

		LOG("Setting working directory \"%s\"", workDir);

		// execute init script
		
		
		if(mInitState->DoFile("assets/scripts/init.lua")){
			WT_EXCEPT("init", "Exception", "Error executing init script");
		}

		// create window
		mWindow->create(AGameWindow::VideoMode(mInitState->GetGlobal("SCREEN_WIDTH").ToInteger(),
			mInitState->GetGlobal("SCREEN_HEIGHT").ToInteger(),
			mInitState->GetGlobal("WINDOW_TITLE").ToString(),
			mInitState->GetGlobal("VSYNC").ToInteger()==1?true:false,
			mInitState->GetGlobal("FULLSCREEN").ToInteger()==1?true:false
			)
		);
		mWindow->hook(mEventManager);

		// initialize glew
		GLenum r = glewInit();
		if(r != GLEW_OK){
			WT_THROW("Error initializing glew \"%s\"",
				(const char*)glewGetErrorString(r));
		}

		// initialize renderer
		mRenderer.init(mWindow->getVideoMode().mScreenWidth,
			mWindow->getVideoMode().mScreenHeigth);

		// setup camera
		if(strcmp("fps", mInitState->GetGlobal("cameraType").ToString())==0){
			mFreeFlyMode=true;

			mFpsCam.setMoveSpeed( mInitState->GetGlobal("cameraSpeed").ToNumber() );
			mCamera->setPosition( Lua::ltableToVec3(mInitState->GetGlobal("cameraPosition")));
			mCamera->setRotation( Lua::ltableToQuat(mInitState->GetGlobal("cameraRotation")));
		}
		
		// load assets
		mAssets.load();
		onLoadDone();
		//mAssets.threadedLoad(this);


		//mLoadCV.wait(mLoadMutex);

		mainLoop();
	}

	virtual void onLoadBegin(){
	}

	virtual void onLoadProgress(float dt){
		printf("\r                \r"
			"Loading: %.2f %%", dt*100.0f
			);
	}

	virtual void onLoadDone(){
		// initialize PhysX
		mPhysics = new Physics();
		mPhysics->hook(mEventManager);

		/*try{
			mPhysics->connectToVisualDebugger("192.168.1.4", 5425, 100);
		}catch(Exception& e){
		}*/

		mScene.setPhysics(mPhysics);

		mScene.load("assets/scene.lua");

		// Setup GUI
		Font* font = FontManager::getSingleton().create("default");
		font->load("assets/fonts/cour.ttf", 15);

		mUi.create(mWindow->getWidth(), mWindow->getHeight());

		mUi.hook(mEventManager);
		
		mLuaStateManager->registerObject(mCamera, 
			mLuaStateManager->exposeObject(mCamera, "CameraMetaTable"),
			"Camera");

		mLuaStateManager->registerObject(mEventManager,
			mLuaStateManager->exposeObject(mEventManager, "EventManagerMetaTable"),
			"EventManager");
		
		mLuaStateManager->registerObject(&mScene,
			mLuaStateManager->exposeObject(&mScene, "SceneMetaTable"),
			"Scene");

		try{
			mLuaStateManager->init("assets/scripts/state.lua");
		}catch(std::exception&){
			exit(1);
		}

		mEventManager->registerGlobalListener(this);

		
		TheGame.input = mInput;
		TheGame.eventManager = &wt::EventManager::getSingleton();
		TheGame.scene = &mScene;
		TheGame.window = mWindow;
		TheGame.physics = mPhysics;
		TheGame.freeFlyMode = &mFreeFlyMode;
		TheGame.sound = &wt::SFSoundSystem::getSingleton();
		TheGame.procManager = &mProcManager;


		onInit(*mInitState.Get());

		mInput->setMouseGrabbed(true);

		mLoadCV.wakeAll();
	}

	void plantSomeTrees(){
	}

	virtual bool handleEvent(const Sp<Event> evt){
		const EvtType& type = evt->getType();

		if(type == KeyPressEvent::TYPE){
			const KeyPressEvent* e = static_cast<const KeyPressEvent*>(evt.get());
			if(e->mAction == KeyPressEvent::DOWN){
				onKeyDown(e->mCode);
			}
			else{
				onKeyUp(e->mCode);
			}
		}

		else if(type == AppQuitEvent::TYPE){
			mAppQuit = true;
		}

		else if(type == WindowSizeChange::TYPE){
			const WindowSizeChange* e = static_cast<const WindowSizeChange*>(evt.get());
			onWindowSizeChanged(e->newWidth, e->newHeight);
		}

		else if(type == MouseMotionEvent::TYPE){
			const MouseMotionEvent* e = static_cast<const MouseMotionEvent*>(evt.get());

			onMouseMotion(e);

			if(mInput->isMouseButtonDown(BTN_MIDDLE)){
				pickActor(e->mX, e->mY);
			}
		}

		else if(type == MousePressEvent::TYPE){
			const MousePressEvent* e = static_cast<const MousePressEvent*>(evt.get());

			if(e->mButton == BTN_MIDDLE){
				pickActor(e->mX, e->mY);
			}

			onMouseDown(e->mX, e->mY, e->mButton);
		}

		return true;
	}

	void pickActor(float x, float y){
		glm::mat4x4 modelView;
		mCamera->getMatrix(modelView);

		glm::vec3 point = math::unProject(x, y, mWindow->getVideoMode().mScreenWidth,
			mWindow->getVideoMode().mScreenHeigth, modelView,
			mRenderer.getFrustum().getProjMatrix());

		//mPhysics->pick(mCamera->getPosition(), glm::normalize(point-mCamera->getPosition()));
	}

	virtual void onMouseMotion(const MouseMotionEvent* evt){
		WT_UNUSED(evt);
	}

	void mainLoop(){
		StopWatch time;

		time.reset();
		float dt=0.0f;

		LOGI("Main loop running.");
		StopWatch perfTime;

		GLDBG("MainLoop init");
#ifndef WT_PRERENDER_MODE
		while(!mInput->isKeyDown(KEY_ESC) && !mAppQuit){
			mInput->pollAndDispatch();
			mEventManager->tick();

			dt = time.getSeconds() * mTimeMod;

		
			perfTime.reset();
			mProcManager.upate(dt);
			update(dt);
			prefLog("UPDATE", perfTime.getSeconds());

			

			render(dt);
			
			mWindow->swapBuffers();
		}
#else
		float tpf=0.0f;
		float fps = 60.0f;
		float dur = 30.0f;
		Uint32 numFrames = (Uint32)(dur*fps);

		mVideoEnc.setFrameRate(60.0f);
		mVideoEnc.setSize(mWindow->getWidth(), mWindow->getHeight());
		const char* loc="test2.avi";
		mVideoEnc.start(loc);

		Image frame;

		

		float elapsed=0.0f;
		time.reset();
		for(Uint32 i=0; i<numFrames; i++){
			
			mInput->pollAndDispatch();
			mEventManager->tick();

			dt = 1/fps;

			update(dt);
			render(dt);
			
			mWindow->swapBuffers();

			frame.fromFrameBuffer();

			mVideoEnc.encodeFrame((const char*)frame.getData(), mWindow->getWidth(), mWindow->getHeight());

			float renderTime = time.getSeconds();
			elapsed += renderTime;

			tpf = (tpf*i + renderTime)/(i+1);
			float remTime = (numFrames-i)*tpf;
			String fmt;
			
			printf("\r");
			LOGI("Rendering %.2f%% ( ETA %s elapsed %s )",
				((float)i/numFrames)*100.0f,
				Utils::formatSeconds(remTime, "%H:%M:%S").c_str(),
				Utils::formatSeconds(elapsed, "%H:%M:%S").c_str()
				);
		}
		mVideoEnc.finish();
		LOGI("Done");
#endif

	}

	void spawnCrate(const glm::vec3& pos, bool shoot=true){

		SceneActor* actor = mScene.createDynamicActor();

		PhysicsActor::Desc desc;

		desc.pose.setPosition(pos);
		desc.type = PhysicsActor::eDYNAMIC_ACTOR;

		desc.controlMode = PhysicsActor::ePHYSICS_MODE;

		desc.geometryType = PhysicsActor::eBOX_GEOMETRY;
		desc.geometryDesc.boxGeometry.hx = 1.0f;
		desc.geometryDesc.boxGeometry.hy = 1.0f;
		desc.geometryDesc.boxGeometry.hz = 1.0f;

		actor->setModel( ModelManager::getSingleton().getFromPath("$ROOT/cube"), "crate");
		mPhysics->createActor(actor, desc);

		glm::vec3 fw = mCamera->getForwardVec()*15.0f;

		((PxRigidDynamic*)actor->getPhysicsActor()->getPxActor())->setAngularDamping(0.75);
		if(shoot){
			((PxRigidDynamic*)actor->getPhysicsActor()->getPxActor())->setLinearVelocity(PxVec3(fw.x, fw.y, fw.z)); 
			((PxRigidDynamic*)actor->getPhysicsActor()->getPxActor())->setAngularVelocity(PxVec3(10, 10, 10));
		}
	}

	void spawnBall(const glm::vec3& pos, bool shoot=true){
		//DynamicActor* sphere = mScene.createDynamicActor();
		//sphere->setModel( SkinnedModelManager::getSingleton().find("ball") );

		//sphere->getTransform().setPosition(pos);
		//PxTransform transform(PxVec3(pos.x, pos.y, pos.z), PxQuat::createIdentity()); // 
		//
		//PxRigidDynamic *actor = mPhysics->createDynamic(PxSphereGeometry(1.0f), sphere);

		//glm::vec3 fw = mCamera->getForwardVec();
		//fw *= 15;

		//actor->setAngularDamping(0.75);
		//if(shoot){
		//	actor->setLinearVelocity(PxVec3(fw.x, fw.y, fw.z)); 
		//	actor->setAngularVelocity(PxVec3(10, 10, 10));
		//}

	}

	virtual void onFpsChanged(float fps){
		WT_UNUSED(fps);
	}



	/** virtual callback */
	virtual void onActorPicked(SceneActor* actor, const RaycastHitEvent* evt){
		WT_UNUSED(actor);
		WT_UNUSED(evt);
	}

	/** virtual callback */
	virtual void render(float dt){
	}

	/** virtual callback */
	virtual void onMouseDown(float x, float y, MouseButton btn){
	}

	/** virtual callback */
	virtual void onWindowSizeChanged(Uint32 w, Uint32 h){
		mRenderer.setViewPort(w, h);
	}

	/** virtual callback */
	virtual void onKeyUp(VirtualKey code){
	}

	/** virtual callback */
	virtual void onKeyDown(VirtualKey code){
		switch(code){
		case KEY_1:
			mInput->setMouseGrabbed(
				!mInput->isMouseGrabbed());
			break;

		case KEY_2:
			GLint mode[2];
			glGetIntegerv(GL_POLYGON_MODE, mode);

			mRenderer.setPolygoneMode((Renderer::PolygonMode)(mode[1]==GL_LINE?GL_FILL:GL_LINE));
			//glPolygonMode(GL_FRONT_AND_BACK, mode[1]==GL_LINE?GL_FILL:GL_LINE);

			break;

		case KEY_4:
			mFreeFlyMode = !mFreeFlyMode;
			break;

		case KEY_p:
			{
				const glm::vec3& pos = mCamera->getPosition();
				LOGI("Camera position: = {%f, %f, %f}", pos.x, pos.y, pos.z);

				glm::quat rot;
				mCamera->getRotation(rot);
				LOGI("Camera rotation = {%f, %f, %f, %f}", rot.x, rot.y, rot.z, rot.w);

				glm::vec3 fw = mCamera->getForwardVec();
				LOGI("Camera facing = {%f, %f, %f}", fw.x, fw.y, fw.z);


				float h = mPhysics->getTerrainHeightAt(glm::vec2(pos.x, pos.z));

				LOGI("Terrain position = {%f, %f, %f}", pos.x, h, pos.z);
			}
			break;

		case KEY_5:{
			String path = "Screenshots\\Screenshot-" + Utils::getCurrentTime() + ".bmp";

			try{
				bool s = mRenderGui;
				mRenderGui=false;
				render(0.0f);
				mWindow->swapBuffers();
				mRenderer.saveScreenshot(path);
				mRenderGui=s;
				render(0.0f);
				mWindow->swapBuffers();
			}catch(std::exception& e){
				e.what();
				LOGE("Test", "Error saving screenshot..");
			}

			LOGI("Test", "Screenshot saved \"%s\"", path.c_str());

			break;
		}
		case KEY_6:
			TheGame.sound->setMuted( !TheGame.sound->isMuted() );
			LOGI("Teset", TheGame.sound->isMuted() ? "Sound muted." : "Sound unmuted" );
			break;
		}
	}

	/** virtual callback */
	virtual void update(float dt){
	}

	/** virtual callback */
	virtual void onInit(LuaPlus::LuaState& script){
	}

}; // </GameBackend>

const String GameBackend::TAG = "Test";

}; // </wt>

#endif
