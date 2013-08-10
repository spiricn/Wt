#ifndef WT_TERRAINRENDERER_H
#define WT_TERRAINRENDERER_H

#include "wt/ARenderer.h"

#include "wt/Scene.h"

namespace wt{

#define  TD_TRACE_TAG "TerrainRenderer"

class TerrainRenderer : public ARenderer{
public:
	TerrainRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass);

	gl::ShaderProgram* setupSceneLighting();

private:
	void render(Scene* scene, const TerrainNode* node);

	gl::ShaderProgram mShader;

}; // </TerrainRenderer>

}; // </wt>

#define TD_TRACE_TAG ""

#endif // </WT_TERRAINRENDERER_H>