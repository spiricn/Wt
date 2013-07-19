#include "wt/stdafx.h"

#include "wt/ParticleEffect.h"

namespace wt{

ParticleEffect::ParticleEffect(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_PARTICLE_EFFECT, id, name),
	mTimeDelta(0.0f){
}

ParticleEffect::~ParticleEffect(){
	for(LayerMap::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		delete iter->second;
	}

	mLayers.clear();
}

ParticleEffect::LayerMap& ParticleEffect::getLayerMap(){
	return mLayers;
}

void ParticleEffect::update(float dt){
	mTimeDelta = dt;

	ASceneActor::update(dt);
}

ParticleLayer* ParticleEffect::createLayer(const String& name, const ParticleLayer::EffectDesc& desc){
	WT_ASSERT(getLayer(name) == NULL, "Layer named \"%s\" already exists for particle effect");

	ParticleLayer* layer = new ParticleLayer(this, name, desc);

	mLayers.insert(std::make_pair(name, layer));

	return layer;
	
}

ParticleLayer* ParticleEffect::createLayer(const String& name, AResourceSystem* assets, const LuaPlus::LuaObject& src){
	WT_ASSERT(getLayer(name) == NULL, "Layer named \"%s\" already exists for particle effect");

	ParticleLayer* layer = new ParticleLayer(this, name, assets, src);

	mLayers.insert(std::make_pair(name, layer));

	return layer;
}

ParticleLayer* ParticleEffect::getLayer(const String& name){
	LayerMap::iterator iter =  mLayers.find(name);
	return iter == mLayers.end() ? NULL : iter->second;
}

void ParticleEffect::deleteLayer(const String& name){
	ParticleLayer* layer = getLayer(name) ;
	WT_ASSERT(layer != NULL, "No layer named \"%s\"", name.c_str());

	deleteLayer(layer);
}

void ParticleEffect::deleteLayer(ParticleLayer* layer){
	mLayers.erase(layer->getName());
	delete layer;
}

void ParticleEffect::render(){
}

float ParticleEffect::getTimeDelta() const{
	return mTimeDelta;
}

void ParticleEffect::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	ASceneActor::serialize(assets, dst);

	LuaObject layersTable;
	LUA_NEW_TABLE(layersTable);

	uint32_t layerIndex = 0;
	for(LayerMap::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		ParticleLayer* layer = iter->second;

		LuaObject layerTable;
		LUA_NEW_TABLE(layerTable);

		layer->serialize(assets, layerTable);
		
		layersTable.Set(iter->first.c_str(), layerTable);
	}

	dst.Set("layers", layersTable);
}

void ParticleEffect::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	ASceneActor::deserialize(assets, src);

	for(LuaTableIterator iter(src.Get("layers")); iter; iter.Next()){
		// TODO checks
		LuaObject& layerTable = iter.GetValue();

		const char* layerName = iter.GetKey().ToString();
		
		createLayer(layerName, assets, layerTable);
	}
}

}; // </wt>
