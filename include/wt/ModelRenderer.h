#ifndef WT_MODELRENDERER_H
#define WT_MODELRENDERER_H

#include "wt/ARenderer.h"

#include "wt/Scene.h"

namespace wt{

class DeferredRender;

class ModelRenderer : public ARenderer{
public:
	ModelRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap);

	void onSceneLightingChanged(Scene* scene, Renderer*);

	bool isDeferred() const;

private:
	void render(Scene* scene, ModelledActor* actor, math::Camera* camera, PassType pass);

	gl::ShaderProgram mShader;
	Texture2D mInvalidTexture;
	//DeferredRender* mDeferredRender;
	int mSkipper;
}; // </ModelRenderer>

}; // </wt>

#endif // </WT_MODELRENDERER_H>