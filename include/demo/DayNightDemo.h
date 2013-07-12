#ifndef WT_DAYNIGHT_H
#define WT_DAYNIGHT_H

#include "demo/ADemo.h"

#include <wt/SkyDome.h>

namespace wt{


class DayNightDemo : public ADemo{
private:
	SkyDome mSkyDome;
	static const char* TAG;
	bool mPaused;

public:
	DayNightDemo() : mPaused(false){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );

		mSkyDome.render(
			getScene()->getCamera(), getRenderer()->getFrustum());
	}

	String getConfigFile() const{
		return "assets/DayNightDemoConfig.lua";
	}

	void printHelp(){
		LOGI(TAG,
			"Press W/A/S/D to move, use mouse to look around");

		LOGI(TAG,
			"Press P to pause day/night cycle");

		LOGI(TAG,
			"Press UP/DOWN while paused to manually move the sun");

		LOGI(TAG,
			"Press H for help");
	}


	String getLevelFile() const{
		return "world.lua";
	}

	void onUpdate(float dt){
		if(!mPaused){
			mSkyDome.update(dt);
		}
		else{
			if(getManager()->getInput()->isKeyDown(KEY_DOWN)){
				mSkyDome.update(dt*3);
			}
			else if(getManager()->getInput()->isKeyDown(KEY_UP)){
				mSkyDome.update(-dt*3);
			}
		}


		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		// lighting
		glm::vec3 sunPos = mSkyDome.getSunPos();
		DirectionalLight& light = getScene()->getDirectionalLight();
		
		mSkyDome.getLightIntensity(&light.mAmbientIntesity, &light.mDiffuseItensity);

		light.mDirection = glm::normalize( -sunPos );

		ADemo::onUpdate(dt);

		getScene()->getPointLight(0).mPosition = getScene()->getCamera().getPosition() + glm::vec3(0, 3, 0);
	}

	void onKeyDown(VirtualKey c){

		if(c == KEY_p){
			mPaused = !mPaused;
		}

		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getRenderer()->setClearColor( Color::black() );

		mSkyDome.create(
			getAssets()->getModelManager()->getFromPath("$ROOT/geodome")->findGeometryByName("GEOMETRY_1"), // dome geometry
			getAssets()->getTextureManager()->getFromPath("$ROOT/sky/sky"), // sky,
			getAssets()->getTextureManager()->getFromPath("$ROOT/sky/glow"), // glow,
			getAssets()->getTextureManager()->getFromPath("$ROOT/sky/sun")// sun
			);

		getScene()->setSkyBox(NULL);


		PointLight light;
		light.mActive = true;
		light.mColor = Color(0.1, 0.1, .3);
		light.mDiffuseItensity = 0.5;
		light.mAmbientIntesity = 0.5;

		light.mAttenuation.constant = 0;
		light.mAttenuation.linear = 0.1;
		light.mAttenuation.exponential = 0.0;

		getScene()->addPointLight(light);

	}

}; // </DayNightDemo>


const char* DayNightDemo::TAG = "DayNightDemo";

WT_DECLARE_DEMO(DayNightDemo)

}; // </wt>

#endif // </WT_DAYNIGHT_H>
