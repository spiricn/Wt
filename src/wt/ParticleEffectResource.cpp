#include "wt/stdafx.h"

#include "wt/ParticleEffectResource.h"
#include "wt/ParticleLayerResource.h"

namespace wt{

ParticleEffectResource::ParticleEffectResource(AResourceManager<ParticleEffectResource>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name){
}

void ParticleEffectResource::serialize(LuaPlus::LuaObject& effectTable){
	AResource::serialize(effectTable);

	LuaObject layerTable;
	LUA_NEW_TABLE(layerTable);
	effectTable.Set("layers", layerTable);

	for(LayerMap::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		
		// TODO change name
		const ParticleLayerResource::LayerDesc& mDesc = iter->second->getDesc();

		LuaObject dst;
		LUA_NEW_TABLE(dst);
		layerTable.Set(iter->first.c_str(), dst);

		// Local velocity
		LuaObject velLocal;
		LUA_NEW_TABLE(velLocal);

		Lua::luaConv(mDesc.localVelocity, velLocal);
		dst.Set("vel.local", velLocal);

		// Random velocity
		LuaObject velRnd;
		LUA_NEW_TABLE(velRnd);

		Lua::luaConv(mDesc.randomVelocity, velRnd);
		dst.Set("vel.rnd", velRnd);

		// emissionVol
		LuaObject emissionVol;
		LUA_NEW_TABLE(emissionVol);

		Lua::luaConv(mDesc.emissionVolume, emissionVol);
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

		LuaObject colorAni;
		LUA_NEW_TABLE(colorAni);

		for(uint32_t i=0; i<mDesc.kMAX_COLORS; i++){
			LuaObject color;
			LUA_NEW_TABLE(color);

			Lua::luaConv(mDesc.colorAnimation[i], color);

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

	Lua::luaConv(src.Get("vel.local"), desc.localVelocity);
	Lua::luaConv(src.Get("vel.rnd"), desc.randomVelocity);
	Lua::luaConv(src.Get("emissionVol"), desc.emissionVolume);
	Lua::luaConv(src.Get("life.min"), desc.minLife);
	Lua::luaConv(src.Get("life.max"), desc.maxLife);
	Lua::luaConv(src.Get("size.min"), desc.minSize);
	Lua::luaConv(src.Get("size.max"), desc.maxSize);
	Lua::luaConv(src.Get("size.grow"), desc.sizeGrow);
	Lua::luaConv(src.Get("emissionRate"), desc.emissionRate);
	Lua::luaConv(src.Get("particleNum"), desc.particleNumber);
	Lua::luaConv(src.Get("worldSpaceSim"), desc.simulateInWorldSpace);

	String texPath;
	LuaObject luaTexPath = src.Get("texture");
	if(luaTexPath.IsString()){
		Lua::luaConv(luaTexPath, texPath);
		desc.texture = assets->getTextureManager()->getFromPath(texPath);
	}

	if(!desc.texture){
		LOGW("Texture not found for particle layer");
	}

	
	for(uint32_t i=0; i<ParticleLayerResource::LayerDesc::kMAX_COLORS; i++){
		LuaObject& colorAnim = src.Get("color.anim");

		Lua::luaConv(colorAnim.Get(i+1), desc.colorAnimation[i]);
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

void ParticleEffectResource::deserialize(const LuaPlus::LuaObject& src){
	for(LuaTableIterator iter(src.Get("layers")); iter; iter.Next()){
		// TODO checks
		LuaObject& layerTable = iter.GetValue();

		const char* layerName = iter.GetKey().ToString();
		
		createLayer(layerName, getManager()->getResourceSystem(), layerTable);
	}
}

}; // </wt>