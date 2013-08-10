#include "wt/stdafx.h"

#include "wt/SceneLoader.h"

namespace wt{


SceneLoader::SceneLoader(Scene* scene, AResourceSystem* assets) : mScene(scene), mAssets(assets){
}

void SceneLoader::load(AIOStream& stream){
	LuaStateOwner state;

	try{
		lua::doStream(state, stream);
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
		if(!lua::luaConv(actorDesc.Get("type"), type)){
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
		else if(!type.compare("particle")){
			ParticleEffect* effect = mScene->createParticleEffect();

			effect->deserialize(mAssets, actorDesc, NULL);

			mScene->getPhysics()->createBBox(effect);
		}
		else{
			TRACEW("Invalid actor type (%s), skipping table..", type.c_str());
			continue;
		}

			
	}

	// lights
	LuaObject dirLight = table.Get("directionalLight");
	if(dirLight.IsTable()){
		DirectionalLight light;
		light.deserialize(mAssets->getLuastate(), dirLight);

		mScene->setDirectionalLight(light);
	}

	LuaObject pointLights = table.Get("pointLights");
	if(pointLights.IsTable()){
		for(LuaPlus::LuaTableIterator iter(pointLights); iter; iter.Next()){
			PointLight pointLight;
			pointLight.deserialize(mAssets->getLuastate(), iter.GetValue());
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

		// Source color
		lua::luaConv(godray.Get("src.clr"), params.sourceColor);
		
		// Ray color
		lua::luaConv(godray.Get("rayColor"), params.rayColor);

		// Source position
		lua::luaConv(godray.Get("src.pos"), params.sourcePosition);

		// Enabled
		lua::luaConv(godray.Get("enabled"), params.enabled);

		// Exposure
		lua::luaConv(godray.Get("exposure"), params.exposure);

	 	// Source size
		lua::luaConv(godray.Get("src.size"), params.sourceSize);

		// Decay
		lua::luaConv(godray.Get("decay"), params.decay);

		// Density
		lua::luaConv(godray.Get("density"), params.density);

		// Weight
		lua::luaConv(godray.Get("weight"), params.weight);

		// Sample number
		lua::luaConv(godray.Get("sampleNumber"), params.sampleNumber);

		// Texture
		String texPath;
		lua::luaConv(godray.Get("src.texture"), texPath);
		params.sourceTexture = mAssets->getTextureManager()->getFromPath(texPath);

		mScene->setGodRayParams(params);
	}

	// Camera
	LuaPlus::LuaObject& cameraTable = table.Get("camera");
	if(cameraTable.IsTable()){
		// Position
		glm::vec3 pos;
		lua::luaConv(cameraTable.Get("pos"), pos);
		mScene->getCamera().setPosition(pos);

		// Rotation
		glm::quat rot;
		lua::luaConv(cameraTable.Get("rot"), rot);
		mScene->getCamera().setRotation(rot);
	}
}

void SceneLoader::save(AIOStream& stream){
	LuaObject sceneTable = mAssets->getLuastate()->newTable();

	// Actor serialization
	LuaPlus::LuaObject actors = mAssets->getLuastate()->newTable();
	sceneTable.Set("ACTORS", actors);

	uint32_t actorCount = 0;
	for(Scene::ActorMap::iterator iter=mScene->getActorMap().begin(); iter!=mScene->getActorMap().end(); iter++){
		LuaPlus::LuaObject actorTable = mAssets->getLuastate()->newTable();

		iter->second->serialize(mAssets, actorTable);

		actors.Set(++actorCount, actorTable);
	}

	// Skybox
	if(mScene->getSkyBox()){
		sceneTable.Set("skybox", mScene->getSkyBox()->getPath().c_str());
	}


	// Lighting
	LuaObject dirLight = mAssets->getLuastate()->newTable();

	DirectionalLight light;
	mScene->getDirectionalLight(light);

	light.serialize(mAssets->getLuastate(), dirLight );

	sceneTable.Set("directionalLight", dirLight);


	// Godrays
	{
		// Params
		Scene::GodRayParams params;
		mScene->getGodRayParams(params);
		LuaObject luaGodrays = mAssets->getLuastate()->newTable();

		// Ray color
		LuaObject luaRayColor = mAssets->getLuastate()->newTable();

		lua::luaConv(params.rayColor, luaRayColor);
		luaGodrays.Set("rayColor", luaRayColor);

		// Source color
		LuaObject luaSourceColor = mAssets->getLuastate()->newTable();

		lua::luaConv(params.sourceColor, luaSourceColor);
		luaGodrays.Set("src.clr", luaSourceColor);

		// Source position
		LuaObject luaSourcePosition = mAssets->getLuastate()->newTable();

		lua::luaConv(params.sourcePosition, luaSourcePosition);
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
		lua::LuaObject cameraTable = mAssets->getLuastate()->newTable();

		// Rotation
		lua::LuaObject luaRot = mAssets->getLuastate()->newTable();
		glm::quat rot;
		mScene->getCamera().getRotation(rot);
		lua::luaConv(rot, luaRot);
		cameraTable.Set("rot", luaRot);

		// Position
		lua::LuaObject pos = mAssets->getLuastate()->newTable();
		lua::luaConv(mScene->getCamera().getPosition(), pos);
		cameraTable.Set("pos", pos);

		sceneTable.Set("camera", cameraTable);
	}

	stream.print("SCENE = \n");
	lua::serializeTable(sceneTable, stream);
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