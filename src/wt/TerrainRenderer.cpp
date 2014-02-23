#include "wt/stdafx.h"

#include "wt/TerrainRenderer.h"
#include "wt/ShaderFactory.h"

#define  TD_TRACE_TAG "TerrainRenderer"

namespace wt
{

TerrainRenderer::TerrainRenderer(){
}

bool TerrainRenderer::isDeferred() const{
	return true;
}

void TerrainRenderer::create(){
	LOGV("Compiling terrain shader...");

	ShaderFactory::createShader(mShader,
		"terrain_deferred.vp",
		"terrain_deferred.fp");

	mShader.bindAttribLocation(0, "inPosition");
	mShader.bindAttribLocation(1, "inTexture");
	mShader.bindAttribLocation(2, "inTiledTexture");
	mShader.bindAttribLocation(3, "inNormal");

	mShader.link();

	mShader.use();
	mShader.setUniformVal("texArray", 0);
	mShader.setUniformVal("map", 1);
}

void TerrainRenderer::render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap){
	//// Terrain is not a shadow caster
	//if(pass != ePASS_NORMAL){
	//	return;
	//}

	if(scene->getActorSet(ASceneActor::eTYPE_TERRAIN).empty()){
		return;
	}

	Terrain* terrain = dynamic_cast<Terrain*>( (*scene->getActorSet(ASceneActor::eTYPE_TERRAIN).begin()) );

	// Render terrain
	mShader.use();

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

	// Enable backface culling
	gl( Enable(GL_CULL_FACE) );
	gl( CullFace(GL_BACK) );

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
