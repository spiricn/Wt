#include "wt/stdafx.h"
#include "wt/SkyDomeRenderer.h"
#include "wt/Scene.h"
#include "wt/SkyDome.h"
#define TD_TRACE_TAG "SkyDomeRenderer"

namespace wt
{

SkyDomeRenderer::SkyDomeRenderer(){
}

SkyDomeRenderer::~SkyDomeRenderer(){
}

void SkyDomeRenderer::create(){
}

void SkyDomeRenderer::render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap){
	const Scene::ActorSet& actorSet = scene->getActorSet(ASceneActor::eTYPE_CUSTOM);

	if(actorSet.empty()){
		return;
	}

	SkyDome* skydome = const_cast<SkyDome*>( dynamic_cast<const SkyDome*>(*actorSet.begin()) );

	skydome->render(scene->getCamera());
}

bool SkyDomeRenderer::isDeferred() const{
	return false;
}
	
} // </wt>

