#ifndef WT_PHYSICSDEMO_H
#define WT_PHYSICSDEMO_H

#include "demo/ADemo.h"

namespace wt{

class PhysicsDemo : public ADemo{
private:
	// Simulation paused
	bool mPhysicsPaused;

	// Slow-motion enabled
	bool mSlowMotion;

public:
	PhysicsDemo() : mPhysicsPaused(false), mSlowMotion(false){
	}

	void onRender(float dt){
		getRenderer()->render( *getScene() );
	}

	void onUpdate(float dt){
		if(!mPhysicsPaused){
			// Acount for the slow-motion / physics pause
			getPhysics()->update(dt * ( mSlowMotion ? 0.1f : 1.0f) );
		}

		getScene()->update(dt);
		getCameraControl()->handle(dt, getManager()->getInput());
	}

	void onMouseDown(float x, float y, MouseButton btn){
		RaycastHitEvent hit;

		if(getScene()->getPhysics()->pick(getScene()->getCamera(),
			getRenderer()->getFrustum(), glm::vec2(x, y),
			glm::vec2(getManager()->getWindow()->getWidth(), getManager()->getWindow()->getHeight()), hit, 1)){

				if(hit.mPickedActor->getType() != PhysicsActor::eDYNAMIC_ACTOR){
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
		if(c == KEY_t){
			spawnBox(
				getScene()->getCamera().getPosition(), getScene()->getCamera().getForwardVec()*30.0f, 0x800000 /* collide only with terrain */);
		}
		else if(c == KEY_p){
			mPhysicsPaused = !mPhysicsPaused;
		}
		else if(c == KEY_f){
			mSlowMotion = !mSlowMotion;
		}
		else if(c == KEY_y){
			spawnBall(
				getScene()->getCamera().getPosition(), getScene()->getCamera().getForwardVec()*30.0f);
		}

		ADemo::onKeyDown(c);
	}

	void onStart(const LuaObject& config){
		getCameraControl()->setCamera(&getScene()->getCamera());

		{
			const glm::vec3 regPos = glm::vec3(200, 0, 200);
			const float regSize = 10;

			getPhysics()->createRegion("test", regPos, regSize);

			SceneActor* actor = getScene()->createActor();
			actor->getTransform().setPosition(regPos);
			actor->getTransform().setScale(regSize, regSize, regSize);

			actor->setModel(getAssets()->getModelManager()->find("sphere"), "default");
		}
		//// Create some stuff
		//spawnBoxStack(glm::vec3(200, 1, 200), 2, 2, 3);

		//spawnBoxStack(glm::vec3(205, 1, 205), 1, 1, 4);

		//for(int i=0; i<3; i++){
		//	spawnBoxStack(glm::vec3(192, 1 + 2.3f*i , 192), 3-i, 3-i, 1);
		//}

		//spawnBall(glm::vec3(38.950851, 37.107651, 190.842422),
		//	glm::vec3(0.525316, -0.850524, -0.025836)*20.0f);

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

	SceneActor* spawnBox(const glm::vec3& position, const glm::vec3& velocity=glm::vec3(0.0f), Uint32 collisionMask=0xFFFFFFFF /* Collide with everything by default */){
		// Create scene actor
		SceneActor* sceneActor = getScene()->createActor();

		//sceneActor->getModel()->get
		sceneActor->getTransform().setPosition(position);

		
		sceneActor->setModel( getAssets()->getModelManager()->find("cube"), "crate" );

		// Create physics actor
		PhysicsActor::Desc desc;

		desc.geometryType = PhysicsActor::eBOX_GEOMETRY;
		desc.controlMode = PhysicsActor::ePHYSICS_MODE;
		desc.type = PhysicsActor::eDYNAMIC_ACTOR;
		desc.geometryDesc.boxGeometry.hx = 1.0f;
		desc.geometryDesc.boxGeometry.hy = 1.0f;
		desc.geometryDesc.boxGeometry.hz = 1.0f;
		desc.pose = sceneActor->getTransform();
		desc.collisionMask = collisionMask;
		desc.group = 1;

		PhysicsActor* physicsActor = getScene()->getPhysics()->createActor(sceneActor, desc);

		PxRigidDynamic* pxActor = (PxRigidDynamic*)physicsActor->getPxActor();
		pxActor->setLinearVelocity(
			PxVec3(velocity.x, velocity.y, velocity.z));

		return sceneActor;
	}

	SceneActor* spawnBall(const glm::vec3& position, const glm::vec3& velocity=glm::vec3(0.0f), Uint32 collisionMask=0xFFFFFFFF /* Collide with everything by default */){
		// Create scene actor
		SceneActor* sceneActor = getScene()->createActor();

		sceneActor->getTransform().setPosition(position);

		sceneActor->setModel( getAssets()->getModelManager()->find("sphere"), "default" );

		// Create physics actor
		PhysicsActor::Desc desc;

		desc.geometryType = PhysicsActor::eSPHERE_GEOMETRY;
		desc.controlMode = PhysicsActor::ePHYSICS_MODE;
		desc.type = PhysicsActor::eDYNAMIC_ACTOR;
		desc.geometryDesc.sphereGeometry.radius = 1.0f;
		desc.pose = sceneActor->getTransform();
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

	String getConfigFile() const{
		return "assets/PhysicsDemoConfig.lua";
	}

	String getLevelFile() const{
		return "assets/demo/PhyscsDemo/level.lua";
	}
}; // </PhysicsDemo>

WT_DECLARE_DEMO(PhysicsDemo)

}; // </wt>

#endif // </WT_PHYSICS_H>
