#ifndef WT_EMPTYFRONTEND_H
#define WT_EMPTYFRONTEND_H


#include "wt/stdafx.h"
#include "wt/TestBackend.h"

namespace wt{

class TestFrontend : public TestBackend {
private:

public:
	void onInit(LuaPlus::LuaState& state){
	}

	void render(float dt){
		mRenderer.render(mScene);
	}

	void update(float dt){
		// update lua state script
		try{
			LuaFunction<void>(mLuaStateManager->getGlobalState(), "onUpdate")(dt);
		}catch(LuaException e){
			WT_EXCEPT(TAG, "Error calling update function");
		}


		// update camera control
		if(mFreeFlyMode){
			mFpsCam.handle(dt, mInput);
		}
		else{
			mTpsCam.handle(dt, mInput);
		}

		// animation & physics
		mScene.update(dt);

		mPhysics->update(dt);
	}

}; // </TestFrontend>

}; // </wt>

#endif
