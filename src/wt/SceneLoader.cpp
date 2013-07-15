#include "wt/stdafx.h"

#include "wt/SceneLoader.h"

namespace wt{


SceneLoader::SceneLoader(Scene* scene, AResourceSystem* assets) : mScene(scene), mAssets(assets){
}

void SceneLoader::load(AIOStream& stream){
	LuaStateOwner state;

	try{
		Lua::doStream(state, stream);
	}catch(...){
		WT_THROW("Error loading scene - error in executing script from stream");
	}


	LuaObject table = state->GetGlobal("SCENE");
	if(!table.IsTable()){
		WT_THROW("Invalid scene description table (\"SCENE\" table missing)");
	}

	for(LuaTableIterator iter(table.Get("ACTORS")); iter; iter.Next()){
		LuaObject& actorDesc = iter.GetValue();

		String type;
		if(!Lua::luaConv(actorDesc.Get("type"), type)){
			TRACEE("Invalid actor table (type field missing)");
			continue;
		}

		ASceneActor* actor = NULL;
			
		if(!type.compare("modelled")){
			actor = mScene->createModelledActor();

			ModelledActor::DeserializationData deserData;
			actor->deserialize(mAssets, actorDesc, &deserData);

			if(deserData.phyiscs){
				mScene->getPhysics()->createActor(actor, deserData.pxDesc);
			}
		}
		else if(!type.compare("terrain")){
			// Terrain
			Terrain* terrain = mScene->createTerrain();

			terrain->deserialize(mAssets, actorDesc);
		}
		else{
			TRACEE("Invalid actor type (%s)", type);
			continue;
		}

			
	}

		
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
}

void SceneLoader::save(AIOStream& stream){
	LuaObject sceneTable;
	LUA_NEW_TABLE(sceneTable);

	// Actor serialization
	LuaPlus::LuaObject actors;
	LUA_NEW_TABLE(actors);
	sceneTable.Set("ACTORS", actors);

	uint32_t actorCount = 0;
	for(Scene::ActorMap::iterator iter=mScene->getActorMap().begin(); iter!=mScene->getActorMap().end(); iter++){
		LuaPlus::LuaObject actorTable;
		LUA_NEW_TABLE(actorTable);

		iter->second->serialize(mAssets, actorTable);

		actors.Set(++actorCount, actorTable);
	}

	// Skybox
	if(mScene->getSkyBox()){
		sceneTable.Set("skybox", mScene->getSkyBox()->getPath().c_str());
	}


	// Lighting

	LuaObject dirLight;
	LUA_NEW_TABLE(dirLight);

	mScene->getDirectionalLight().serialize( dirLight );

	sceneTable.Set("directionalLight", dirLight);


	//// Terrain
	//if(mScene->getTerrain()){
	//	LuaObject terrain;
	//	LUA_NEW_TABLE(terrain);

	//	sceneTable.Set("terrain", terrain);

	//	sceneTable.Set("vertsPerChunk", mScene->getTerrain()->getDesc().numRows);
	//	sceneTable.Set("size", 1);
	//	sceneTable.Set("heightScale", mScene->getTerrain()->getDesc().heightScale);
	//	sceneTable.Set("rowScale", mScene->getTerrain()->getDesc().rowScale);
	//	sceneTable.Set("columnScale", mScene->getTerrain()->getDesc().columnScale);
	//	sceneTable.Set("chunks", mScene->getTerrain()->getDesc().heightmapPath.c_str());
	//	sceneTable.Set("map", mScene->getTerrain()->getDesc().textureMap->getPath().c_str());
	//	sceneTable.Set("tex1", mScene->getTerrain()->getDesc().texture1->getPath().c_str());
	//	sceneTable.Set("tex2", mScene->getTerrain()->getDesc().texture2->getPath().c_str());
	//	sceneTable.Set("tex3", mScene->getTerrain()->getDesc().texture3->getPath().c_str());
	//}


	stream.print("SCENE = \n");
	Lua::serializeTable(sceneTable, stream);
	stream.print("\n");
}

void SceneLoader::load(const String& uri){
	Sp<AIOStream> stream = mAssets->getFileSystem()->open(uri, AIOStream::eMODE_READ);

	try{
		load(*stream.get());
	}catch(...){
		TRACEE("Error loading scene from uri \"%s\"", uri.c_str());
		throw;
	}
}

void SceneLoader::save(const String& uri){
	Sp<AIOStream> stream = mAssets->getFileSystem()->open(uri, AIOStream::eMODE_WRITE);

	try{
		save(*stream.get());
	}catch(...){
		TRACEE("Error saving scene to uri \"%s\"", uri.c_str());
		throw;
	}
}

}; // </wt>