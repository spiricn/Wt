#ifndef WT_SKYBOXRENDERER_H
#define WT_SKYBOXRENDERER_H
#include "wt/ARenderer.h"

#include "wt/Scene.h"

namespace wt{

class SkyboxRenderer : public ARenderer{
public:
	SkyboxRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass);

	bool isDeferred() const;

private:
	gl::ShaderProgram mShader;
}; // </SkyboxRenderer>

}; // </wt>

#endif // </WT_SKYBOXRENDERER_H>