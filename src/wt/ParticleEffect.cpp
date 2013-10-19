#include "wt/stdafx.h"

#include "wt/ParticleEffect.h"

#include "wt/Lua.h"

namespace wt{

ParticleEffect::ParticleEffect(Scene* parent, uint32_t id, const String& name) : ASceneActor(parent, ASceneActor::eTYPE_PARTICLE_EFFECT, id, name),
	mTimeDelta(0.0f), mVisible(true){
}

ParticleEffect::~ParticleEffect(){
	for(LayerMap::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		delete iter->second;
	}

	mLayers.clear();
}

ATransformable* ParticleEffect::getTransformable(){
	return &mTransform;
}

LuaObject ParticleEffect::lua_getTransform(){
	return getLuaState()->box( mTransform );
}

void ParticleEffect::generateMetaTable(){
	expose("getTransform", &ParticleEffect::lua_getTransform);
}

ParticleEffect::LayerMap& ParticleEffect::getLayerMap(){
	return mLayers;
}

ParticleEffectResource* ParticleEffect::getEffectResource() const{
	return mEffectResource;
}

void ParticleEffect::update(float dt){
	mTimeDelta = dt;

	ASceneActor::update(dt);
}

void ParticleEffect::render(){
}

void ParticleEffect::kill(){
	for(LayerMap::iterator iter=mLayers.begin(); iter!=mLayers.end(); iter++){
		// Create layer instances
		iter->second->kill();
	}
}

bool ParticleEffect::isEmitting() const{
	// TODO
	return true;
}

float ParticleEffect::getTimeDelta() const{
	return mTimeDelta;
}

void ParticleEffect::serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque){
	ASceneActor::serialize(assets, dst);

	dst.Set("effect", mEffectResource->getPath().c_str());
}

void ParticleEffect::create(ParticleEffectResource* rsrc){
	for(ParticleEffectResource::LayerMap::iterator iter=rsrc->getLayerMap().begin(); iter!=rsrc->getLayerMap().end(); iter++){
		// Create layer instances
		ParticleLayer* layer = new ParticleLayer(this, iter->first, iter->second);

		mLayers.insert(std::make_pair(iter->first, layer));
	}

	mEffectResource = rsrc;
}

void ParticleEffect::deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque){
	ASceneActor::deserialize(assets, src);

	ParticleEffectResource* rsrc = NULL;

	String effectUri;
	if(lua::luaConv(src.Get("effect"), effectUri)){
		rsrc = assets->getParticleResourceManager()->getFromPath(effectUri);
	}
	
	WT_ASSERT(rsrc, "ParticleEffectResource not found");

	create(rsrc);	
}

}; // </wt>
