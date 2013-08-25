#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include "wt/AGameWindow.h"
#include "wt/AGameInput.h"
#include "demo/DemoManager.h"

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


void ADemo::createDemo(DemoManager* manager, AGameWindow* window, AGameInput* input, EventManager* evtManager){
	String configPath = getConfigFile();
	if(configPath.empty()){
		mConfig->DoString(DEFAULT_CONFIG_SCRIPT);
	}
	else{
		mConfig->DoFile(configPath.c_str());
	}

	mWindow = window;

	mInput = input;

	mEventManager = evtManager;

	mDemoManager = manager;

		
	// Load assets
	Assets::FileSystemType type;

	String typeStr;
	String rfs;
	if(!lua::luaConv(mConfig->GetGlobal("fileSystemType"), typeStr)){
		type = Assets::eFS_DIR;
	}
	else{
		type = typeStr.compare("DIR") == 0 ?  Assets::eFS_DIR : Assets::eFS_ZIP;
	}

	if(!lua::luaConv(mConfig->GetGlobal("fileSystemRoot"), rfs)){
		WT_THROW("No file system root specified!");
	}

	mAssets = new Assets(type, rfs);

	// Initialize PhysX
	mPhysics = new Physics(mEventManager);

	if(mConfig->GetGlobal("usePhysxVDB").ToInteger()==1){
		try{
			mPhysics->connectToVisualDebugger(
				mConfig->GetGlobal("physxVDBAddress").ToString(), // address
				mConfig->GetGlobal("physxVDBPort").ToInteger(),	// port
				100		// timeout
				);
		}catch(Exception& e){
			WT_UNUSED(e);
		}
	}

	// Setup rendering scene
	mScene = new Scene(mPhysics, mAssets, mEventManager, getManager()->getLuaState());

	String cameraMode;
	if(!lua::luaConv(mConfig->GetGlobal("cameraMode"), cameraMode)){
		// default value
		cameraMode = "fps";
	}

	if(cameraMode.compare("fps")==0){
		setCameraControlMode(eCAMERA_FPS);
	}
	else{
		setCameraControlMode(eCAMERA_TPS);
	}

	
	// Initialize the scene renderer (must be done after scene creation due to event registration)
	mRenderer = new Renderer(mEventManager);
	mRenderer->init(mWindow->getVideoMode().mScreenWidth,
		mWindow->getVideoMode().mScreenHeigth);

	mFpsCam.setCamera(&getScene()->getCamera());
	mTpsCam.setCamera(&getScene()->getCamera());

	mInput->setMouseGrabbed( mConfig->GetGlobal("mouseGrabbed").ToInteger() ? true : false );

	{
		// Camera configuration
		glm::vec3 pos;
		glm::quat rot;
		float speed;

		lua::luaConv( mConfig->GetGlobal("cameraPosition"), pos);
		lua::luaConv( mConfig->GetGlobal("cameraRotation"), rot);
		if(!lua::luaConv(mConfig->GetGlobal("cameraSpeed"), speed)){
			// default
			speed = 20.0f;
		}

		mFpsCam.setMoveSpeed(speed);

		//mScene->getCamera().setTranslation(pos);
		//mScene->getCamera().setRotation(rot);
	}

	mShowGrid = mConfig->GetGlobal("showGrid").ToInteger() ? true : false;

	//// Load level
	//if(!getLevelFile().empty()){
	//	lua::LuaStateOwner state;
	//	Sp<AIOStream> stream = mAssets->getFileSystem()->open(getLevelFile(), AIOStream::eMODE_READ);
	//	WT_ASSERT(stream->isOpen(), "Error openning demo level file \"%s\"", getLevelFile().c_str());

	//	try{
	//		lua::doStream(state, *stream);
	//	}catch(LuaPlus::LuaException& e){
	//		WT_THROW("Error loading level file \"%s\" - \"%s\"", getLevelFile().c_str(), e.GetErrorMessage());
	//	}

	//	const LuaObject& table = state->GetGlobal("WORLD");
	//	WT_ASSERT(table.IsTable(), "Error loading level - invalid level table");

	//	const LuaObject& assetsTable = table.Get("ASSETS");

	//	if(!assetsTable.IsTable()){
	//		WT_THROW("Error loading level - missing assets table");
	//	}

	//	mAssets->load(assetsTable);

	//	// lights
	//	LuaObject dirLight = table.Get("directionalLight");
	//	if(dirLight.IsTable()){
	//		mScene->getDirectionalLight().deserialize( dirLight );
	//	}

	//	LuaObject pointLights = table.Get("pointLights");
	//	if(pointLights.IsTable()){
	//		for(LuaPlus::LuaTableIterator iter(pointLights); iter; iter.Next()){
	//			PointLight pointLight;
	//			pointLight.deserialize(iter.GetValue());
	//			mScene->addPointLight(pointLight);
	//		}
	//	}


	//	// Skybox
	//	if(table.Get("skybox").IsString()){
	//		mScene->setSkyBox( mAssets->getSkyBoxManager()->getFromPath(table.Get("skybox").ToString()) );
	//	}


	//	// Terrain
	//	if(table.Get("terrain").IsTable()){
	//		Terrain* terrain = mScene->createTerrain();
	//		terrain->deserialize(mAssets, table.Get("terrain"));
	//	}

	//	// Actors
	//	for(LuaTableIterator i(table.Get("ACTORS")); i; i.Next()){
	//		const char* name = i.GetKey().ToString();
	//		LuaObject table = i.GetValue();
	//		const char* type = table.Get("type").ToString();

	//		if(strcmp(type, "DOODAD")==0){
	//			ModelledActor* sceneActor = mScene->createModelledActor(name);

	//			{
	//				// Deserialize doodad
	//				glm::quat rot;
	//				lua::luaConv(table.Get("rot"), rot);
	//				sceneActor->getTransform().setRotation(rot);

	//				glm::vec3 pos;
	//				lua::luaConv(table.Get("pos"), pos);
	//				sceneActor->getTransform().setPosition(pos);

	//				glm::vec3 scale;
	//				if(!lua::luaConv(table.Get("scale"), scale)){
	//					// default
	//					scale = glm::vec3(1.0f, 1.0f, 1.0f);
	//				}
	//				sceneActor->getTransform().setScale(scale);

	//				const char* model = table.Get("model").ToString();

	//				const char* skin = table.Get("skin").ToString();

	//				sceneActor->setModel(
	//					getAssets()->getModelManager()->getFromPath(model), skin);
	//			}

	//			// Create physics actor
	//			{
	//				PhysicsActor::Desc desc;

	//				desc.pose = sceneActor->getTransform();

	//				desc.type = PhysicsActor::eSTATIC_ACTOR;

	//				desc.controlMode = PhysicsActor::ePHYSICS_MODE;

	//				desc.geometryType = PhysicsActor::eMESH_GEOMETRY;
	//				desc.geometryDesc.meshGeometry.model = sceneActor->getModel();

	//				mScene->getPhysics()->createActor(sceneActor, desc);
	//			}
	//		}
	//	}

	//}
}

