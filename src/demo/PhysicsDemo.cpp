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
	MoverProcess(ASceneActor* actor) : mActor(actor){
		mCtrl = new ActorMoveController(actor);
	}

	MoverProcess* addWaypoint(const glm::vec3& pos){
		mWaypoints.push_back(pos);
		return this;
	}

	void setAnimations(const String& run, const String& stand){
		mAnimations[eANI_RUN] = run;
		mAnimations[eANI_STAND] = stand;
	}

	void onProcUpdate(float dt){
		bool move = true;

		if(!mWaypoints.empty()){
			/*glm::vec3 eye;
			mActor->getScene()->getCamera().getTranslation(eye);*/

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
			killProcess();
			move = false;
		}

		mCtrl->update(move, false, false, false, false, false, dt);

		String newAnimation = "";

		if(mCtrl->isMoving()){
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

	enum Animation{
		eANI_RUN,
		eANI_STAND,
		eANI_MAX
	};

	String mAnimations[eANI_MAX];

	typedef std::vector<glm::vec3> WaypointList;
	WaypointList mWaypoints;

	ASceneActor* mActor;
	ActorMoveController* mCtrl;
}; // </MoverProcess>


class CinematicProc : public AProcess{
public:
	enum Phase{
		PHASE1 = 1,
		PHASE2,
		PHASE3,
		PHASE4,
		PHASE5,
		PHASE6,
		PHASE7,
		PHASE8,
		PHASE9,
		PHASE10,
	};

	Phase mPhase;

	CinematicProc(Phase phase=PHASE1, ActorMoveController* move=NULL) : mPhase(phase), mMove(move){
		mActor = dynamic_cast<ModelledActor*>( s->getScene()->findActorByName("actor") );

		mCubeModel = s->getAssets()->getModelManager()->find("cube");
		WT_ASSERT(mCubeModel != NULL, "Missing demo resource");

		mSphereModel = s->getAssets()->getModelManager()->find("sphere");
		WT_ASSERT(mSphereModel != NULL, "Missing demo resource");
		d->getCameraControl()->setCamera(&s->getScene()->getCamera());

		//f->setTimeMod(5);
	}
	 
	ActorMoveController* mMove;
	ModelledActor* mActor;

	struct{
		FloatInterpolator interp;
	} p3;

	struct{
		ActorMoveController* mover;
	} p4;

	void onProcStart(){
		LOG("Start phase %d", mPhase);

		if(mPhase == PHASE1){
			mActor->setVisible(false);

			setNext( 
				(new TransformableAnimator(
				&s->getScene()->getCamera(), s->getAssets()->getAnimationManager()->find("cam_1"), "default", false, true)))
				->setNext( new CinematicProc(PHASE2));

			killProcess();
		}
		else if(mPhase == PHASE2){
			mActor->setVisible(true);
			mActor->getAnimationPlayer()->play("hover", true);

			AProcess* p1 = new TransformableAnimator(
				&s->getScene()->getCamera(), s->getAssets()->getAnimationManager()->find("cam_2"), "default", false, true);

			AProcess* p2 = new CinematicProc(PHASE3);

			setNext( new ProcessSpawner(2, p1, p2) )->setNext(new CinematicProc(PHASE4));

			killProcess();
		}
		else if(mPhase == PHASE3){
			p3.interp = FloatInterpolator(0, 1, 3, false);
		}
		else if(mPhase == PHASE4){
			PhysicsActor::Desc desc;
			desc.type = PhysicsActor::eTYPE_DYNAMIC;
			desc.controlMode = PhysicsActor::eCTRL_MODE_CONTROLLER;
			desc.controllerDesc.geometryType = PhysicsActor::eCTRL_GEOMETRY_CAPSULE;
			desc.controllerDesc.geometryDesc.capsuleController.height = 1.0f;
			desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;
			mActor->getTransformable()->getTransformMatrix(desc.pose);
			s->getScene()->getPhysics()->createActor(mActor, desc);

			mMove = new ActorMoveController(mActor);


			AProcess* p1 = new TransformableAnimator(
				&s->getScene()->getCamera(), s->getAssets()->getAnimationManager()->find("cam_3"), "default", false, true);

			AProcess* p2 = new AnimatorProcess(mActor, "jump_end", 0.08, false, 0.1);

			setNext( new ProcessSpawner(3, p1, p2, new CinematicProc(PHASE5, mMove)) );

			killProcess();
		}
		else if(mPhase == PHASE6){
			spawnBoxStack(glm::vec3( 110, 35, 275), glm::vec3(0, 5, 0),  6, 6, 2, 2, 5);

			f->setSystemTimeMod(AEngineFramework::eSYSTEM_PHYSICS, 0.3f);
			setNext( new TransformableAnimator(
				&s->getScene()->getCamera(), s->getAssets()->getAnimationManager()->find("cam_5"), "default", false, true))->setNext(
				new CinematicProc(PHASE7, mMove));

			killProcess();
		}
		else if(mPhase == PHASE7){
			f->setSystemTimeMod(AEngineFramework::eSYSTEM_PHYSICS, 1.0f);
			AProcess* p1 = new TransformableAnimator(
				&s->getScene()->getCamera(), s->getAssets()->getAnimationManager()->find("cam_6"), "default", false, true);

			MoverProcess* p2 = new MoverProcess(mActor);
			delete mMove;
			mMove = new ActorMoveController(mActor);
			p2->addWaypoint(glm::vec3(119.252914, -4.539143, 112.956436));
			p2->addWaypoint(glm::vec3(260.745270, -76.071754, 112.541550));
			
			setNext(new ProcessSpawner(2, p1, p2));

			killProcess();
		}
	}
	
	void onProcUpdate(float dt){
		if(mPhase == PHASE3){
			if(p3.interp.update(dt)){
				killProcess();
			}
			else{
				DirectionalLight::Desc d = s->getScene()->getDirectionalLight()->getDesc();

				d.ambientIntensity = glm::mix(0.0f, 0.5f, p3.interp.getValue());
				d.diffuseIntensity = glm::mix(0.0f, 1.0f, p3.interp.getValue());

				s->getScene()->setDirectionalLightDesc(d);
			}
		}
		else if(mPhase == PHASE5){
			mMove->update(false, false, false, false, false, false, dt);

			if(!mActor->getAnimationPlayer()->isPlaying()){
				MoverProcess* p1 = new MoverProcess(mActor);
				p1->addWaypoint(glm::vec3(130.387543, -7.229599, 254.166519));
				p1->setAnimations("run", "stand_1");

				p1->setNext(
					new AnimatorProcess(mActor, "kneel_start", 1.0f, false))
					->setNext(
					new AnimatorProcess(mActor, "kneel_loop", 1.0f, false))
					->setNext(
					new AnimatorProcess(mActor, "kneel_end", 1.0f))
				;

				p1->mCtrl->setForwardSpeed(30);

				AProcess* p2 = new TransformableAnimator(
				&s->getScene()->getCamera(), s->getAssets()->getAnimationManager()->find("cam_4"), "default", false, true);

				setNext(new ProcessSpawner(2, p1, p2))->setNext(new CinematicProc(PHASE6, mMove));

				killProcess();
			}
		}
		else if(mPhase == PHASE6){
			killProcess();
		}
	}


	void onProcEnd(){
		LOG("Phase end %d", mPhase);
	}


	
	void spawnBoxStack(const glm::vec3& position, const glm::vec3& vel, Uint32 numRows, Uint32 numColumns, Uint32 height, float scale, float spacing =0.1){
		for(Uint32 i=0; i<numRows; i++){
			for(Uint32 j=0; j<numColumns; j++){
				for(Uint32 k=0; k<height; k++){
					const float x = position.x + i * (scale * 2 + spacing);
					const float y =  position.y + k * (scale * 2 + spacing);
					const float z = position.z + j *(scale * 2 + spacing);

					spawnBox(glm::vec3(x,y, z),vel, 0xFFFFFFFF, scale);
				}
			}
		}
	}

	ModelledActor* spawnBox(const glm::vec3& position, const glm::vec3& velocity=glm::vec3(0.0f), Uint32 collisionMask=0xFFFFFFFF /* Collide with everything by default */, float scale=1.0f){
		// Create scene actor
		ModelledActor* sceneActor = s->getScene()->createModelledActor();

		sceneActor->getTransformable()->setTranslation(position);

		sceneActor->setModel( mCubeModel, "default" );

		// Create physics actor
		PhysicsActor::Desc desc;

		sceneActor->getTransformable()->setScale(glm::vec3(scale, scale, scale));

		desc.geometryType = PhysicsActor::eGEOMETRY_BOX;
		desc.controlMode = PhysicsActor::eCTRL_MODE_PHYSICS;
		desc.type = PhysicsActor::eTYPE_DYNAMIC;
		desc.geometryDesc.boxGeometry.hx = scale;
		desc.geometryDesc.boxGeometry.hy = scale;
		desc.geometryDesc.boxGeometry.hz = scale;
		sceneActor->getTransformable()->getTransformMatrix(desc.pose);
		desc.collisionMask = collisionMask;
		desc.group = 1;

		PhysicsActor* physicsActor = s->getScene()->getPhysics()->createActor(sceneActor, desc);

		PxRigidDynamic* pxActor = (PxRigidDynamic*)physicsActor->getPxActor();
		pxActor->setLinearVelocity(
			PxVec3(velocity.x, velocity.y, velocity.z));

		return sceneActor;
	}

	ModelledActor* spawnBall(const glm::vec3& position, const glm::vec3& velocity=glm::vec3(0.0f), Uint32 collisionMask=0xFFFFFFFF /* Collide with everything by default */){
		// Create scene actor
		ModelledActor* sceneActor = s->getScene()->createModelledActor();

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

		PhysicsActor* physicsActor = s->getScene()->getPhysics()->createActor(sceneActor, desc);

		PxRigidDynamic* pxActor = (PxRigidDynamic*)physicsActor->getPxActor();

		pxActor->setLinearVelocity(
			PxVec3(velocity.x, velocity.y, velocity.z));
		

		return sceneActor;
	}


	Model* mSphereModel;
	Model* mCubeModel;
};


class PhysicsDemo : public ADemo{
public:
	PhysicsDemo() : mPhysicsPaused(false), mSlowMotion(false){
	}

	ActorMoveController* mMover;

	void onBeforeSystemUpdate(int32_t system, float dt){
		//if(system == eSYSTEM_PHYSICS){

		//	mMover->update(
		//		getInput()->isKeyDown(KEY_UP),
		//		getInput()->isKeyDown(KEY_DOWN),
		//		false,
		//		false,
		//		getInput()->isKeyDown(KEY_LEFT),
		//		getInput()->isKeyDown(KEY_RIGHT),
		//		dt);

		//	static bool justJumped = false;

		//	String newAnimation = "";
		//	bool loop = true;

		//	if(mMover->isJumping()){
		//		if(mMover->getJumpState() == ActorMoveController::eJUMP_START){
		//			newAnimation = "jump_start";
		//			loop = false;
		//		}
		//		else if( (mMover->getJumpState() == ActorMoveController::eJUMP_RISING || mMover->getJumpState() == ActorMoveController::eJUMP_FALLING) && !mActor->getAnimationPlayer()->isPlaying()){
		//			newAnimation = "jump";
		//			loop = true;
		//		}
		//		else if(mMover->getJumpState() == ActorMoveController::eJUMP_LANDED){
		//			newAnimation = mMover->isMoving() ? "jump_end_run" : "jump_end";
		//			loop = false;

		//			justJumped = true;
		//		}
		//	}
		//	else if(mMover->isMoving()){
		//		if(mMover->startedMoving() || mMover->changedMoveDirection() || ( justJumped && !mActor->getAnimationPlayer()->isPlaying())){
		//			justJumped = false;
		//			newAnimation = mMover->movingBackward() ? "walk_backwards" : "run";
		//			loop = true;
		//		}
		//	}
		//	else if(mMover->isRotating()){
		//		if(mMover->startedRotating() || mMover->stoppedMoving()){
		//			newAnimation = getInput()->isKeyDown(KEY_LEFT) ? "shuffle_left" : "shuffle_right";
		//		}
		//	}
		//	else if(mMover->stoppedMoving() || mMover->stoppedRotating() || ( justJumped && !mActor->getAnimationPlayer()->isPlaying())){
		//		newAnimation = "stand_1";
		//		justJumped = false;
		//	}

		//	if(newAnimation.size()){
		//		//LOG("%s", newAnimation.c_str());
		//		//mActor->getAnimationPlayer()->play(newAnimation, loop);
		//		mActor->blendToAnimation(newAnimation, 0.3, loop);
		//	}
		//}
		//else if(system == eSYSTEM_RENDERER){
			//getCameraControl()->handle(dt, getInput());
		//}
		getCameraControl()->handle(dt, getInput());
	}

	bool handleEvent(const Sp<Event> e){
		if(e->getType() == RegionEvent::TYPE){
			const RegionEvent* evt = dynamic_cast<const RegionEvent*>(e.get());

			LOGI("Actor %p %s region %d", evt->collidingActor, evt->type == RegionEvent::eACTOR_LEFT_REGION ? "left" : "entered", evt->regionActor->getId());

			if(mActor == evt->collidingActor->getSceneActor()){
				mMover->jump();
			}
			else{
				if(evt->type == RegionEvent::eACTOR_ENTERED_REGION){
					PxVec3 vel =((PxRigidDynamic*)evt->collidingActor->getPxActor())->getLinearVelocity();
					vel += PxVec3(0, 10, 0);
					((PxRigidDynamic*)evt->collidingActor->getPxActor())->setLinearVelocity(vel);
				}
			}

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
				vel += PxVec3(0, 49, 0);
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
		s = this;
		f = this;
		d = this;

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



		//{
		//	mActor = dynamic_cast<wt::ModelledActor*>(getScene()->findActorByName("actor"));
		//	WT_ASSERT(mActor != NULL, "Missing actor");


		//	mActor->getAnimationPlayer()->play("stand_1", true);

		//	PhysicsActor::Desc desc;
		//	desc.type = PhysicsActor::eTYPE_DYNAMIC;
		//	desc.controlMode = PhysicsActor::eCTRL_MODE_CONTROLLER;
		//	desc.controllerDesc.geometryType = PhysicsActor::eCTRL_GEOMETRY_CAPSULE;
		//	desc.controllerDesc.geometryDesc.capsuleController.height = 1.0f;
		//	desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;
		//	mActor->getTransformable()->getTransformMatrix(desc.pose);
		//	getScene()->getPhysics()->createActor(mActor, desc);

		//	mMover = new ActorMoveController(mActor);

		//	MoverProcess* proc;
		///*	getProcessManager()->attach(
		//		(proc = new MoverProcess(mActor))
		//		->addWaypoint(glm::vec3(145.751724, -9.066059, 257.192505))
		//		->addWaypoint(glm::vec3(120.660721, -7.656898, 242.724609))
		//		->addWaypoint(glm::vec3(106.126564, -3.403854, 182.536392))
		//		->addWaypoint(glm::vec3(101.544006, -2.540668, 92.131935))
		//		->addWaypoint(glm::vec3(127.577583, -4.088352, 104.036400))
		//	);
		//	proc->setAnimations("run", "stand_1");*/
		//}

		//{
		//	setCameraController(eCAM_CTRL_FPS);
		//	/*math::TPSCameraControler* ctrl = dynamic_cast<math::TPSCameraControler*>(getCameraControl());
		//	ctrl->zoomOut(10);
		//	ctrl->setTarget(mActor->getTransformable());*/
		//}




		getProcessManager()->attach( new CinematicProc );

		
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
}; // </PhysicsDemo>

WT_DECLARE_DEMO_IMPL(PhysicsDemo)


