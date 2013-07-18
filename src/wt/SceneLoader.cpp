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
			TRACEW("Invalid actor table (type field missing), skipping table..");
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

			mScene->getPhysics()->createBBox(actor);
		}
		else if(!type.compare("terrain")){
			// Terrain
			Terrain* terrain = mScene->createTerrain();

			terrain->deserialize(mAssets, actorDesc);

			PhysicsActor::Desc desc;
			terrain->getPhysicsDesc(desc);

			mScene->getPhysics()->createActor(terrain, desc);
		}
		else{
			TRACEW("Invalid actor type (%s), skipping table..", type.c_str());
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

	// Godrays
	if(table.Get("godrays").IsTable()){
		Scene::GodRayParams params;
		mScene->getGodRayParams(params);

		LuaObject& godray = table.Get("godrays");

		// Source 
		Lua::luaConv(godray.Get("src.clr"), params.sourceColor);

		// Source position
		Lua::luaConv(godray.Get("src.pos"), params.sourcePosition);

		// Enabled
		Lua::luaConv(godray.Get("enabled"), params.enabled);

		// Exposure
		Lua::luaConv(godray.Get("exposure"), params.exposure);

	 	// Source size
		Lua::luaConv(godray.Get("src.size"), params.sourceSize);

		// Decay
		Lua::luaConv(godray.Get("decay"), params.decay);

		// Density
		Lua::luaConv(godray.Get("density"), params.density);

		// Weight
		Lua::luaConv(godray.Get("weight"), params.weight);

		// Sample number
		Lua::luaConv(godray.Get("sampleNumber"), params.sampleNumber);

		// Texture
		String texPath;
		Lua::luaConv(godray.Get("src.texture"), texPath);
		params.sourceTexture = mAssets->getTextureManager()->getFromPath(texPath);

		mScene->setGodRayParams(params);
	}

	// Camera
	LuaPlus::LuaObject& cameraTable = table.Get("camera");
	if(cameraTable.IsTable()){
		// Position
		glm::vec3 pos;
		Lua::luaConv(cameraTable.Get("pos"), pos);
		mScene->getCamera().setPosition(pos);

		// Rotation
		glm::quat rot;
		Lua::luaConv(cameraTable.Get("rot"), rot);
		mScene->getCamera().setRotation(rot);
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


	// Godrays
	{
		// Params
		Scene::GodRayParams params;
		mScene->getGodRayParams(params);
		LuaObject luaGodrays;
		LUA_NEW_TABLE(luaGodrays);

		// Ray color
		LuaObject luaRayColor;
		LUA_NEW_TABLE(luaRayColor);

		Lua::luaConv(params.rayColor, luaRayColor);
		luaGodrays.Set("rayColor", luaRayColor);

		// Source color
		LuaObject luaSourceColor;
		LUA_NEW_TABLE(luaSourceColor);

		Lua::luaConv(params.sourceColor, luaSourceColor);
		luaGodrays.Set("src.clr", luaSourceColor);

		// Source position
		LuaObject luaSourcePosition;
		LUA_NEW_TABLE(luaSourcePosition);

		Lua::luaConv(params.sourcePosition, luaSourcePosition);
		luaGodrays.Set("src.pos", luaSourcePosition);

		// Enabled
		luaGodrays.Set("enabled", params.enabled);

		// Exposure
		luaGodrays.Set("exposure", params.exposure);

	 	// Source size
		luaGodrays.Set("src.size", params.sourceSize);

		// Decay
		luaGodrays.Set("decay", params.decay);

		// Density
		luaGodrays.Set("density", params.density);

		// Weight
		luaGodrays.Set("weight", params.weight);

		// Sample number
		luaGodrays.Set("sampleNumber", params.sampleNumber);

		// Texture
		if(params.sourceTexture){
			luaGodrays.Set("src.texture", params.sourceTexture->getPath().c_str());
		}
		else{
			luaGodrays.Set("src.texture", 0);
		}

		sceneTable.Set("godrays", luaGodrays);
	}

	// Camera
	{
		Lua::LuaObject cameraTable;
		LUA_NEW_TABLE(cameraTable);

		// Rotation
		Lua::LuaObject luaRot;
		LUA_NEW_TABLE(luaRot);
		glm::quat rot;
		mScene->getCamera().getRotation(rot);
		Lua::luaConv(rot, luaRot);
		cameraTable.Set("rot", luaRot);

		// Position
		Lua::LuaObject pos;
		LUA_NEW_TABLE(pos);
		Lua::luaConv(mScene->getCamera().getPosition(), pos);
		cameraTable.Set("pos", pos);

		sceneTable.Set("camera", cameraTable);
	}

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