#ifndef WT_MODELRENDERER_H
#define WT_MODELRENDERER_H

#include "wt/ARenderer.h"

#include "wt/Scene.h"

namespace wt{


class ModelRenderer : public ARenderer{
public:
	ModelRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass);

	gl::ShaderProgram* setupSceneLighting();

private:
	void render(Scene* scene, ModelledActor* actor, math::Camera* camera, PassType pass);

	gl::ShaderProgram mShader;
	Texture2D mInvalidTexture;

}; // </ModelRenderer>

}; // </wt>

#endif // </WT_MODELRENDERER_H>