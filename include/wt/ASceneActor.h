#ifndef WT_ASCENEACTOR_H
#define WT_ASCENEACTOR_H


#include "wt/stdafx.h"


#include "wt/Transform.h"
#include "wt/Model.h"
#include "wt/AResourceSystem.h"

namespace wt{

class PhysicsActor;
class Scene;

class ASceneActor{
public:
	enum ActorType{
		eTYPE_TERRAIN,
		eTYPE_PARTICLE_EFFECT,
		eTYPE_MODELLED
	};

protected:
	/** Owner of the actor (scene which created it) */
	Scene* mParent;

	/** Name of the actor, might not be set and is not a unique identifier */
	String mName;
	
	/** Unique actor identification generated by the Scene */
	uint32_t mId;

	/** Indicates wheter or not he user data has been set by the user */
	bool mUserDataSet;

	/** Data set by the user */
	void* mUserData;

	/** Type of the actor, used for RTTI */
	ActorType mType;

	math::Transform mTransform;

	/** Scene actor may be paired with a physics actor for its physicall representation */
	PhysicsActor* mPhysicsActor;

public:
	ASceneActor(Scene* parent, ActorType type, uint32_t id, const String& name="") : mName(name), mId(id),
		mUserData(NULL), mUserDataSet(false), mParent(parent), mPhysicsActor(NULL), mType(type){
	}

	ActorType getActorType() const{
		return mType;
	}

	void* getUserData() const{
		return mUserData;
	}

	void setUserData(void* data){
		mUserDataSet = true;
		mUserData = data;
	}

	PhysicsActor* getPhysicsActor(){
		return mPhysicsActor;
	}

	void setPhysicsActor(PhysicsActor* actor){
		mPhysicsActor = actor;
	}
	bool hasUserData() const{
		return mUserDataSet;
	}

	virtual ~ASceneActor(){
	}

	uint32_t getId() const{
		return mId;
	}	

	const String& getName() const{
		return mName;
	}

	math::Transform& getTransform(){
		return mTransform;
	}

	const math::Transform& getTransform() const{
		return mTransform;
	}

	virtual void update(float /*dt*/){
	}

	/*********************************/
	/*********** Serialization *******/
	/*********************************/

	virtual void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst){
		Lua::LuaObject tf;
		LUA_NEW_TABLE(tf);

		Lua::luaConv(mTransform, tf);
		dst.Set("transform", tf);

		dst.Set("name", mName.c_str());

		// TODO storing this as an integer might not be a good idea
		dst.Set("type", (int)mType);
	}

	virtual void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src){
		if(!Lua::luaConv(src.Get("transform"), mTransform)){
			WT_THROW("Error deserializing transform");
		}

		if(!Lua::luaConv(src.Get("name"), mName)){
			mName = "";
		}
	}


}; // </ASceneActor>


}; // </wt>

#endif // </WT_ASCENEACTOR_H>