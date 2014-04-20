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
	TerrainRenderer(bool boundsRenderer);

	void create();

	void render(Scene* scene, Camera* camera, PassType pass, Texture2D* shadowMap);

	gl::ShaderProgram* setupSceneLighting();

	bool isDeferred() const;

private:
	void render(Scene* scene, const TerrainNode* node);

private:
	gl::ShaderProgram mShader;
	bool mBoundsRenderer;

}; // </TerrainRenderer>

}; // </wt>

#define TD_TRACE_TAG ""

#endif // </WT_TERRAINRENDERER_H>