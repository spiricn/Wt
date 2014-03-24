#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include <wt/ActorMoveController.h>

using namespace wt;

#define TD_TRACE_TAG "PhysicsDemo"

using namespace wt;


class PhysicsDemo : public ADemo{
public:
	PhysicsDemo() : mPhysicsPaused(false), mSlowMotion(false){
	}

	ActorMoveController* mMover;

	void onBeforeSystemUpdate(int32_t system, float dt){
		if(system == eSYSTEM_PHYSICS){

			mMover->update(
				getInput()->isKeyDown(KEY_UP),
				getInput()->isKeyDown(KEY_DOWN),
				false,
				false,
				getInput()->isKeyDown(KEY_LEFT),
				getInput()->isKeyDown(KEY_RIGHT),
				dt);

			static bool justJumped = false;

			String newAnimation = "";
			bool loop = true;

			if(mMover->isJumping()){
				if(mMover->getJumpState() == ActorMoveController::eJUMP_START){
					newAnimation = "jump_start";
					loop = false;
				}
				else if( (mMover->getJumpState() == ActorMoveController::eJUMP_RISING || mMover->getJumpState() == ActorMoveController::eJUMP_FALLING) && !mActor->getAnimationPlayer()->isPlaying()){
					newAnimation = "jump";
					loop = true;
				}
				else if(mMover->getJumpState() == ActorMoveController::eJUMP_LANDED){
					newAnimation = mMover->isMoving() ? "jump_end_run" : "jump_end";
					loop = false;

					justJumped = true;
				}
			}
			else if(mMover->isMoving()){
				if(mMover->startedMoving() || mMover->changedMoveDirection() || ( justJumped && !mActor->getAnimationPlayer()->isPlaying())){
					justJumped = false;
					newAnimation = mMover->movingBackward() ? "walk_backwards" : "run";
					loop = true;
				}
			}
			else if(mMover->isRotating()){
				if(mMover->startedRotating() || mMover->stoppedMoving()){
					newAnimation = getInput()->isKeyDown(KEY_LEFT) ? "shuffle_left" : "shuffle_right";
				}
			}
			else if(mMover->stoppedMoving() || mMover->stoppedRotating() || ( justJumped && !mActor->getAnimationPlayer()->isPlaying())){
				newAnimation = "stand_1";
				justJumped = false;
			}

			if(newAnimation.size()){
				//LOG("%s", newAnimation.c_str());
				//mActor->getAnimationPlayer()->play(newAnimation, loop);
				mActor->blendToAnimation(newAnimation, 0.3, loop);
			}
		}
		else if(system == eSYSTEM_RENDERER){
			getCameraControl()->handle(dt, getInput());
		}
	}

	bool handleEvent(const Sp<Event> e){
		if(e->getType() == RegionEvent::TYPE){
			LOG("########\nRegion event\n##########");
			return true;
		}
		else{
			return ADemo::handleEvent(e);
		}
	}

	void onUpdate(float dt){
		
	}

	void onMouseDown(float x, float y, MouseButton btn){
		RaycastHitEvent hit;

		if(getScene()->getPhysics()->pick(getScene()->getCamera(), glm::vec2(x, y),
			glm::vec2(getWindow()->getWidth(), getWindow()->getHeight()), hit, 1)){

				if(hit.mPickedActor->getType() != PhysicsActor::eTYPE_DYNAMIC){
					// Ignore terrain
					return;
				}

				PxRigidDynamic* pxActor = (PxRigidDynamic*)hit.mPickedActor->getPxActor();
				
				// Nudge it upwards
				PxVec3 vel = pxActor->getLinearVelocity();
				vel += PxVec3(0, 10, 0);
				pxActor->setLinearVelocity(vel);
		}
	}

