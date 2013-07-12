#ifndef WT_LEVELLOADER_H
#define WT_LEVELLOADER_H

#include "wt/game/GameActorManager.h"
#include "wt/game/Creature.h"
#include "wt/game/Doodad.h"


namespace wt{

class GameLevel{
private:
	EntityManager* mEntityManager;
	Assets* mAssets;
	Scene* mScene;

public:
	GameLevel(Assets* assets, Scene* scene, EntityManager* entityManager) : mAssets(assets), mScene(scene), mEntityManager(entityManager){
	}

	Assets* getAssets() const{
		return mAssets;
	}

	Scene* getScene() const{
		return mScene;
	}

	EntityManager* getEntityManager() const{
		return mEntityManager;
	}

	void clear(){
		mEntityManager->clear();
	}

	void init(const LuaObject& table){
		WT_ASSERT(table.IsTable(), "Error loading level - invalid level table");

		const LuaObject& assetsTable = table.Get("ASSETS") ;

		if(!assetsTable.IsTable()){
			WT_THROW("Error loading level - missing assets table");
		}

		mAssets->load(assetsTable);

		// lights
		LuaObject dirLight = table.Get("directionalLight");
		if(dirLight.IsTable()){
			mScene->getDirectionalLight().deserialize( dirLight );
		}

		LuaObject pointLights = table.Get("pointLights");
		if(pointLights.IsTable()){
			for(LuaPlus::LuaTableIterator iter(pointLights); iter; iter.Next()){
				PointLight pointLight;
				pointLight.deserialize(iter.GetValue());
				mScene->addPointLight(pointLight);
			}
		}


		// Skybox
		if(table.Get("skybox").IsString()){
			mScene->setSkyBox( mAssets->getSkyBoxManager()->getFromPath(table.Get("skybox").ToString()) );
		}


		// Terrain
		if(table.Get("terrain").IsTable()){
			mScene->createTerrain( table.Get("terrain") );
		}

		// Actors
		for(LuaTableIterator i(table.Get("ACTORS")); i; i.Next()){
			const char* name = i.GetKey().ToString();
			LuaObject table = i.GetValue();
			const char* type = table.Get("type").ToString();

#if 0
			if(strcmp(type, "CREATURE")==0){
				DynamicActor* sceneActor = TheGame.scene->createDynamicActor(name);

				Creature* entity = new Creature(sceneActor, name, NULL);

				{
					// physics controller
					PhysicsActor::Desc desc;

					desc.type = PhysicsActor::eDYNAMIC_ACTOR;

					desc.controlMode = PhysicsActor::eCONTROLLER_MODE;

					desc.controllerDesc.geometryType = PhysicsActor::eCAPSULE_CONTROLLER;
					desc.controllerDesc.geometryDesc.capsuleController.height = 2.0f;
					desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;

					TheGame.physics->createActor(sceneActor, desc);
				}
	
				mEntityManager->addEntity(name,
					entity);

				sceneActor->setUserData(entity);

				entity->deserialize(table);
			}
#endif
			if(strcmp(type, "DOODAD")==0){
				SceneActor* sceneActor = mScene->createActor(name);

				Doodad* entity = new Doodad(sceneActor, name, mAssets);
				entity->deserialize(table);

				{
					PhysicsActor::Desc desc;

					desc.pose = entity->getActor()->getTransform();

					desc.type = PhysicsActor::eSTATIC_ACTOR;

					desc.controlMode = PhysicsActor::ePHYSICS_MODE;

					desc.geometryType = PhysicsActor::eMESH_GEOMETRY;
					desc.geometryDesc.meshGeometry.model = sceneActor->getModel();

					mScene->getPhysics()->createActor(sceneActor, desc);
				}

				mEntityManager->addEntity(name,
					entity);

				sceneActor->setUserData(entity);
			}
		}
	}

	void init(const String& path){
#if 0
		TheGame.procManager->attach( mEntityManager = new EntityManager());
#endif

		Lua::LuaStateOwner state;

		state->DoFile(path.c_str());

		init( state->GetGlobal("WORLD") );
	}

