#ifndef WTG_ENTITY_H
#define WTG_ENTITY_H

#include "wt/SceneActor.h"
#include "wt/Event.h"
#include "wt/game/AMoveable.h"
#include "wt/Scene.h"

namespace wt{

class GameActor;


/*

actor types:
	Doodad
		| collidable
		| non-collidable

	Creature
		- AI controlled NPC
		- all creatures defined by CreatureDesc
		- can have a custom AI or a built-in default one





*/

class AGameActor : public AMoveable, public Lua::ASerializable{
public:
    typedef HashedString Handle;

protected:
	Assets* mAssets;
    Handle mHandle;
    bool mIsAlive;
	SceneActor* mSceneActor;

	Assets* getAssets(){
		return mAssets;
	}

public:
    AGameActor(SceneActor* actor, const String& name, Assets* assets) : mSceneActor(actor), mHandle(name), mIsAlive(true), mAssets(assets){
    }

	virtual ~AGameActor(){
	}

	virtual void serialize(LuaPlus::LuaObject& dst){
	}

	virtual void deserialize(const LuaPlus::LuaObject& src){
	}

    void killEntity(AGameActor* killer){
        mIsAlive = false;
    }

    bool isAlive() const{
        return mIsAlive;
    }

    SceneActor* getActor(){
        return mSceneActor;
    }

	SceneActor* geScenetActor(){
		return mSceneActor;
	}

	virtual const glm::vec3& getScale() const{
		static glm::vec3 tmp(1, 1, 1);
		// TODO
		return tmp;
	}

	uint32_t getId() const{
		return mHandle.getHashCode();
	}

	const String& getName() const{
		return mHandle.getString();
	}

	const HashedString& getHandle() const{
		return mHandle;
	}

	virtual void move(const glm::vec3& delta, float dt=1.0f){
		if(mSceneActor->getPhysicsActor()->isControlled()){
			physx::PxVec3 pxDisp;
			pxConvert(delta, pxDisp);

			mSceneActor->getPhysicsActor()->getController()->move(
				pxDisp, 0.0000001, dt, PxControllerFilters()
				);
		}
		else{
			mSceneActor->getTransform().translate(delta);
		}
	}

	virtual void setScale(const glm::vec3& scale){
	}

	virtual void setPosition(const glm::vec3& position){
		if(mSceneActor->getPhysicsActor()->isControlled()){
			physx::PxExtendedVec3 pxPos;
			pxConvert(position, pxPos);
			mSceneActor->getPhysicsActor()->getController()->setFootPosition(pxPos);
		}
		else{
			mSceneActor->getTransform().setPosition(position);
		}
	}

    virtual void takeDamage(AGameActor* source, float damage){
    }

    virtual void onUpdate(float dt){
    }

    float getDistance(AGameActor* other) const{
            return glm::length(mSceneActor->getTransform().getPosition() - other->getActor()->getTransform().getPosition());
    }

    float getDistance(const glm::vec3& point) const{
            return glm::length(mSceneActor->getTransform().getPosition() - point);
    }
};



}; // </wt>

#endif