String ADemo::getLevelFile() const{
	return "";
}

void ADemo::printHelp(){
	LOGW("No demo-specific help available");
}

Gui::UIWindow& ADemo::getUi(){
	return mUi;
}

void ADemo::destroyDemo(){
	onStop();

	mScene->clear();
	mAssets->unloadAll();

	delete mScene;
	delete mAssets;
	delete mPhysics;
	delete mRenderer;
	mInput->setMouseGrabbed(false);
}

void ADemo::setCameraControlMode(CameraControlMode mode){
	mCameraCtrlMode = mode;
}

void ADemo::update(float dt){
	// Recalculate framerate
	if(mFpsCalc.update(dt) && mFpsTextView){
		float fps = mFpsCalc.getFps();

		char bfr[256];
		sprintf(bfr, "%.2f fps", fps);
		
		if(fps >= 60.0f){
			mFpsTextView->setTextColor(Color::Green());
		}
		else if(fps < 60.0f && fps >= 30.0f){
			mFpsTextView->setTextColor(Color::Yellow());
		}
		else{
			mFpsTextView->setTextColor(Color::Red());
		}

		mFpsTextView->setText(bfr);
	}

	mProcManager.upate(dt);

	onUpdate(dt);


	getUi().draw();

	onRender(dt);

	if(mGuiEnabled){
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		getRenderer()->render(
			getUi().getCanvas().getTexture(),
			glm::vec2(mWindow->getWidth(), mWindow->getHeight()),
			0, 0, mWindow->getWidth(), mWindow->getHeight());
	}
}

