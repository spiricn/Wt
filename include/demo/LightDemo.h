#ifndef WT_LIGHTDEMO_H
#define WT_LIGHTDEMO_H

#include "demo/ADemo.h"
#include "wt/CameraAnimation.h"

#define TD_TRACE_TAG "LightDemo"

namespace wt{

struct LightOrb{
private:
	GenericAnimator mAnimator;
	ModelledActor* mActor;
	float mColorFactor;
	float mCycleTime;

public:

	LightOrb(ModelledActor* actor, Animation* anim, float clrCycleTime) : mActor(actor), mColorFactor(0), mAnimator(GenericAnimator(anim)), mCycleTime(clrCycleTime){
	}

	void update(float dt){
		glm::vec3 pos, scale;
		glm::quat rot;
		mAnimator.update(dt,
			pos,
			scale,
			rot);

		mActor->getTransform().setPosition(pos);
		mActor->getTransform().setRotation(rot);

		mColorFactor = fmod(mColorFactor + dt, mCycleTime);
	}

	Color getColor() const{
		return Color::fromHSV(mColorFactor/mCycleTime, 0.5, 0.5);
	}

	glm::vec3 getPosition() const{
		return mActor->getTransform().getPosition();
	}
}; // </LightOrb>

class LightDemo : public ADemo{
private:
	CameraAnimationBuilder mBuilder;
	std::vector<Sp<LightOrb>> mOrbs;
	int mFollowing;

public:

	LightDemo() : mFollowing(-1){
	}

	void onMouseDown(float x, float y, MouseButton btn){
		if(btn == BTN_RIGHT){
			static bool fz=false;

			if(fz){
				mBuilder.addKeyframe();
				LOG("ADD");
			}

			else{
				LOG("START");
				fz = true;
				mBuilder.start(&getScene()->getCamera(), "ani_demo_1");
			}
		}
		else if(btn == BTN_MIDDLE){
			mBuilder.stop();
			mBuilder.saveAnimation("light_animation.wta");
			LOG("DONE");
		}
	}

	void printHelp(){
		LOGI(
			"Press W/A/S/D to move, use mouse to look around");

		LOGI(
			"Press F to toggle flashlight");

		LOGI(
			"Press R to toggle following one of the light orbs");

		LOGI(
			"Press H for help");
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		
		getPhysics()->update(dt);
		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());

		// Move spotlight to the camera
		SpotLight l;
		getScene()->getSpotLight(0, l);
		l.mPosition = getScene()->getCamera().getPosition();
		l.direction = getScene()->getCamera().getForwardVec();
		getScene()->setSpotLight(0, l);

	
		// Update orbs
		for(Uint32 i=0; i<mOrbs.size(); i++){
			mOrbs[i]->update(dt);

			PointLight light;
			getScene()->getPointLight(i, light);

			light.mColor = mOrbs[i]->getColor();
			light.mPosition = mOrbs[i]->getPosition();

			getScene()->setPointLight(i, light);
		}
	
		// Handle camera movement
		if(mFollowing == -1){
			//ADemo::onUpdate(dt);
		}
		else{
			getScene()->getCamera().setPosition(
				mOrbs[mFollowing]->getPosition());
		}
	}

	void onKeyDown(VirtualKey c){
		ADemo::onKeyDown(c);

		if(c == KEY_f){
			// Toggle flashlight
			SpotLight light;
			getScene()->getSpotLight(0, light);
			
			light.mActive = light.mActive;

			getScene()->setSpotLight(0, light);
		}
		else if(c == KEY_r){
			mFollowing++;
			if(mFollowing == mOrbs.size()){
				mFollowing = -1;
			}
		}
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());


		
		// Setup light orbs

		const char* anims[3] = {
			"light_animation1.wta",
			"light_animation2.wta",
			"light_animation3.wta"
		};

		for(int i=0; i<3; i++){

			// Create actor
			ModelledActor* actor = getScene()->createModelledActor();
			actor->setModel( getAssets()->getModelManager()->find("sphere"), "default" );

			// Load animation
			Animation* ani  = getAssets()->getAnimationManager()->create(anims[i]);
			FileIOStream stream(anims[i], AIOStream::eMODE_READ);
			getAssets()->getAnimationManager()->getLoader()->load(&stream, ani);

			// Create orb
			Sp<LightOrb> orb = new LightOrb(actor, ani,
				i == 0 ? 7 : 1
				);
			mOrbs.push_back(orb);
			
			// Setup corresponding point light
			PointLight l;
			l.mActive = true;
			l.mPosition = glm::vec3(210, 10, 210);

			l.mColor =  Color::white();
			l.mAmbientIntesity = 2;
			l.mDiffuseItensity = 2;

			l.mAttenuation.constant = 0;
			l.mAttenuation.linear = 0;
			l.mAttenuation.exponential = 0.01;
			getScene()->addPointLight(l);
		}

		// Setup spot light
		SpotLight sl;

		sl.cutoff = 30.0f;
		sl.direction = glm::vec3(0, -1, 0);
		sl.mActive = false;
		sl.mPosition = glm::vec3(210, 10, 210);

		sl.mColor =  Color::white();
		sl.mAmbientIntesity = 2;
		sl.mDiffuseItensity = 2;

		sl.mAttenuation.constant = 0;
		sl.mAttenuation.linear = 0;
		sl.mAttenuation.exponential = 0.01;

		getScene()->addSpotLight(sl);


		// Setup global lighting / fog
		DirectionalLight light;
		getScene()->getDirectionalLight(light);
		
		light.mAmbientIntesity = .1;
		light.mDiffuseItensity = .1;

		getScene()->setDirectionalLight(light);

		getScene()->getFog().color = Color::black();
	}

	String getConfigFile() const{
		return "assets/LightDemoConfig.lua";
	}

	String getLevelFile() const{
		return "world.lua";
	}

}; // </LightDemo>

WT_DECLARE_DEMO(LightDemo)

}; // </wt>

#endif // </WT_LIGHTDEMO_H>