	void onKeyDown(VirtualKey c){
		glm::vec3 fw, pos;
		getScene()->getCamera().getForwardVector(fw);
		getScene()->getCamera().getTranslation(pos);

		fw *= -1.0f;

		if(c == KEY_t){
			spawnBox(
				pos, fw*30.0f /* collide only with terrain */);
		}
		else if(c == KEY_j){
			mMover->jump();
		}
		else if(c == KEY_p){
			if(mPhysicsPaused){
				setActiveSystems( getActiveSystems() | AEngineFramework::eSYSTEM_PHYSICS );
			}
			else{
				setActiveSystems( getActiveSystems() & ~AEngineFramework::eSYSTEM_PHYSICS );
			}

			mPhysicsPaused = !mPhysicsPaused;
		}
		else if(c == KEY_f){
			mSlowMotion = !mSlowMotion;

			setSystemTimeMod(eSYSTEM_PHYSICS, mSlowMotion ? 0.1f : 1.0f);
		}
		else if(c == KEY_y){
			spawnBall(
				pos, fw*30.0f);
		}

		ADemo::onKeyDown(c);
	}

	void onDemoStart(const LuaObject& config){

		{
			glm::vec3 pos(187.071808, -7.917824, 236.465485);
			float size = 5;
			getScene()->getPhysics()->createRegion("test", pos, size);

			ModelledActor* actor = getScene()->createModelledActor();
			
			actor->setModel(getAssets()->getModelManager()->find("sphere"), "default");
			actor->getTransformable()->setScale(glm::vec3(size, size, size));
			actor->getTransformable()->setTranslation(pos);
		}

		setSystemTimeMod(eSYSTEM_SCENE | eSYSTEM_PHYSICS, 1);

		mCubeModel = getAssets()->getModelManager()->find("cube");
		WT_ASSERT(mCubeModel != NULL, "Missing demo resource");

		mSphereModel = getAssets()->getModelManager()->find("sphere");
		WT_ASSERT(mSphereModel != NULL, "Missing demo resource");
		getCameraControl()->setCamera(&getScene()->getCamera());

		//// Create some stuff
		//spawnBoxStack(glm::vec3(304.983246, 45.222458, 245.641754), 2, 2, 3);

		//spawnBoxStack(glm::vec3(304.983246, 50.222458, 245.641754), 1, 1, 4);

		//for(int i=0; i<3; i++){
		//	spawnBoxStack(glm::vec3(137, 1 + 2.3f*i , 238), 3-i, 3-i, 1);
		//}

		//spawnBall(glm::vec3(38.950851, 37.107651, 190.842422),
		//	glm::vec3(0.525316, -0.850524, -0.025836)*20.0f);

		{
			mActor = dynamic_cast<wt::ModelledActor*>(getScene()->findActorByName("actor"));
			WT_ASSERT(mActor != NULL, "Missing actor");


			mActor->getAnimationPlayer()->play("stand_1", true);

			PhysicsActor::Desc desc;
			desc.type = PhysicsActor::eTYPE_DYNAMIC;
			desc.controlMode = PhysicsActor::eCTRL_MODE_CONTROLLER;
			desc.controllerDesc.geometryType = PhysicsActor::eCTRL_GEOMETRY_CAPSULE;
			desc.controllerDesc.geometryDesc.capsuleController.height = 1.0f;
			desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;
			mActor->getTransformable()->getTransformMatrix(desc.pose);
			getScene()->getPhysics()->createActor(mActor, desc);

			mMover = new ActorMoveController(mActor);
		}

		{
			setCameraController(eCAM_CTRL_FPS);
			/*math::TPSCameraControler* ctrl = dynamic_cast<math::TPSCameraControler*>(getCameraControl());
			ctrl->zoomOut(10);
			ctrl->setTarget(mActor->getTransformable());*/
		}
	}


	void spawnBoxStack(const glm::vec3& position, Uint32 numRows, Uint32 numColumns, Uint32 height){
		for(Uint32 i=0; i<numRows; i++){
			for(Uint32 j=0; j<numColumns; j++){
				for(Uint32 k=0; k<height; k++){
					spawnBox(glm::vec3(position.x + i * (2.0f + 0.1f), position.y + k * (2.0f + 0.1f), position.z + j *(2.0f + 0.1f)));
				}
			}
		}
	}

