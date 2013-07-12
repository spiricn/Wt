#ifndef WTG_GAMEFRONTEND_H
#define WTG_GAMEFRONTEND_H

#include "wt/stdafx.h"
#include "wt/game/GameBackend.h"
#include "wt/game/GameProc.h"

#include "wt/processmanager.h"
#include "wt/skydome.h"

#include "wt/game/Creature.h"

#include "game/CreatureDesc.h"

#include "wt/RemoteConsole.h"

#include "wt/game/GameActorManager.h"

#include "wt/game/GameLevel.h"


namespace wt{


class Character : public AGameActor{
public:
	Character(SceneActor* actor) : AGameActor(actor, "character"){
	}

	public:
	virtual void setRotation(const glm::quat& quat){
	}

	virtual const glm::vec3& getPosition() const{
		return glm::vec3();
	}

	virtual const glm::quat& getRotation() const{
		return glm::quat();
	}

	virtual void rotate(float x, float y, float z, float angle){
	}
};


class EntityClickedEvent : public Event{
protected:
	void serialize(LuaObject& dst){
		dst.Set("name", entity->getName().c_str());
	}

	void deserialize(LuaObject& src){
	}

public:
	
	static const EvtType TYPE;

	AGameActor* entity;

	EntityClickedEvent(AGameActor* entity) : entity(entity){
	}

	const EvtType& getType() const{
		return TYPE;
	}

}; // </EntityClickedEvent>

const EvtType EntityClickedEvent::TYPE = "EntityClicked";


class GameFrontend : public GameBackend, public ALuaObject{
public:
	
	Character* mCharacter;
	ProcessManager mProcManager;
	SoundStreamPtr mAmbient, mSoundtrack;
	LuaObject creatureMeta;
	RemoteConsole mRemoteConsole;
	GameLevel mLevel;

	GameFrontend(){
	}


