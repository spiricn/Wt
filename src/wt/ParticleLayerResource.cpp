#include "wt/stdafx.h"

#include "wt/ParticleLayerResource.h"

namespace wt{

ParticleLayerResource::ParticleLayerResource(ParticleEffectResource* parent, const String& name) : mName(name), mParent(parent){
}

ParticleEffectResource* ParticleLayerResource::getParent() const{
	return mParent;
}

void ParticleLayerResource::setName(const String& name){
	mName = name;
}

const String& ParticleLayerResource::getName() const{
	return mName;
}

ParticleLayerResource::LayerDesc& ParticleLayerResource::getDesc(){
	return mDesc;
}

const ParticleLayerResource::LayerDesc& ParticleLayerResource::getDesc() const{
	return mDesc;
}

ParticleLayerResource::LayerDesc::LayerDesc() : 
	localVelocity(0, -3, 0),
	randomVelocity(0, 0, 0),
	emissionVolume(0, 0, 0),
	minLife(4), maxLife(4),
	minSize(10), maxSize(10),
	sizeGrow(0),
	emissionRate(10),
	particleNumber(200),
	texture(NULL){
		colorAnimation[0] = Color(1, 1, 1, 0);
		colorAnimation[1] = Color::white();
		colorAnimation[2] = Color(1, 1, 1, 0.5);
		colorAnimation[3] = Color(1, 1, 1, 0.1);
}


}; // </wt>