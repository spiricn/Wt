#include "wt/stdafx.h"

#include "wt/ParticleEffectResource.h"
#include "wt/ParticleLayerResource.h"
#include "wt/Lua.h"

namespace wt{

ParticleEffectResource::ParticleEffectResource(AResourceManager<ParticleEffectResource>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name){
}

void ParticleEffectResource::serialize(lua::State* luaState, LuaPlus::LuaObject& effectTable){
	AResource::serialize(luaState, effectTable);

	LuaObject layerTable = luaState->newTable();
	effectTable.Set("layers", layerTable);

	for(LayerMap::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		
		// TODO change name
		const ParticleLayerResource::LayerDesc& mDesc = iter->second->getDesc();

		LuaObject dst = luaState->newTable();
		layerTable.Set(iter->first.c_str(), dst);

		// Local velocity
		LuaObject velLocal = luaState->newTable();

		lua::luaConv(mDesc.localVelocity, velLocal);
		dst.Set("vel.local", velLocal);

		// Random velocity
		LuaObject velRnd = luaState->newTable();

		lua::luaConv(mDesc.randomVelocity, velRnd);
		dst.Set("vel.rnd", velRnd);

		// emissionVol
		LuaObject emissionVol = luaState->newTable();

		lua::luaConv(mDesc.emissionVolume, emissionVol);
		dst.Set("emissionVol", emissionVol);


		dst.Set("life.min", mDesc.minLife);
		dst.Set("life.max", mDesc.maxLife);

		dst.Set("size.min", mDesc.minSize);
		dst.Set("size.max", mDesc.maxSize);
		dst.Set("size.grow", mDesc.sizeGrow);

		dst.Set("emissionRate", mDesc.emissionRate);

		dst.Set("particleNum", mDesc.particleNumber);

		dst.Set("worldSpaceSim", mDesc.simulateInWorldSpace);

		if(mDesc.texture){
			dst.Set("texture", mDesc.texture->getPath().c_str());
		}
		else{
			dst.Set("texture", 0);
		}

		LuaObject colorAni = luaState->newTable();

		for(uint32_t i=0; i<mDesc.kMAX_COLORS; i++){
			LuaObject color = luaState->newTable();

			lua::luaConv(mDesc.colorAnimation[i], color);

			colorAni.Set(i+1, color);
		}

		dst.Set("color.anim", colorAni);
	}
}

ParticleLayerResource* ParticleEffectResource::createLayer(const String& name, const ParticleLayerResource::LayerDesc& desc){
	WT_ASSERT(getLayer(name) == NULL, "Layer named \"%s\" already exists for particle effect");

	ParticleLayerResource* layer = new ParticleLayerResource(this, name);

	layer->getDesc() = desc;

	mLayers.insert(std::make_pair(name, layer));

	return layer;
	
}

ParticleLayerResource* ParticleEffectResource::createLayer(const String& name, AResourceSystem* assets, const LuaPlus::LuaObject& src){
	WT_ASSERT(getLayer(name) == NULL, "Layer named \"%s\" already exists for particle effect");

	ParticleLayerResource* layer = new ParticleLayerResource(this, name);

	
	ParticleLayerResource::LayerDesc desc;

	lua::luaConv(src.Get("vel.local"), desc.localVelocity);
	lua::luaConv(src.Get("vel.rnd"), desc.randomVelocity);
	lua::luaConv(src.Get("emissionVol"), desc.emissionVolume);
	lua::luaConv(src.Get("life.min"), desc.minLife);
	lua::luaConv(src.Get("life.max"), desc.maxLife);
	lua::luaConv(src.Get("size.min"), desc.minSize);
	lua::luaConv(src.Get("size.max"), desc.maxSize);
	lua::luaConv(src.Get("size.grow"), desc.sizeGrow);
	lua::luaConv(src.Get("emissionRate"), desc.emissionRate);
	lua::luaConv(src.Get("particleNum"), desc.particleNumber);
	lua::luaConv(src.Get("worldSpaceSim"), desc.simulateInWorldSpace);

	String texPath;
	LuaObject luaTexPath = src.Get("texture");
	if(luaTexPath.IsString()){
		lua::luaConv(luaTexPath, texPath);
		desc.texture = assets->getTextureManager()->getFromPath(texPath);
	}

	if(!desc.texture){
		LOGW("Texture not found for particle layer");
	}

	
	for(uint32_t i=0; i<ParticleLayerResource::LayerDesc::kMAX_COLORS; i++){
		LuaObject& colorAnim = src.Get("color.anim");

		lua::luaConv(colorAnim.Get(i+1), desc.colorAnimation[i]);
	}

	layer->getDesc() = desc;

	mLayers.insert(std::make_pair(name, layer));

	return layer;
}

ParticleLayerResource* ParticleEffectResource::getLayer(const String& name){
	LayerMap::iterator iter =  mLayers.find(name);
	return iter == mLayers.end() ? NULL : iter->second;
}

void ParticleEffectResource::deleteLayer(const String& name){
	ParticleLayerResource* layer = getLayer(name) ;
	WT_ASSERT(layer != NULL, "No layer named \"%s\"", name.c_str());

	deleteLayer(layer);
}

void ParticleEffectResource::deleteLayer(ParticleLayerResource* layer){
	mLayers.erase(layer->getName());
	delete layer;
}

ParticleEffectResource::LayerMap& ParticleEffectResource::getLayerMap(){
	return mLayers;
}

void ParticleEffectResource::deserialize(lua::State* luaState, const LuaPlus::LuaObject& src){
	AResource::deserialize(luaState, src);

	for(LuaTableIterator iter(src.Get("layers")); iter; iter.Next()){
		// TODO checks
		LuaObject& layerTable = iter.GetValue();

		const char* layerName = iter.GetKey().ToString();
		
		createLayer(layerName, getManager()->getResourceSystem(), layerTable);
	}
}

}; // </wt>