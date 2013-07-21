#pragma once

#ifndef WT_PARTICLEEFFECT_H
#define WT_PARTICLEEFFECT_H

#include "wt/Texture2D.h"
#include "wt/GLBatch.h"
#include "wt/GLShaderProgram.h"
#include "wt/Math.h"
#include "wt/ASceneActor.h"
#include "wt/GLTransformFeedback.h"
#include "wt/ParticleLayer.h"
#include "wt/ParticleEffectResource.h"

namespace wt{

class Renderer;

class ParticleEffect : public ASceneActor{
friend class Renderer;

public:
	ParticleEffect(Scene* parent, uint32_t id, const String& name="");

	~ParticleEffect();

	void update(float dt);

	void render();

	float getTimeDelta() const;

	void create(ParticleEffectResource* rsrc);
	
	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque);

private:
	typedef std::map<String, ParticleLayer*> LayerMap;

	LayerMap mLayers;

	float mTimeDelta;

	ParticleEffectResource* mEffectResource;

protected:
	LayerMap& getLayerMap();


}; // </ParticleEffect>




}; // </wt>

#endif // </WT_PARTICLEEFFECT_H>