	void onInit(LuaPlus::LuaState& state){
		// processes

		
		mProcManager.attach(new CameraCtrlProc);


		// character
		SceneActor* actor = mScene.createDynamicActor();
		actor->getTransform().setPosition(196, 5, 215);
		actor->setModel( TheModelManager.getFromPath("$ROOT/character"), "default");
		//mScene.createCapsuleController(actor, 2, 0.5);


		// controller
		{
			PhysicsActor::Desc desc;

			desc.type = PhysicsActor::eDYNAMIC_ACTOR;

			desc.pose.setPosition( actor->getTransform().getPosition() );

			desc.controlMode = PhysicsActor::eCONTROLLER_MODE;

			desc.controllerDesc.geometryType = PhysicsActor::eCAPSULE_CONTROLLER;
			desc.controllerDesc.geometryDesc.capsuleController.height = 2.0f;
			desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;

			mPhysics->createActor(actor, desc);
		}

		createCreatures();

		// creatures
		mLevel.init("world.lua");

		mLevel.mEntityManager->addEntity("player", mCharacter = new Character(actor));

		



		mFreeFlyMode = false;

		// renderer
		Renderer::GodRayParams p;
		mRenderer.getGodRayParams(p);
		p.mSunPos = glm::vec3(-0.385030, 0.453779, -0.803640)*1000.0f;
		p.mIsEnabled = false;
		p.mSunSize = 30.0f;

		mRenderer.setGodRayParams(p);


		
		plantSomeTrees();


		TheGame.sound->setMuted(true);

		mAmbient = TheGame.sound->createSoundStream("assets/music/forestnormalday.ogg");
		mAmbient->setLoop(true);
		mAmbient->setVolume(0.3);
		mAmbient->play();


		mLuaStateManager->registerObject(this,
			mLuaStateManager->exposeObject(this, "GameFrontendMetaTable"),
			"Game");

		EventManager::getSingleton().registerInternalEvent(
			CreatureAggroEvent::TYPE
			);

		EventManager::getSingleton().registerInternalEvent(
			CreatureCritEvent::TYPE
			);

		TheGame.sound->registerEvents();

		EventManager::getSingleton().registerInternalEvent(
			EntityClickedEvent::TYPE
			);
		

		//
		LuaFunction<void>(mLuaStateManager->getGlobalState(), "onGameInit")();

		
		{
		/*SceneActor* ac = mScene.createStaticActor();
		ac->setModel(ModelManager::getSingleton().find("sphere"), "default");
		ac->getTransform().setScale(10.0, 10, 10);
		ac->getTransform().setPosition(200, 3, 200);

		ac->getModel()->getSkin("default")->getMeshList().begin()->material.setBlend(true);*/

		/*PhysicsActor::Desc d;

		d.type = PhysicsActor::eSTATIC_ACTOR;

		d.geometryDesc.sphereGeometry.radius = 10.0f;
		d.geometryType = PhysicsActor::eSPHERE_GEOMETRY;

		d.position = ac->getTransform().getPosition();
		d.controlMode = PhysicsActor::ePHYSICS_MODE;

		mPhysics->createActor(ac, d);*/
		}

		mRemoteConsole.start();

		{
		/*LuaObject table;
		LUA_NEW_TABLE(table);
		mEntityManager->serialize(table);

		std::ofstream stream("world.lua");
		Lua::serializeTable(table, stream);
		stream.close();*/
		}
	}



	
	void onMouseDown(float x, float y, MouseButton btn){
		if(btn==BTN_MIDDLE){
			spawnCrate(mCamera->getPosition(), true);
		}
		else if(btn==BTN_RIGHT){
			spawnBall(mCamera->getPosition(), true);
		}
		else{

			if(mInput->isMouseGrabbed()){
				Sp<Buffer<PhysicsActor*>> res = mPhysics->getActorsInRegion(mCharacter->getActor()->getTransform().getPosition(), 5);
				if(res){
					for(uint32_t i=0; i<res->getCapacity(); i++){
						void* data;
						if(data = res->operator[](i)->getSceneActor()->getUserData()){
							AGameActor* e = (AGameActor*)data;
							e->takeDamage(mCharacter, 9);
						}
					}
				}
			}
			else{
				RaycastHitEvent res;
				if(mPhysics->pick(*mCamera, mRenderer.getFrustum(),
					glm::vec2(x, y), glm::vec2(mWindow->getWidth(), mWindow->getHeight()), res)){
					void* data = res.mPickedActor->getSceneActor()->getUserData();
					if(data){
						mEventManager->queueEvent(
							new EntityClickedEvent((AGameActor*)data)
							);
					}
				}
			}

			/*ProcPtr p;
			(p=new Animator(mCharacter->getActor(), "attack", false, false))->setNext( new Animator(mCharacter->getActor(), "stand", true) );
			mProcManager.attach(p);

			Creature* target = NULL;
			if(mEntityManager->contains("wolf")){
				target = (Creature*)mEntityManager->findByName("wolf");
			}
			else if(mEntityManager->contains("spider")){
				target = (Creature*)mEntityManager->findByName("spider");
			}

			if(target){
				if(target->getDistance(mCharacter->getActor()->getTransform().getPosition()) <= 5.0f){
					target->takeDamage(mCharacter, 9.0f);
				}
			}*/
		}
	}

	void onKeyUp(VirtualKey code){
		if(!mFreeFlyMode){
			if(code == KEY_w || code == KEY_s){
				mCharacter->getActor()->getAnimationPlayer()->play("stand", true);
			}
		}

		GameBackend::onKeyUp(code);
	}

	void onKeyDown(VirtualKey code){
		if(!mFreeFlyMode){
			if(code == KEY_w){
				mCharacter->getActor()->getAnimationPlayer()->play("run", true);
			}
			else if(code == KEY_s){
				mCharacter->getActor()->getAnimationPlayer()->play("walkbackwards", true);
			}
		}

		GameBackend::onKeyDown(code);
	}

	void render(float dt){
		mRenderer.render(mScene);
	}
	
	void controlCharacter(float dt){
		// update camera control
		if(!mFreeFlyMode){
			{
			glm::vec3 fw = -mCamera->getRightVec();
			fw.y = 0.0f;
			mCharacter->getActor()->getTransform().setFacing(glm::normalize(fw),
				glm::vec3(0, 1, 0));
			}
		

			float fallSpeed = -9.18;
			float moveSpeed = 8.6f;

			glm::vec3 fw = glm::normalize( mCamera->getForwardVec() * glm::vec3(1, 0, 1) )  * moveSpeed;
		
			glm::vec3 disp(0, fallSpeed, 0);

			if(mInput->isKeyDown(KEY_w)){
				disp += fw;
			}
			else if(mInput->isKeyDown(KEY_s)){
				disp -= fw;
			}

			PxVec3 pxDisp;
			pxConvert(disp, pxDisp);

			mCharacter->move(disp*dt);
	
			
		}
	}

