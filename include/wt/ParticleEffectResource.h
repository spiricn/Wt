#ifndef WT_PARTICLEEFFECTRESOURCE_H
#define WT_PARTICLEEFFECTRESOURCE_H


#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/ParticleLayerResource.h"

namespace wt{

class ParticleEffectResource : public AResource<ParticleEffectResource>{
public:
	ParticleEffectResource(AResourceManager<ParticleEffectResource>* manager=NULL, ResourceHandle handle=0, const String& name="");

	void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

	void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table);
	
	ParticleLayerResource* createLayer(const String& name, const ParticleLayerResource::LayerDesc& desc);

	ParticleLayerResource* createLayer(const String& name, AResourceSystem* assets, const LuaPlus::LuaObject& src);

	void deleteLayer(const String& name);

	void deleteLayer(ParticleLayerResource* layer);

	ParticleLayerResource* getLayer(const String& name);

	typedef std::map<String, ParticleLayerResource*> LayerMap;

	LayerMap& getLayerMap();

private:
	LayerMap mLayers;

}; // </ParticleEffectResource>


class ParticleEffectLoader : public AResourceLoader<ParticleEffectResource>{
public:
	void load(AIOStream* /*stream*/, ParticleEffectResource* /*tex*/){
	}

	void save(AIOStream* /*stream*/, Texture2D* /*src*/){
	}

	void create(Texture2D* /*tex*/){
	}
}; // </ParticleEffectLoader>

} // </wt>

#endif // </WT_PARTICLEEFFECTRESOURCE_H>