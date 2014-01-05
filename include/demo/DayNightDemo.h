#ifndef WT_DAYNIGHT_H
#define WT_DAYNIGHT_H

#include "demo/ADemo.h"

#include "wt/SkydomeRenderer.h"
#include <wt/SkyDome.h>

#define TD_TRACE_TAG "DayNightDemo"

namespace wt{

class DayNightDemo : public ADemo{
private:
	SkyDome mSkyDome;
	bool mPaused;

public:
	DayNightDemo() : mPaused(false){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );

		//mSkyDome.render(getScene()->getCamera());
	}


	void printHelp(){
		LOGI(
			"Press W/A/S/D to move, use mouse to look around");

		LOGI(
			"Press P to pause day/night cycle");

		LOGI(
			"Press UP/DOWN while paused to manually move the sun");

		LOGI(
			"Press H for help");
	}


	void onUpdate(float dt){
		if(!mPaused){
			//mSkyDome.update(dt);
		}
		else{
			if(getManager()->getInput()->isKeyDown(KEY_DOWN)){
				mSkyDome.advance(dt*3);
			}
			else if(getManager()->getInput()->isKeyDown(KEY_UP)){
				mSkyDome.advance(-dt*3);
			}
		}


		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		//
		////{
		////	// Directional light
		////	glm::vec3 sunPos = mSkyDome.getSunPos();
		////	DirectionalLight::Desc light;
		////	getScene()->getDirectionalLight();
		////
		////	mSkyDome.getLightIntensity(&light.ambientIntensity, &light.diffuseIntensity);

		////	light.direction = glm::normalize( -sunPos );

		////	getScene()->setDirectionalLight(light);
		////}

		////{
		////	// Point light
		////	PointLight light;
		////	getScene()->getPointLight(0, light);

		////	light.position = getScene()->getCamera().getPosition() + glm::vec3(0, 3, 0);

		////	getScene()->setPointLight(0, light);
		////}


		glm::vec3 sunPos = mSkyDome.getSunPos();

		glm::vec3 center;
		ModelledActor* a = dynamic_cast<ModelledActor*>(getScene()->findActorByName("cube"));
		a->getTransformable()->getTranslation(center);


		Scene::ShadowMappingDesc desc = getScene()->getShadowMappingDesc();

		//math::Camera* dst = &getScene()->getCamera();
		math::Camera* dst = &desc.casterSource;


		dst->setTranslation(center + 120.0f * glm::normalize(center-sunPos));
		dst->lookAt(center);
		
		getScene()->setShadowMappingDesc(desc);

	}

	void onKeyDown(VirtualKey c){
		if(c == KEY_p){
			mPaused = !mPaused;
			mSkyDome.setPaused(mPaused);
		}

		ADemo::onKeyDown(c);
	}


	String getScriptPath() const{
		return "assets/demo/TestDemo/main.lua";
	}

	void onStart(const LuaObject& config){
		getRenderer()->setClearColor( Color::Black() );

		mSkyDome.create(
			getAssets()->getModelManager()->getFromPath("$ROOT/geodome")->findGeometryByName("GEOMETRY_1"), // dome geometry
			getAssets()->getTextureManager()->getFromPath("$ROOT/skydome/sky"), // sky,
			getAssets()->getTextureManager()->getFromPath("$ROOT/skydome/glow"), // glow,
			getAssets()->getTextureManager()->getFromPath("$ROOT/skydome/sun")// sun
			);

		getScene()->setSkyBox(NULL);

		getRenderer()->attachRenderer(new SkyDomeRenderer);

		getScene()->insertCustomActor(&mSkyDome);

		Scene::ShadowMappingDesc desc = getScene()->getShadowMappingDesc();
		desc.enabled = true;
		desc.casterSource.setTranslation(glm::vec3(206.809372, 95.402977, 239.410477));
		desc.casterSource.setOrientation(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
		getScene()->setShadowMappingDesc(desc);


		//PointLight::Desc desc;
		//desc.enabled = true;
		//desc.color= Color(0.1, 0.1, .3);
		//desc.diffuseIntensity = 0.5;
		//desc.ambientIntensity = 0.5;

		//desc.attenuation.constant = 0;
		//desc.attenuation.linear = 0.1;
		//desc.attenuation.quadratic = 0.0;

		//getScene()->createPointLight("", desc);

	}

}; // </DayNightDemo>


WT_DECLARE_DEMO(DayNightDemo)

}; // </wt>

#endif // </WT_DAYNIGHT_H>
