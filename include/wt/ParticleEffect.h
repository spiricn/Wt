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

namespace wt{

class ParticleEffect : public ASceneActor{
public:

	typedef std::vector<ParticleLayer*> LayerList;

	LayerList mLayers;

	ParticleLayer* addLayer();

	ParticleEffect(Scene* parent, uint32_t id, const String& name="");

	void update(float dt);

	void render();

	float getTimeDelta() const;
	
	float mTimeDelta;	

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst, void* opaque=NULL);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src, void* opaque);

}; // </ParticleEffect>




}; // </wt>

#endif // </WT_PARTICLEEFFECT_H>