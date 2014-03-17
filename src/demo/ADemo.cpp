#include "demo/stdafx.h"

#include "demo/ADemo.h"
#include "wt/SceneLoader.h"
#include "wt/AGameWindow.h"
#include "wt/AGameInput.h"
#include "wt/ScriptProcess.h"
#include "demo/DemoManager.h"
#include "wt/lua/LuaBindings.h"
#include "wt/FileSystemFactory.h"

#define TD_TRACE_TAG "ADemo"

namespace wt{

static const char* DEFAULT_CONFIG_SCRIPT = 
	"cameraRotation = {0.017936, 0.980664, 0.108806, -0.161684}\n"
	"\n"
	"cameraPosition  = {3, 3, 3}\n"
	"\n"
	"cameraMode = 'fps'\n"
	"\n"
	"cameraSpeed = 2\n"
	"\n"
	"mouseGrabbed = 1\n"
	"\n"
	"cameraSpeed = 20.0\n"
	"\n"
	"showGrid = 1\n"
	"\n"
	"usePhysxVDB = 1\n"
	"\n"
	"physxVDBAddress = \"192.168.1.4\"\n"
	"\n"
	"physxVDBPort = 5425\n"
	"\n"
	"fileSystemType = 'DIR'\n"
	"fileSystemRoot = 'd:/Documents/prog/c++/workspace/Wt/assets/'\n"
;

DemoManager* ADemo::getManager(){
	return mDemoManager;
}

bool ADemo::handleEvent(const Sp<Event> e){
	processEvent(e);
	
	return false;
}
math::CameraControler* ADemo::getCameraControl(){
	return &mFpsCam;
}

void ADemo::createDemo(DemoManager* manager){
	String configPath = getScriptPath();

	if(configPath.empty()){
		LOGW("WARNING: No script file provided, using default");
		mMainScript = getLuaState()->createScriptFromString( DEFAULT_CONFIG_SCRIPT );
	}
	else{
		mMainScript = getLuaState()->createScriptFromFile( configPath );
	}

	mDemoManager = manager;

	AEngineFramework::Desc desc;

	// Window
	desc.window.screenWidth = 1280;
	desc.window.screenHeight = 720;

	// PhysX
	desc.physicsDebugging.remoteDebuggingEnabled = true;

	// File system
	desc.fileSystem.type = AFileSystem::eTYPE_LOCAL;
	if(!lua::luaConv(mMainScript->getState().Get("fileSystemRoot"), desc.fileSystem.dir.root)){
		WT_THROW("No file system root specified!");
	}
	
	// Initialize the engine
	initializeFramework(desc);

	// Lua bindings
	lua::LuaBindings_expose( getLuaState()->getGlobals() );

	// Setup scripting lua state
	getLuaState()->expose(*getScene(), "Scene");
	getLuaState()->expose(*getEventManager(), "EventManager");

	getInput()->setMouseGrabbed(true);

	mFpsCam.setCamera(&getScene()->getCamera());

	String scenePath, assetsPath;

	if(lua::luaConv(mMainScript->getState().Get("loadAssets"), assetsPath)){
		LOGD("Assets path provided, loading ...");

		getAssets()->load(assetsPath);
	}

	if(lua::luaConv(mMainScript->getState().Get("loadScene"), scenePath)){
		LOGD("Scene path provided, loading ...");

		SceneLoader loader(getScene(), getAssets());

		loader.load(scenePath);
	}

	getEventManager()->registerGlobalListener(this);
}

void ADemo::onDemoStart(const LuaObject&){
}

void ADemo::onDemoStop(){
}

void ADemo::startDemo(){
	onDemoStart( mMainScript->getState() );

	startMainLoop();

	if(mMainScript->getState().Get("startProcess").GetBoolean() ){
		getProcessManager()->attach( new ScriptProcess( mMainScript ) );
	}

//	// Default font
//	Font* font = getAssets()->getFontManager()->create("_demo_font");
//	font->load("assets/fonts/cour.ttf", 20);
//
//	mUi.create(
//		mWindow->getWidth(),
//		mWindow->getHeight()
//	);
//
//	mUi.setInput(mInput);
//
//	mUi.hook(mEventManager);
//	mUi.setDefaultFont( font );
//
//	getScene()->setUIWindow(&mUi);
//
//
//	bool drawFps;
//	if(!lua::luaConv(mMainScript->getState().Get("drawFps"), drawFps)){
//		drawFps = true;
//	}
//
//	if(drawFps){
//		mFpsTextView = mUi.createView<gui::TextView>();
//
//		mFpsTextView->setPosition(10, 10);
//		mFpsTextView->setSize(200, 30);
//		mFpsTextView->setScalingMode( gui::TextView::eAUTO );
//		mFpsTextView->setBackgroundColor( Color(1, 1, 1, 0.3) );
//	}
//	else{
//		mFpsTextView = NULL;
//	}
//
//	mRunning = true;
//
//#if 0
//	// Setup camera controller
//	PhysicsActor::Desc desc;
//	desc.type = PhysicsActor::eTYPE_DYNAMIC;
//	desc.controlMode = PhysicsActor::eCTRL_MODE_CONTROLLER;
//	desc.controllerDesc.geometryType = PhysicsActor::eCTRL_GEOMETRY_CAPSULE;
//	desc.controllerDesc.geometryDesc.capsuleController.height = 1.0f;
//	desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;
//	math::Transform t;
//	t.setPosition(200, 3, 200);
//	desc.pose = t;
//	mCamController = getPhysics()->createActor(NULL, desc);
//#endif
//
//	printHelp();
//	onStart( mMainScript->getState() );
//
//	if(mMainScript->getState().Get("startProcess").GetBoolean() ){
//		getProcManager().attach( new ScriptProcess( mMainScript ) );
//	}
}

	
void ADemo::onKeyUp(wt::VirtualKey code){
	switch(code){
		case KEY_F2:
			getInput()->setMouseGrabbed(
				!getInput()->isMouseGrabbed());
			break;

		case KEY_F3:
			GLint mode[2];
			glGetIntegerv(GL_POLYGON_MODE, mode);

			getRenderer()->setPolygoneMode((Renderer::PolygonMode)(mode[1]==GL_LINE?GL_FILL:GL_LINE));

			break;

		case KEY_ESC:
			stopMainLoop();
			break;

		case KEY_F5:
			{
				glm::vec3 pos;
				getScene()->getCamera().getTranslation(pos);
				LOGI("Camera position: = {%f, %f, %f}", pos.x, pos.y, pos.z);

				glm::quat rot;
				getScene()->getCamera().getRotation(rot);
				LOGI("Camera rotation = {%f, %f, %f, %f}", rot.x, rot.y, rot.z, rot.w);

				glm::vec3 fw;
				getScene()->getCamera().getForwardVector(fw);
				LOGI("Camera facing = {%f, %f, %f}", fw.x, fw.y, fw.z);


				float h = getScene()->getPhysics()->getTerrainHeightAt(glm::vec2(pos.x, pos.z));

				LOGI("Terrain position = {%f, %f, %f}", pos.x, h, pos.z);
			}
			break;

		/*case KEY_F9:{
			mGuiEnabled = !mGuiEnabled;
			break;
			}*/
		case KEY_F10:{
			String path = "screenshots\\screenshot-" + utils::getCurrentTime() + ".bmp";

			try{
				getRenderer()->saveScreenshot(path);
			}catch(std::exception& e){
				e.what();
				LOGE("Error saving screenshot..");
			}

			LOGI("Screenshot saved \"%s\"", path.c_str());

			break;
		}
		//default:
		//	onKeyDown(code);
	}
}

void ADemo::setName(const String& name){
	mName = name;
}

//void ADemo::stopDemo(){
//	mRunning = false;
//}


//ADemo::~ADemo(){
//}
//
//ADemo::ADemo() : mDemoManager(NULL), mRunning(false){
//}

bool ADemo::isRunning() const{
	return mRunning;
}
const String& ADemo::getName() const{
	return mName;
}

void ADemo::destroyDemo(){
}

String ADemo::getScriptPath() const{
	return "";
}

}; // </wt>

