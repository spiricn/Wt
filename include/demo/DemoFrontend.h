#ifndef WTG_DemoFrontend_H
#define WTG_DemoFrontend_H

#include <wt/game/GameProc.h>
#include <wt/game/Creature.h>
#include <wt/game/GameActorManager.h>
#include <wt/game/GameLevel.h>
#include <game/CreatureDesc.h>
#include <wt/processmanager.h>
#include <wt/skydome.h>

#include "demo/DemoFrontend.h"

namespace wt{

#if 0
	
class DemoFrontend : public DemoBackend{
public:
	GameLevel mLevel;

	DemoFrontend(){
	}

	void onInit(LuaPlus::LuaState& state){
		// processes
		mProcManager.attach(new CameraCtrlProc);

		// creatures
		mLevel.init("world.lua");

		mFreeFlyMode = false;

		// renderer
		Renderer::GodRayParams p;
		mRenderer.getGodRayParams(p);
		p.mSunPos = glm::vec3(-0.385030, 0.453779, -0.803640)*1000.0f;
		p.mIsEnabled = false;
		p.mSunSize = 30.0f;

		mRenderer.setGodRayParams(p);


		TheGame.sound->setMuted(true);


		//
		LuaFunction<void>(mLuaStateManager->getGlobalState(), "onGameInit")();

	}



	
	void onMouseDown(float x, float y, MouseButton btn){
		DemoBackend::onMouseDown(x, y, btn);
	}

	void onKeyUp(VirtualKey code){
		DemoBackend::onKeyUp(code);
	}

	void onKeyDown(VirtualKey code){
		if(code == KEY_F4){
			LOG("shutting down...");

			mScene.clear();
			mAssets.unloadAll();
		}


		DemoBackend::onKeyDown(code);
	}

	void render(float dt){
		mRenderer.render(mScene);
	}
	

	void update(float dt){
		// processes
		mProcManager.upate(dt);

		// update lua state script
		try{
			LuaFunction<void>(mLuaStateManager->getGlobalState(), "onUpdate")(dt);
		}catch(LuaException e){
			WT_EXCEPT(TAG, "Error calling update function");
		}

		// animation & physics
		mScene.update(dt);
	
		mPhysics->update(dt);

		TheGame.sound->setListenerPosition(mCamera->getPosition());
		TheGame.sound->setListenerForwardVec(mCamera->getForwardVec());

		mFpsCam.handle(dt, mInput);
	}

}; // </TestFrontend>

#endif

} // </Wt>

#endif