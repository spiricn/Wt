#ifndef WT_PARTICLERENDERER_H
#define WT_PARTICLERENDERER_H

#include "wt/ARenderer.h"

#include "wt/Scene.h"
#include "wt/ParticleLayer.h"

namespace wt{

class ParticleRenderer : public ARenderer{
public:
	ParticleRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass);

private:
	gl::ShaderProgram mParticleRenderShader;
	ParticleCalculationShader mParticleCalcShader;
	Texture2D mRandomTexture;

}; // </ParticleRenderer>

}; // </wt>

#endif // </WT_PARTICLERENDERER_H>