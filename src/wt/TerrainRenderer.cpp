#include "wt/stdafx.h"

#include "wt/TerrainRenderer.h"

#define  TD_TRACE_TAG "TerrainRenderer"


namespace wt{

TerrainRenderer::TerrainRenderer(){
}

bool TerrainRenderer::isDeferred() const{
	return true;
}

void TerrainRenderer::create(){
	LOGV("Compiling terrain shader...");
	mShader.createFromFiles("shaders\\terrain_deferred.vp",
		"shaders\\terrain_deferred.fp");

	mShader.bindAttribLocation(0, "inPosition");
	mShader.bindAttribLocation(1, "inTexture");
	mShader.bindAttribLocation(2, "inTiledTexture");
	mShader.bindAttribLocation(3, "inNormal");

	mShader.link();

	mShader.use();
	mShader.setUniformVal("texArray", 0);
	mShader.setUniformVal("map", 1);
}

void TerrainRenderer::render(Scene* scene, math::Camera* camera, PassType pass){
	Terrain* terrain = (Terrain*)(*scene->getTerrainSet().begin());

	// Render terrain
	mShader.use();

	gl::ShaderUniform<int>(&mShader, "uPassType") = pass;

	glm::mat4x4 view;
	camera->getCameraMatrix(view);

	glm::mat4 model;
	terrain->getTransformable()->getTransformMatrix(model);

	mShader.setUniformVal("uModelMat", model);
	mShader.setUniformVal("uViewMat",view);
	mShader.setUniformVal("uProjMat", camera->getProjectionMatrix());

	glActiveTexture(GL_TEXTURE0);
	terrain->getTerrainTexture()->bind();

	glActiveTexture(GL_TEXTURE1);
	terrain->getMapTexture()->bind();


	render(scene, terrain->getRootNode() );
}

gl::ShaderProgram* TerrainRenderer::setupSceneLighting(){
	return &mShader;
}

void TerrainRenderer::render(Scene* scene, const TerrainNode* node){
	// frustum cull test
	if(true /* inFrustum(frustum, node->getBounds() */){
		// have we reached a leaf?
		if(!node->isLeaf()){
			// render children
			for(uint32_t i=0; i<4; i++){
				render(scene, &node->getChildren()[i]);
			}
		}
		else{
			gl( Disable(GL_TEXTURE_2D) );
			node->render();
		}
	}
}

}; // </wt>