	Doodad* spawnDoodad(const String& name, const String& model, const String& skin, const glm::vec3& pos){
			SceneActor* sceneActor = mScene->createActor(name);

			sceneActor->setModel(
				mAssets->getModelManager()->getFromPath(model), skin);

			sceneActor->getTransform().setPosition(pos);

			Doodad* entity = new Doodad(sceneActor, sceneActor->getName(), mAssets);

			{
				PhysicsActor::Desc desc;

				desc.pose = entity->getActor()->getTransform();

				desc.type = PhysicsActor::eSTATIC_ACTOR;

				desc.controlMode = PhysicsActor::ePHYSICS_MODE;

				desc.geometryType = PhysicsActor::eMESH_GEOMETRY;
				desc.geometryDesc.meshGeometry.model = sceneActor->getModel();

				mScene->getPhysics()->createActor(sceneActor, desc);
			}

			mEntityManager->addEntity(sceneActor->getName(),
				entity);

			sceneActor->setUserData(entity);

			return entity;
	}

	void serialize(LuaObject& table){

		// Actors
		LuaObject actors;
		LUA_NEW_TABLE(actors);
		mEntityManager->serialize(actors);
		table.Set("ACTORS", actors);


		// Assets
		LuaObject assets;
		LUA_NEW_TABLE(assets);
		mAssets->serialize(assets);
		table.Set("ASSETS", assets);
		
		// Skybox
		if(mScene->getSkyBox()){
			table.Set("skybox", mScene->getSkyBox()->getPath().c_str());
		}

		// Lighting

		LuaObject dirLight;
		LUA_NEW_TABLE(dirLight);

		mScene->getDirectionalLight().serialize( dirLight );

		table.Set("directionalLight", dirLight);


		// Terrain
		if(mScene->getTerrain()){
			LuaObject terrain;
			LUA_NEW_TABLE(terrain);

			table.Set("terrain", terrain);

			terrain.Set("vertsPerChunk", mScene->getTerrain()->getDesc().numRows);
			terrain.Set("size", 1);
			terrain.Set("heightScale", mScene->getTerrain()->getDesc().heightScale);
			terrain.Set("rowScale", mScene->getTerrain()->getDesc().rowScale);
			terrain.Set("columnScale", mScene->getTerrain()->getDesc().columnScale);
			terrain.Set("chunks", mScene->getTerrain()->getDesc().heightmapPath.c_str());
			terrain.Set("map", mScene->getTerrain()->getDesc().textureMap->getPath().c_str());
			terrain.Set("tex1", mScene->getTerrain()->getDesc().texture1->getPath().c_str());
			terrain.Set("tex2", mScene->getTerrain()->getDesc().texture2->getPath().c_str());
			terrain.Set("tex3", mScene->getTerrain()->getDesc().texture3->getPath().c_str());
		}

	}

	void serialize(const String& path){
		LuaObject table;
		LUA_NEW_TABLE(table);

		serialize(table);

		std::ofstream dst(path.c_str());

		dst << "WORLD = ";

		Lua::serializeTable(table, dst);

		dst.close();
	}

#if 0
	Creature* spawnCreature(const String& name, const String& type, const glm::vec3& pos){
		CreatureDesc& desc = *CreatureDescManager::getSingleton().getFromPath(type);

		DynamicActor* sceneActor = TheGame.scene->createDynamicActor(name);

		// TODO
		sceneActor->setModel(ModelManager::getSingleton().getFromPath(desc.modelDesc.model),
			desc.modelDesc.skin);


		sceneActor->getTransform().setPosition(pos);

		Creature* entity = new Creature(sceneActor, name, &desc);

		{
			// physics controller
			PhysicsActor::Desc desc;

			desc.type = PhysicsActor::eDYNAMIC_ACTOR;

			desc.controlMode = PhysicsActor::eCONTROLLER_MODE;

			desc.pose.setPosition(pos);

			desc.controllerDesc.geometryType = PhysicsActor::eCAPSULE_CONTROLLER;
			desc.controllerDesc.geometryDesc.capsuleController.height = 2.0f;
			desc.controllerDesc.geometryDesc.capsuleController.radius = 1.0f;

			TheGame.physics->createActor(sceneActor, desc);
		}
	
		sceneActor->getAnimationPlayer()->play("stand", true);
		mEntityManager->addEntity(name,
			entity);

		sceneActor->setUserData(entity);

		return entity;
	}
#endif

	void addActor(AGameActor* actor){
		mEntityManager->addEntity(actor->getName().c_str(), actor);
	}
}; // </LevelLoader>

}; // </wt>

#endif // </WT_LEVELLOADER_H>