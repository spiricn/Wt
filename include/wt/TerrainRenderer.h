#ifndef WT_TERRAINRENDERER_H
#define WT_TERRAINRENDERER_H

#include "wt/ARenderer.h"
#include "wt/Scene.h"
#include "wt/GLShaderProgram.h"

namespace wt
{

class TerrainNode;

#define  TD_TRACE_TAG "TerrainRenderer"

class TerrainRenderer : public ARenderer{
public:
	TerrainRenderer();

	void create();

	void render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap);

	gl::ShaderProgram* setupSceneLighting();

	bool isDeferred() const;

private:
	void render(Scene* scene, const TerrainNode* node);

	gl::ShaderProgram mShader;

}; // </TerrainRenderer>

}; // </wt>

#define TD_TRACE_TAG ""

#endif // </WT_TERRAINRENDERER_H>