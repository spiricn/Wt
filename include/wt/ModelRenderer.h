#ifndef WT_MODELRENDERER_H
#define WT_MODELRENDERER_H

#include "wt/ARenderer.h"

#include "wt/Scene.h"
#include "wt/GLShaderProgram.h"

namespace wt
{

class DeferredRender;

class ModelRenderer : public ARenderer{
public:
	ModelRenderer();

	void create();

	void render(Scene* scene, Camera* camera, PassType pass, Texture2D* shadowMap);

	bool isDeferred() const;

private:
	void render(Scene* scene, ModelledActor* actor, Camera* camera, PassType pass);

	gl::ShaderProgram mShader;
	Texture2D mInvalidTexture;
}; // </ModelRenderer>

}; // </wt>

#endif // </WT_MODELRENDERER_H>