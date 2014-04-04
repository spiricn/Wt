#include "demo/stdafx.h"

#include "demo/ADemo.h"

#include <wt/ActorMoveController.h>

#include <wt/ModelledActor.h>
#include <wt/TransformableAnimator.h>
#include <wt/Interpolator.h>
#include <wt/ProcessSpawner.h>
#include <wt/AnimatorProcess.h>
using namespace wt;

#define TD_TRACE_TAG "PhysicsDemo"

using namespace wt;


static IEngineServiceLocator* s;
static AEngineFramework* f;
static ADemo* d;

class MoverProcess : public AProcess{
public:
	enum Animation{
		eANI_RUN,
		eANI_WALK,
		eANI_JUMP_START,
		eANI_JUMP_END,
		eANI_JUMP,
		eANI_SHUFFLE_RIGHT,
		eANI_SHUFFLE_LEFT,
		eANI_WALK_BACKWARDS,
		eANI_STAND,
		eANI_MAX
	}; // </Animation>

	MoverProcess(ASceneActor* actor) : mActor(actor){
		mCtrl = new ActorMoveController(actor);
	}

	MoverProcess* addWaypoint(const glm::vec3& pos){
		mWaypoints.push_back(pos);
		return this;
	}

	void moveTo(const glm::vec3& pos){
		mWaypoints.clear();
		addWaypoint(pos);
	}

	void setAnimation(Animation type, const String& name){
		mAnimations[type] = name;
	}

	void onProcUpdate(float dt){
		bool move = true;

		if(!mWaypoints.empty()){
			glm::vec3 currPos;
			mActor->getTransformable()->getTranslation(currPos);

			glm::vec3 waypoint = *mWaypoints.begin();

			// Face the target
			glm::vec3 direction = glm::cross(  glm::normalize( ((waypoint - currPos) * glm::vec3(1, 0, 1)) ), glm::vec3(0, 1, 0) );
			mActor->getTransformable()->lookAt(currPos + direction);


			if( glm::length(currPos - waypoint) < 5 ){
				mWaypoints.erase(mWaypoints.begin());
			}
		}
		else{
			move = false;
		}

		mCtrl->update(dt, move);

		String newAnimation = "";

		if(mCtrl->moving()){
			if(mCtrl->startedMoving() || mCtrl->changedMoveDirection()){
				newAnimation = mAnimations[eANI_RUN];
			}
		}
		else if(mCtrl->stoppedMoving()){
			newAnimation = mAnimations[eANI_STAND];
		}

		if(newAnimation.size()){
			ModelledActor* actor = dynamic_cast<ModelledActor*>(mActor);
			actor->blendToAnimation(newAnimation, 0.3, true);
		}
	}

private:
	typedef std::vector<glm::vec3> WaypointList;

private:
	String mAnimations[eANI_MAX];
	WaypointList mWaypoints;
	ASceneActor* mActor;
	ActorMoveController* mCtrl;
}; // </MoverProcess>


class PhysicsDemo : public ADemo{
public:
	PhysicsDemo() : mPhysicsPaused(false), mSlowMotion(false){
	}

	void onBeforeSystemUpdate(int32_t system, float dt){
	}

	bool handleEvent(const Sp<Event> e){
		if(e->getType() == RegionEvent::TYPE){
			const RegionEvent* evt = (const RegionEvent*)e.get();

			if(evt->type == RegionEvent::eACTOR_ENTERED_REGION){
				if(!evt->regionActor->getSceneActor()->getName().compare("door_region")){
					ModelledActor* door = (ModelledActor*)getScene()->findActorByName("door");

					getProcessManager()->attach( (new AnimatorProcess(door))
					->addAnimation("open", 0.5, false)
					->addAnimation("openned", 0, 0, 0, true, 0)
					);

					getScene()->deleteActor(evt->regionActor->getSceneActor());
					getScene()->getPhysics()->removeActor(door->getPhysicsActor());
				}
				else if(!evt->regionActor->getSceneActor()->getName().compare("scene_end_region")){
					LOGI("END!");
				}
			}
		}

		return ADemo::handleEvent(e);
	}

	void onUpdate(float dt){
	}

	void onMouseDown(float x, float y, MouseButton btn){
		RaycastHitEvent hit;

		if(getScene()->getPhysics()->pick(getScene()->getCamera(), glm::vec2(x, y),
			glm::vec2(getWindow()->getWidth(), getWindow()->getHeight()), hit, 0xFFFFFFFF, Physics::ePICK_TERRAIN)){
				mMover->moveTo(hit.mImpact);
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
		mActor = dynamic_cast<wt::ModelledActor*>(getScene()->findActorByName("actor"));
		WT_ASSERT(mActor != NULL, "Missing actor");

		PhysicsActor::Desc desc;
		desc.type = PhysicsActor::eTYPE_DYNAMIC;
		desc.controlMode = PhysicsActor::eCTRL_MODE_CONTROLLER;
		desc.controllerDesc.geometryType = PhysicsActor::eCTRL_GEOMETRY_CAPSULE;
		desc.controllerDesc.geometryDesc.capsuleController.height = 1.0f;
		desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;
		mActor->getTransformable()->getTransformMatrix(desc.pose);
		getScene()->getPhysics()->createActor(mActor, desc);

		mMover = new MoverProcess(mActor);

		mMover->setAnimation(MoverProcess::eANI_RUN, "run");
		mMover->setAnimation(MoverProcess::eANI_STAND, "stand_1");

		getProcessManager()->attach(mMover);

		mCubeModel = getAssets()->getModelManager()->find("cube");
		mSphereModel = getAssets()->getModelManager()->find("sphere");

		getCameraControl()->setMode(CameraController::eCTRL_MODE_TPS);
		getCameraControl()->setTarget(mActor->getTransformable(), glm::vec3(0, 15, 0));
		getCameraControl()->setDistance(20);
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
		MoverProcess* mMover;
}; // </PhysicsDemo>

WT_DECLARE_DEMO_IMPL(PhysicsDemo)