	ModelledActor* spawnBox(const glm::vec3& position, const glm::vec3& velocity=glm::vec3(0.0f), Uint32 collisionMask=0xFFFFFFFF /* Collide with everything by default */){
		// Create scene actor
		ModelledActor* sceneActor = getScene()->createModelledActor();

		sceneActor->getTransformable()->setTranslation(position);

		sceneActor->setModel( mCubeModel, "default" );

		// Create physics actor
		PhysicsActor::Desc desc;

		desc.geometryType = PhysicsActor::eGEOMETRY_BOX;
		desc.controlMode = PhysicsActor::eCTRL_MODE_PHYSICS;
		desc.type = PhysicsActor::eTYPE_DYNAMIC;
		desc.geometryDesc.boxGeometry.hx = 1.0f;
		desc.geometryDesc.boxGeometry.hy = 1.0f;
		desc.geometryDesc.boxGeometry.hz = 1.0f;
		sceneActor->getTransformable()->getTransformMatrix(desc.pose);
		desc.collisionMask = collisionMask;
		desc.group = 1;

		PhysicsActor* physicsActor = getScene()->getPhysics()->createActor(sceneActor, desc);

		PxRigidDynamic* pxActor = (PxRigidDynamic*)physicsActor->getPxActor();
		pxActor->setLinearVelocity(
			PxVec3(velocity.x, velocity.y, velocity.z));

		return sceneActor;
	}

	ModelledActor* spawnBall(const glm::vec3& position, const glm::vec3& velocity=glm::vec3(0.0f), Uint32 collisionMask=0xFFFFFFFF /* Collide with everything by default */){
		// Create scene actor
		ModelledActor* sceneActor = getScene()->createModelledActor();

		sceneActor->getTransformable()->setTranslation(position);

		sceneActor->setModel( mSphereModel, "default" );

		// Create physics actor
		PhysicsActor::Desc desc;

		desc.geometryType = PhysicsActor::eGEOMETRY_SPHERE;
		desc.controlMode = PhysicsActor::eCTRL_MODE_PHYSICS;
		desc.type = PhysicsActor::eTYPE_DYNAMIC;
		desc.geometryDesc.sphereGeometry.radius = 1.0f;
		sceneActor->getTransformable()->getTransformMatrix(desc.pose);
		desc.collisionMask = collisionMask;
		desc.group = 0x1;

		PhysicsActor* physicsActor = getScene()->getPhysics()->createActor(sceneActor, desc);

		PxRigidDynamic* pxActor = (PxRigidDynamic*)physicsActor->getPxActor();

		pxActor->setLinearVelocity(
			PxVec3(velocity.x, velocity.y, velocity.z));
		

		return sceneActor;
	}

	void printHelp(){
		LOGI("PhysicsDemo", "*************************");

		LOGI("PhysicsDemo",
			"Press T to throw a box");

		LOGI("PhysicsDemo",
			"Press Y to throw a ball");

		LOGI("PhysicsDemo",
			"Press 1 to lock/unlock the mouse");

		LOGI("PhysicsDemo",
			"Click on a box to nudge it upwards");

		LOGI("PhysicsDemo",
			"Press P to pause physics simulaion");

		LOGI("PhysicsDemo",
			"Press F to toggle slow-motion");

		LOGI("PhysicsDemo",
			"Press H for help");

		LOGI("PhysicsDemo", "*************************");
	}

	String getScriptPath() const{
		return "demo_workspace/PhysicsDemo/main.lua";
	}

private:
		Model* mCubeModel;
		Model* mSphereModel;
		// Simulation paused
		bool mPhysicsPaused;

		// Slow-motion enabled
		bool mSlowMotion;

		wt::ModelledActor* mActor;
}; // </PhysicsDemo>

WT_DECLARE_DEMO_IMPL(PhysicsDemo)