void ADemo::startDemo(){
	// Default font
	Font* font = getAssets()->getFontManager()->create("_demo_font");
	font->load("assets/fonts/cour.ttf", 20);

	mUi.create(
		mWindow->getWidth(),
		mWindow->getHeight()
	);

	mUi.setInput(mInput);

	mUi.hook(mEventManager);
	mUi.setDefaultFont( font );


	bool drawFps;
	if(!lua::luaConv(mConfig->GetGlobal("drawFps"), drawFps)){
		drawFps = true;
	}

	if(drawFps){
		mFpsTextView = mUi.createView<Gui::TextView>();

		mFpsTextView->setPosition(10, 10);
		mFpsTextView->setSize(200, 30);
		mFpsTextView->setScalingMode( Gui::TextView::eAUTO );
		mFpsTextView->setBackgroundColor( Color(1, 1, 1, 0.3) );
	}
	else{
		mFpsTextView = NULL;
	}

	mRunning = true;

#if 0
	// Setup camera controller
	PhysicsActor::Desc desc;
	desc.type = PhysicsActor::eDYNAMIC_ACTOR;
	desc.controlMode = PhysicsActor::eCONTROLLER_MODE;
	desc.controllerDesc.geometryType = PhysicsActor::eCAPSULE_CONTROLLER;
	desc.controllerDesc.geometryDesc.capsuleController.height = 1.0f;
	desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;
	math::Transform t;
	t.setPosition(200, 3, 200);
	desc.pose = t;
	mCamController = getPhysics()->createActor(NULL, desc);
#endif

	printHelp();
	onStart( mConfig->GetGlobals() );
}


Assets* ADemo::getAssets() const{
	return mAssets;
}

Scene* ADemo::getScene() const{
	return mScene;
}

Physics* ADemo::getPhysics() const{
	return mPhysics;
}


void ADemo::setName(const String& name){
	mName = name;
}

void ADemo::stopDemo(){
	mRunning = false;
}

Renderer* ADemo::getRenderer() const{
	return mRenderer;
}

ADemo::~ADemo(){
}

ADemo::ADemo() : mDemoManager(NULL), mWindow(NULL), mInput(NULL), mRunning(false), mGuiEnabled(true){
}

bool ADemo::isRunning() const{
	return mRunning;
}

math::CameraControler* ADemo::getCameraControl(){
	return mCameraCtrlMode == eCAMERA_FPS ? (math::CameraControler*)&mFpsCam : (math::CameraControler*)&mTpsCam;
}

const String& ADemo::getName() const{
	return mName;
}

void ADemo::onRender(float dt){
	if(mShowGrid){
		glm::mat4 mv;
		getScene()->getCamera().getCameraMatrix(mv);
	}
}

void ADemo::onMouseDown(float x, float y, MouseButton btn){
}

void ADemo::onWindowSizeChanged(Uint32 w, Uint32 h){
	mRenderer->setViewPort(w, h);
}

void ADemo::onKeyUp(VirtualKey code){
}