	void update(float dt){
		// processes
		mProcManager.upate(dt);

		// update lua state script
		try{
			LuaFunction<void>(mLuaStateManager->getGlobalState(), "onUpdate")(dt);
		}catch(LuaException e){
			WT_EXCEPT(TD_TRACE_TAG, "Error calling update function");
		}

		controlCharacter(dt);

		// animation & physics
		mScene.update(dt);
	
		
		mPhysics->update(dt);
		
		mTpsCam.setTarget(&mCharacter->getActor()->getTransform().getPosition());
		mTpsCam.handle(dt, mInput);

		TheGame.sound->setListenerPosition(mCamera->getPosition());
		TheGame.sound->setListenerForwardVec(mCamera->getForwardVec());
	}


	/* LUA BINDINGS */
	void lua_setTimeMod(float mod){
		mTimeMod = mod;
	}

	uint32_t lua_getEntityHandle(LuaObject name){
		AGameActor* e = mLevel.mEntityManager->findByName(name.ToString());
		if(e==NULL){
			LOGE("No entity named \"%s\"", name.ToString());
			return 0;
		}
		else{
			return e->getId();
		}
	}

	void lua_setCreatureTarget(LuaObject creatureHandle, LuaObject targetHandle){
		Creature* creature = (Creature*)mLevel.mEntityManager->findByName( creatureHandle.ToString() );
		AGameActor* target = mLevel.mEntityManager->findByName( targetHandle.ToString() );

		if(creature && target){
			creature->setTarget(target);
		}
		else{
			LOG("%p %p", creature, target);
		}
	}

	void lua_clearCreatureTargets(LuaObject creatureHandle){
		Creature* creature = (Creature*)mLevel.mEntityManager->findByName( creatureHandle.ToString() );
		if(creature){
			creature->clearTargets();
		}
		else{
			LOG("%p", creature);
		}
	}
	
	void lua_playMusic(LuaObject name){
		if(mSoundtrack){
			mSoundtrack->stop();
		}

		mSoundtrack = TheGame.sound->createSoundStream( name.ToString() );
		mSoundtrack->setLoop(true);
		mSoundtrack->setVolume(0.2);
		mSoundtrack->play();
	}

	void lua_playEntityAnimation(LuaObject handle, LuaObject animation){
		AGameActor* entity = mLevel.mEntityManager->findByName( handle.ToString() );

		entity->getActor()->getAnimationPlayer()->play( animation.ToString(), true);
	}

	LuaObject lua_getEntitiesInRegion(LuaObject pos, LuaObject radius){
		Sp<Buffer<PhysicsActor*>> res = mPhysics->getActorsInRegion(
			Lua::ltableToVec3(pos), radius.ToNumber());

		LuaObject luaRes;
		LUA_NEW_TABLE(luaRes);

		if(res){

			luaRes.Set("number", res->getCapacity());

			int iter=1;
			for(int i=0; i<res->getCapacity(); i++){
				PhysicsActor* actor = (*res)[i];
				LOG("%p", actor);

			/*	void* data;
				if(data = actor->getSceneActor()->getUserData()){
					luaRes.Set(iter++, ((AGameActor*)data)->getName().c_str());
				}*/

				
			}
		}
		else{
			luaRes.Set("number", 0);
		}

		return luaRes;

	}

	void lua_spawnCreature(LuaObject name, LuaObject type, LuaObject position){
		glm::vec2 pos;
		Lua::luaConvert(position, pos);
		float h = mPhysics->getTerrainHeightAt(pos);

		mLevel.spawnCreature(name.ToString(), type.ToString(),
			glm::vec3(pos.x, h+5, pos.y)
			);
	}

	void lua_playSound(LuaObject name){
		mProcManager.attach(
			new SoundPlayer(name.ToString())
			);
	}

	void lua_createRegion(LuaObject name, LuaObject position, LuaObject radius){
		mPhysics->createRegion(name.ToString(),
			Lua::ltableToVec3(position), radius.ToNumber());
	}

	void lua_teleportEntity(LuaObject luaName, LuaObject luaPos){
		AGameActor* entity = mLevel.mEntityManager->findByName( luaName.ToString() );

		if(entity){
			glm::vec3 pos = Lua::ltableToVec3(luaPos);

			entity->setPosition(pos);
		}
	}

