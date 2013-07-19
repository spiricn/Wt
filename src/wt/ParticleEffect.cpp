#include "wt/stdafx.h"

#include "wt/ParticleEffect.h"

namespace wt{

ParticleEffect::ParticleEffect(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_PARTICLE_EFFECT, id, name),
	mTimeDelta(0.0f){
}


void ParticleEffect::update(float dt){
	mTimeDelta = dt;

	ASceneActor::update(dt);
}

ParticleLayer* ParticleEffect::addLayer(){
	ParticleLayer* layer = new ParticleLayer(this);
	mLayers.push_back(layer);

	return layer;
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
	for(LayerList::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		ParticleLayer* layer = *iter;

		LuaObject layerTable;
		LUA_NEW_TABLE(layerTable);

		layer->serialize(assets, layerTable);
		
		layersTable.Set(layerIndex++, layerTable);
	}

	dst.Set("layers", layersTable);
}

void ParticleEffect::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	ASceneActor::deserialize(assets, src);

	for(LuaTableIterator iter(src.Get("layers")); iter; iter.Next()){
		LuaObject& layerTable = iter.GetValue();

		ParticleLayer* layer = addLayer();
		layer->deserialize(assets, layerTable);
		layer->create(layer->getDesc());
	}
}

}; // </wt>
