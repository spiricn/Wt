#ifndef WT_DOODAD_H
#define WT_DOODAD_H


#include "wt/game/AGameActor.h"

namespace wt{

class Doodad : public AGameActor{
private:
	void create(){
	}

public:

	Doodad(SceneActor* actor, const String& name, Assets* assets) : AGameActor(actor, name, assets){
		if(actor){
			create();
		}
	}

	virtual ~Doodad(){
	}

	virtual void serialize(LuaPlus::LuaObject& dst){
		dst.Set("type", "DOODAD");

		// position
		LuaObject luaPos;
		LUA_NEW_TABLE(luaPos);

		Lua::luaConv(
			mSceneActor->getTransform().getPosition(), luaPos);

		dst.Set("pos", luaPos);

		// orientation
		LuaObject luaRot;
		LUA_NEW_TABLE(luaRot);

		Lua::luaConv(
			mSceneActor->getTransform().getQuat(), luaRot);
		dst.Set("rot", luaRot);

		// scale
		LuaObject luaScale;
		LUA_NEW_TABLE(luaScale);

		Lua::luaConv(
			mSceneActor->getTransform().getScale(), luaScale);
		dst.Set("scale", luaScale);

		dst.Set("model", mSceneActor->getModel()->getPath().c_str());
		dst.Set("skin", mSceneActor->getSkin()->getName().c_str());
	}


	/* AMoveable */

	virtual void setRotation(const glm::quat& quat){
		mSceneActor->getTransform().setRotation(quat);
		updatePxTransform();
	}

	virtual void move(const glm::vec3& delta, float dt=1.0f){
		mSceneActor->getTransform().translate(delta*dt);

		updatePxTransform();
	}

	void updatePxTransform(){
		if(mSceneActor->getPhysicsActor()){
			physx::PxTransform pxPose;

			pxConvert(mSceneActor->getTransform(), pxPose);

			((PxRigidStatic*)mSceneActor->getPhysicsActor()->getPxActor())->setGlobalPose(
				pxPose);
		}
	}

	void setPosition(const glm::vec3& position){
		mSceneActor->getTransform().setPosition(position);

		if(mSceneActor->getPhysicsActor()){
			physx::PxTransform pxPose;

			pxConvert(mSceneActor->getTransform(), pxPose);

			((PxRigidStatic*)mSceneActor->getPhysicsActor()->getPxActor())->setGlobalPose(
				pxPose);
		}
	}

	void setScale(const glm::vec3& scale){
		mSceneActor->getTransform().setScale(scale);

		physx::PxTransform pxPose;

		pxConvert(mSceneActor->getTransform(), pxPose);

		((PxRigidStatic*)mSceneActor->getPhysicsActor()->getPxActor())->setGlobalPose(
			pxPose);
	}

	virtual void rotate(float x, float y, float z, float angle){
		mSceneActor->getTransform().rotate(x, y, z, angle);

		physx::PxTransform pxPose;

		pxConvert(mSceneActor->getTransform(), pxPose);

		((PxRigidStatic*)mSceneActor->getPhysicsActor()->getPxActor())->setGlobalPose(
			pxPose);
	}

	virtual const glm::vec3& getPosition() const{
		return mSceneActor->getTransform().getPosition();
	}

	virtual const glm::vec3& getScale() const{
		return mSceneActor->getTransform().getScale();
	}

	virtual const glm::quat& getRotation() const{
		return mSceneActor->getTransform().getQuat();
	}

	// </AMoveable>

	virtual void deserialize(const LuaPlus::LuaObject& src){
		glm::quat rot;
		Lua::luaConv(src.Get("rot"), rot);
		mSceneActor->getTransform().setRotation(rot);

		glm::vec3 pos;
		Lua::luaConv(src.Get("pos"), pos);
		mSceneActor->getTransform().setPosition(pos);

		glm::vec3 scale;
		if(!Lua::luaConv(src.Get("scale"), scale)){
			// default
			scale = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		mSceneActor->getTransform().setScale(scale);

		const char* model = src.Get("model").ToString();

		const char* skin = src.Get("skin").ToString();

		mSceneActor->setModel(
			getAssets()->getModelManager()->getFromPath(model), skin);
	
	}
}; // </Doodad>

}; // </wt>


#endif // </WT_DOODAD_H>