	void expose(LuaObject& meta){
		meta.RegisterObjectDirect("setTimeMod", (GameFrontend*)0, &GameFrontend::lua_setTimeMod);
		meta.RegisterObjectDirect("getEntityHandle", (GameFrontend*)0, &GameFrontend::lua_getEntityHandle);
		meta.RegisterObjectDirect("setCreatureTarget", (GameFrontend*)0, &GameFrontend::lua_setCreatureTarget);
		meta.RegisterObjectDirect("clearCreatureTargets", (GameFrontend*)0, &GameFrontend::lua_clearCreatureTargets);
		meta.RegisterObjectDirect("playMusic", (GameFrontend*)0, &GameFrontend::lua_playMusic);
		meta.RegisterObjectDirect("setEntityAnimation", (GameFrontend*)0, &GameFrontend::lua_playEntityAnimation);
		meta.RegisterObjectDirect("spawnCreature", (GameFrontend*)0, &GameFrontend::lua_spawnCreature);
		meta.RegisterObjectDirect("playSound", (GameFrontend*)0, &GameFrontend::lua_playSound);
		meta.RegisterObjectDirect("getEntitiesInRegion", (GameFrontend*)0, &GameFrontend::lua_getEntitiesInRegion);
		meta.RegisterObjectDirect("createRegion", (GameFrontend*)0, &GameFrontend::lua_createRegion);
		meta.RegisterObjectDirect("teleportEntity", (GameFrontend*)0, &GameFrontend::lua_teleportEntity);
	}
	
	void createCreatures(){
		
		wt::AResourceGroup<CreatureDesc>* grp = &CreatureDescManager::getSingleton();

		// timber_wolf

		{
		CreatureDesc& desc = *grp->create("timber_wolf");

		desc.modelDesc.model = "wolf";
		desc.modelDesc.skin = "timber";

		desc.soundDesc.aggro = "wolf/aggro";
		desc.soundDesc.attack = "wolf/attack";
		desc.soundDesc.wound = "wolf/wound";
		desc.soundDesc.death = "wolf/death";
		desc.soundDesc.walk = "wolf/walk";

		desc.statDesc.aggroRange = 20.0f;
		desc.statDesc.attackSpeed = 0.2f;
		desc.statDesc.critChance = 0.3f;
		desc.statDesc.defense = 1.2f;
		desc.statDesc.critModifier = 2.0f;
		desc.statDesc.damage= 25.0f;
		desc.statDesc.maxHitPoints = 100.0f;
		desc.statDesc.moveSpeed = 5.0f;
		}

	
		//// black wolf
		//{
		//desc = *grp->create("black_wolf");
		//desc.modelDesc.skin = "black";
		//}

		//// coyote
		//{
		//desc = *grp->create("coyote");
		//desc.modelDesc.skin = "coyote";
		//}

		//// tarantula
		//desc = *grp->create("tarantula");

		//desc.modelDesc.model = "tarantula";
		//desc.modelDesc.skin = "brown";

		//desc.soundDesc.aggro = "$ROOT/spider/aggro";
		//desc.soundDesc.attack = "$ROOT/spider/attack";
		//desc.soundDesc.wound = "$ROOT/spider/wound";
		//desc.soundDesc.death = "$ROOT/spider/death";
		//desc.soundDesc.walk = "$ROOT/spider/walk";

		//// ghoul
		//desc = *grp->create("ghoul");

		//desc.modelDesc.model = "$ROOT/ghoul";
		//desc.modelDesc.skin = "white";

		//desc.soundDesc.aggro = "$ROOT/ghoul/aggro";
		//desc.soundDesc.attack = "$ROOT/ghoul/attack";
		//desc.soundDesc.wound = "$ROOT/ghoul/wound";
		//desc.soundDesc.death = "$ROOT/ghoul/death";
		//desc.soundDesc.walk = "$ROOT/ghoul/walk";

		//// human
		//desc = *grp->create("human");

		//desc.statDesc = grp->find("timber_wolf")->statDesc;

		//desc.modelDesc.model = "$ROOT/human";
		//desc.modelDesc.skin = "default";

		//desc.soundDesc.aggro = "$ROOT/ghoul/aggro";
		//desc.soundDesc.attack = "$ROOT/ghoul/attack";
		//desc.soundDesc.wound = "$ROOT/ghoul/wound";
		//desc.soundDesc.death = "$ROOT/ghoul/death";
		//desc.soundDesc.walk = "$ROOT/ghoul/walk";
		//desc.statDesc.attackSpeed = 1.5;
		//desc.statDesc.damage = 5.0f;
		//desc.statDesc.maxHitPoints = 300;
	}

}; // </TestFrontend>

} // </Wt>

#endif

