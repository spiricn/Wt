#ifndef WTG_CREATURE_H
#define WTG_CREATURE_H

#include "wt/stdafx.h"

#include "wt/AProcess.h"
#include <td/td.h>

#include "wt/game/AGameActor.h"
#include "game/CreatureDesc.h"

namespace wt{

#if 0
class Creature;

class CreatureAggroEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.Set("creature", mCreatureHandle.c_str());
		dst.Set("target", mTargetHandle.c_str());
	}

	void deserialize(LuaObject& src){
	}

public:
	AGameActor::Handle mCreatureHandle;
	AGameActor::Handle mTargetHandle;

	CreatureAggroEvent(const AGameActor::Handle& creature, const AGameActor::Handle& target) : mCreatureHandle(creature), 
		mTargetHandle(target){

	}

	static const EvtType TYPE;

	const EvtType& getType() const{
		return TYPE;
	}

}; // </CreatureAggroEvent>


class CreatureCritEvent : public Event{
protected:
	void serialize(LuaObject& dst){
	}

	void deserialize(LuaObject& src){
	}

public:
	static const EvtType TYPE;

	CreatureCritEvent(){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </CreatureCritEvent>



class Creature : public AGameActor{
	DynamicActor* mActor;
	AGameActor* mTarget;

	enum State{
		eMOVING,
		eATTACKING,
		eIDLE,
		eMOVING_HOME,
		eWANDERING,
		eATTACK_MOVE,
		eDEAD
	};

	State mState;

protected:
	float mHealth;

	glm::vec3 mHome;
	
	glm::vec3 mMoveDestination;
	float mWanderRadius;
	float mTimeBeforeWander;
	float mAttackTimer;
	
	typedef std::queue<AGameActor*> AGameActorQueue;
	AGameActorQueue mTargets;

	void changeState(State state){

		if(state != eIDLE && state != eATTACKING){
			mWalkSound->setLoop(true);
			mWalkSound->play();
		}

		if(mState == eATTACKING){
			mAttackSound->stop();
		}

		switch(state){
		case eDEAD:
			mActor->getAnimationPlayer()->play("death", false);
			mDeathSound->play();
			mAggroSound->stop();
			mAttackSound->stop();
			mDamageSound->stop();
			mWalkSound->stop();
			break;

		case eMOVING:
			mActor->getAnimationPlayer()->play("walk", true);
			break;

		case eATTACK_MOVE:
			EventManager::getSingleton().queueEvent(
				new CreatureAggroEvent(getName(), mTarget->getName())
				);

			mAggroSound->play();
			mActor->getAnimationPlayer()->play("run", true);
			break;

		case eMOVING_HOME:
			mActor->getAnimationPlayer()->play("run", true);
			break;

		case eATTACKING:
			mAttackTimer = 0.0f;
			mWalkSound->stop();
			break;

		case eIDLE:
			mWalkSound->stop();
			mActor->getAnimationPlayer()->play("stand", true);
			mTimeBeforeWander = 2.0f + math::random()*2.0f;
			break;
		}

		mState = state;
	}

	void onUpdate(float dt){
		if(!mTarget && !mTargets.empty()){
			mTarget = mTargets.front();
			mTargets.pop();
		}

		mAttackTimer -= dt;
		if(mAttackTimer < 0){
			mAttackTimer = 0.0f;
		}

		switch(mState){

		case eDEAD:
			if(!mActor->getAnimationPlayer()->isPlaying()){
				// remove AGameActor ?
			}
			break;

		case eIDLE:
			if(mTarget && getDistance(mTarget) <= mStats.aggroRange && mTarget->isAlive()){
				changeState(eATTACK_MOVE);
			}
			else{
				if(mTarget && getDistance(mHome) > mWanderRadius){
					changeState(eMOVING_HOME);
				}
				else{
					if(mTimeBeforeWander <= 0.0f){
						mMoveDestination = mHome + glm::vec3(math::random()*(mWanderRadius-1.0f), 0.0f, math::random()*(mWanderRadius-1.0f));

						mMoveDestination.y = TheGame.physics->getTerrainHeightAt(
							glm::vec2(mMoveDestination.x, mMoveDestination.z));

						changeState(eMOVING);
					}
					else{
						mTimeBeforeWander -= dt;
					}
				}
			}
			break;

		case eATTACK_MOVE:
			if(mTarget && getDistance(mTarget) < 5.0f){
				changeState(eATTACKING);
			}
			else if(mTarget && getDistance(mTarget) >= mStats.aggroRange){
				changeState(eIDLE);
			}
			else if(mTarget){
				PxController* ctrl = mActor->getPhysicsActor()->getController();

				glm::vec3 dest = mTarget->getActor()->getTransform().getPosition();
				PxVec3 disp ;
				pxConvert(glm::normalize(dest-mActor->getTransform().getPosition()), disp);

				ctrl->move(disp*mStats.moveSpeed*dt, 0.0000001, dt, PxControllerFilters());

				mActor->getTransform().setFacing(
					glm::cross(glm::normalize(dest-mActor->getTransform().getPosition()), glm::vec3(0, 1, 0)),
					glm::vec3(0, 1, 0));
			}
			else{
				changeState(eIDLE);
			}
			break;

		case eMOVING:
			if(mTarget && getDistance(mTarget) <= mStats.aggroRange && mTarget->isAlive()){
				changeState(eATTACK_MOVE);
			}
			else if(getDistance(mMoveDestination) <= 1.0f){
				changeState(eIDLE);
			}
			else{
				PxController* ctrl = mActor->getPhysicsActor()->getController();

				PxVec3 moveVec;

				pxConvert(glm::normalize(mMoveDestination - mActor->getTransform().getPosition()*glm::vec3(1,0,1)), moveVec);

				ctrl->move(moveVec*mStats.moveSpeed*dt, 0.00001, dt, PxControllerFilters());

				mActor->getTransform().setFacing(
					glm::cross(glm::normalize(mMoveDestination-mActor->getTransform().getPosition()), glm::vec3(0, 1, 0)),
					glm::vec3(0, 1, 0));
			}
			break;

		case eATTACKING:
			if(mTarget && !mTarget->isAlive()){
				mTarget = NULL;
				changeState(eIDLE);
			}
			else if(mTarget && getDistance(mTarget) >= 5.0f){
				changeState(eATTACK_MOVE);
			}
			else if(mTarget){
				if(mAttackTimer <= 0){
					mActor->getAnimationPlayer()->play("attack", false);
					
					mAttackTimer = 1/mStats.attackSpeed;
					mAttackSound->play();

					float damage = mStats.damage;
					if(math::random() <= mStats.critChance){
						damage *= mStats.critModifier;
						EventManager::getSingleton().queueEvent(
							new CreatureCritEvent
							);
					}

					mTarget->takeDamage(this, damage);
					if(!mTarget->isAlive()){
						// killed target
						mTarget = NULL;
						changeState(eIDLE);
						break;
					}
				}
				else{
					if(!mActor->getAnimationPlayer()->isPlaying()){
						mActor->getAnimationPlayer()->play("stand", true);
					}
				}
				mActor->getTransform().setFacing(
					glm::cross(glm::normalize(mTarget->getActor()->getTransform().getPosition()-mActor->getTransform().getPosition()), glm::vec3(0, 1, 0)),
						glm::vec3(0, 1, 0));
			}
			else{
				changeState(eIDLE);
			}
			break;

		case eMOVING_HOME:
			if(mTarget && getDistance(mTarget) <= mStats.aggroRange && mTarget->isAlive()){
				changeState(eIDLE);
			}
			else{
				if(getDistance(mHome)> 4.0f){
					PxController* ctrl = mActor->getPhysicsActor()->getController();

					glm::vec3 dest = mHome;
					PxVec3 disp ;
					pxConvert(glm::normalize(dest-mActor->getTransform().getPosition()), disp);

					ctrl->move(disp*mStats.moveSpeed*dt, 0.00001, dt, PxControllerFilters());

					mActor->getTransform().setFacing(
						glm::cross(glm::normalize(dest-mActor->getTransform().getPosition()), glm::vec3(0, 1, 0)),
						glm::vec3(0, 1, 0));
				}
				else{
					changeState(eIDLE);
				}
			}
			break;
		}

		// gravity
		PxController* ctrl = mActor->getPhysicsActor()->getController();
		ctrl->move(PxVec3(0, -9.18, 0)*dt, 0.00001, dt, PxControllerFilters());

		mAggroSound->setPosition( mActor->getTransform().getPosition() );
		mAttackSound->setPosition( mActor->getTransform().getPosition() );
		mDeathSound->setPosition( mActor->getTransform().getPosition() );
		mDamageSound->setPosition( mActor->getTransform().getPosition() );
		mWalkSound->setPosition( mActor->getTransform().getPosition() );
	}

	SoundPtr mAttackSound;
	SoundPtr mAggroSound;
	SoundPtr mDamageSound;
	SoundPtr mDeathSound;
	SoundPtr mWalkSound;

	CreatureDesc::StatDesc mStats;
	const CreatureDesc* mCreatureDesc;

	void create(){
		const CreatureDesc& desc = *mCreatureDesc;

		mStats = desc.statDesc;
		mHealth = mStats.maxHitPoints;

		mAggroSound = TheGame.sound->createSound( desc.soundDesc.aggro );
		mAttackSound = TheGame.sound->createSound( desc.soundDesc.attack );
		mDeathSound = TheGame.sound->createSound(desc.soundDesc.death );
		mDamageSound = TheGame.sound->createSound(desc.soundDesc.wound );
		mWalkSound = TheGame.sound->createSound(desc.soundDesc.walk );

#if 0
		mActor->setModel(
			ModelManager::getSingleton().getFromPath(mCreatureDesc->modelDesc.model),
			mCreatureDesc->modelDesc.skin);
#endif
	}

public:
	Creature(SceneActor* actor, const String& name, const CreatureDesc* desc) : AGameActor(actor, name),
		mTarget(NULL), mState(eIDLE), mWanderRadius(10.0f), mCreatureDesc(desc){

		mHome = actor->getTransform().getPosition();

		mHome.y = TheGame.physics->getTerrainHeightAt(
			glm::vec2(mHome.x, mHome.z));

		mActor = (DynamicActor*)actor;
		
		if(desc){
			create();
		}

	}

	/* AMoveable */

	virtual void setRotation(const glm::quat& quat){
	}

	virtual void move(const glm::vec3& delta, float dt=1.0f){
	}

	virtual void setPosition(const glm::vec3& position){
		PxExtendedVec3 pxVec;

		pxConvert(position, pxVec);
		mActor->getPhysicsActor()->getController()->setFootPosition(pxVec);
	}

	virtual const glm::vec3& getPosition() const{
		return mSceneActor->getTransform().getPosition();
	}

	virtual const glm::quat& getRotation() const{
		return mSceneActor->getTransform().getQuat();
	}

	virtual void rotate(float x, float y, float z, float angle){
	}

	// </AMoveable>


	void takeDamage(AGameActor* source, float damage){
		AGameActor::takeDamage(source, damage);

		if(mState != eDEAD){
			mHealth -= damage / mStats.defense;

			if(mHealth <= 0.0f){
				changeState(eDEAD);
				killEntity(source);
			}
			else{
				mDamageSound->play();
			}

		}
	}

	void setTarget(AGameActor* AGameActor){
		mTargets.push(AGameActor);
	}

	void expose(LuaObject& meta){
		meta.RegisterObjectDirect("clearTargets",
			(Creature*)0, &Creature::clearTargets);
	}

	void clearTargets(){
		while(!mTargets.empty()){
			mTargets.pop();
		}

		mTarget = NULL;
		changeState(eIDLE);
	}

	virtual void serialize(LuaPlus::LuaObject& dst){
		dst.Set("type", "CREATURE");

		dst.Set("desc", mCreatureDesc->getName().c_str());
		
		// position
		LuaObject luaPos;
		LUA_NEW_TABLE(luaPos);

		Lua::luaConvert(
			mSceneActor->getTransform().getPosition(), luaPos);

		dst.Set("pos", luaPos);

		// orientation
		LuaObject luaRot;
		LUA_NEW_TABLE(luaRot);
		Lua::luaConvert(
			mSceneActor->getTransform().getQuat(), luaRot);
		dst.Set("rot", luaRot);

		// home
		LuaObject luaHome;
		LUA_NEW_TABLE(luaHome);
		Lua::luaConvert(mHome, luaHome);
		dst.Set("home", luaHome);
	}

	virtual void deserialize(const LuaPlus::LuaObject& src){
		glm::vec3 pos;
		Lua::luaConvert(src.Get("pos"), pos);
		setPosition(pos);

		glm::quat rot;
		Lua::luaConvert(src.Get("rot"), rot);
		mSceneActor->getTransform().setRotation(rot);

		const char* desc = src.Get("desc").ToString();

		mCreatureDesc = CreatureDescManager::getSingleton().getFromPath(desc);

		create();

		Lua::luaConvert(src.Get("home"), mHome);

		setPosition(pos);
	}

}; // </Creature>

#endif

} // </wt>

#endif