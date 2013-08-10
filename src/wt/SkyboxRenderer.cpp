#include "wt/stdafx.h"

#include "wt/SkyboxRenderer.h"

#define  TD_TRACE_TAG "SkyboxRenderer"

namespace wt{

SkyboxRenderer::SkyboxRenderer(){
}

void SkyboxRenderer::create(){
	LOGV("Compiling skybox shader..");

	mShader.createFromFiles("shaders/skybox.vp", "shaders/skybox.fp");

	mShader.bindAttribLocation(0, "inVertex");
	mShader.bindAttribLocation(1, "inTexCoord");

	mShader.link();

	mShader.use();
	mShader.setUniformVal("uSkyboxTexture", 0);
}

void SkyboxRenderer::render(Scene* scene, math::Camera* camera, PassType pass){
	if(pass != ePASS_NORMAL){
		return;
	}

	SkyBox* sky = scene->getSkyBox();
	if(sky == NULL){
		return;
	}

	mShader.use();

	// Set the modelview-projection matrix
	glm::mat4 mvp;
	camera->getMVPMatrix(sky->getTransform(), mvp, true);
	mShader.setUniformVal("uModelViewProjection", mvp);

	// Bind the sky texture
	glActiveTexture(GL_TEXTURE0);
	sky->bind();

	// Render the sky
	sky->getBatch().render();
}

}; // </wt>