void ADemo::printHelpPriv(){
	LOGI(
		"Press F1 for help");

	LOGI(
		"Press F2 to toggle mouse grab");

	LOGI(
		"Press F3 to toggle polygon mode");

	LOGI(
		"Press F4 to stop current demo");

	LOGI(
		"Press F5 to print camera info");

	LOGI(
		"Press F10 to take a screen shot");

	LOGI(
		"Press F9 to hide GUI");

	printHelp();
}


void ADemo::onKeyDownPriv(VirtualKey code){
	switch(code){
		case KEY_F1:
			printHelpPriv();
			break;

		case KEY_F2:
			mInput->setMouseGrabbed(
				!mInput->isMouseGrabbed());
			break;
			
		case KEY_F3:
			GLint mode[2];
			glGetIntegerv(GL_POLYGON_MODE, mode);

			mRenderer->setPolygoneMode((Renderer::PolygonMode)(mode[1]==GL_LINE?GL_FILL:GL_LINE));

			break;


		case KEY_F5:
			{
				glm::vec3 pos;
				mScene->getCamera().getTranslation(pos);
				LOGI("Camera position: = {%f, %f, %f}", pos.x, pos.y, pos.z);

				glm::quat rot;
				mScene->getCamera().getRotation(rot);
				LOGI("Camera rotation = {%f, %f, %f, %f}", rot.x, rot.y, rot.z, rot.w);

				glm::vec3 fw;
				mScene->getCamera().getForwardVector(fw);
				LOGI("Camera facing = {%f, %f, %f}", fw.x, fw.y, fw.z);


				float h = mPhysics->getTerrainHeightAt(glm::vec2(pos.x, pos.z));

				LOGI("Terrain position = {%f, %f, %f}", pos.x, h, pos.z);
			}
			break;

		case KEY_F9:{
			mGuiEnabled = !mGuiEnabled;
			break;
			}
		case KEY_F10:{
			String path = "screenshots\\screenshot-" + utils::getCurrentTime() + ".bmp";
			
			try{
				mRenderer->saveScreenshot(path);
			}catch(std::exception& e){
				e.what();
				LOGE("Error saving screenshot..");
			}

			LOGI("Screenshot saved \"%s\"", path.c_str());

			break;
		}
		default:
			onKeyDown(code);
	}
}

void ADemo::onKeyDown(VirtualKey code){
	
}

void ADemo::onUpdate(float dt){
	// camera movement

	float moveSpeed = 15.0f;

	glm::vec3 disp(0);

	glm::vec3 moveVec(0);
	glm::vec3 fw;
	getScene()->getCamera().getForwardVector(fw);

	if(mInput->isKeyDown(KEY_w)){
		disp += glm::normalize(fw * glm::vec3(1, 0, 1)) * moveSpeed;
	}
	if(mInput->isKeyDown(KEY_s)){
		disp += -glm::normalize(fw * glm::vec3(1, 0, 1)) * moveSpeed;
	}
	if(mInput->isKeyDown(KEY_a)){
		disp += glm::normalize(fw * glm::vec3(1, 0, 1)) * moveSpeed;
	}
	if(mInput->isKeyDown(KEY_d)){
		disp += -glm::normalize(fw * glm::vec3(1, 0, 1)) * moveSpeed;
	}

	// gravity
	disp.y = -30.0f;

	PxVec3 pxDisp;

	pxConvert(disp, pxDisp);


	mCamController->getController()->move(
		pxDisp*dt, 0.0001, dt, PxControllerFilters());


	glm::vec3 camPos;
	pxConvert(mCamController->getController()->getFootPosition(),
		camPos);
	getScene()->getCamera().setTranslation(camPos + glm::vec3(0, 3, 0));

}

void ADemo::onMouseMotion(const MouseMotionEvent* evt){
}

void ADemo::onStart(const LuaObject& config){
}

void ADemo::onStop(){
}

void ADemo::onMouseUp(float x, float y, MouseButton btn){
}

String ADemo::getConfigFile() const{
	return "";
}


}; // </wt>
