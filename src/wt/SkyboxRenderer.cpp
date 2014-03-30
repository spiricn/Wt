#include "wt/stdafx.h"

#include "wt/SkyboxRenderer.h"
#include "wt/ShaderFactory.h"
#include "wt/Skybox.h"

#define  TD_TRACE_TAG "SkyboxRenderer"

namespace wt{

SkyboxRenderer::SkyboxRenderer(){
}

bool SkyboxRenderer::isDeferred() const{
	return false;
}

void SkyboxRenderer::create(){
	LOGV("Compiling skybox shader..");

	ShaderFactory::createShader(mShader, "skybox.vp", "skybox.fp");

	mShader.bindAttribLocation(0, "inVertex");
	mShader.bindAttribLocation(1, "inTexCoord");

	mShader.link();

	mShader.use();
	mShader.setUniformVal("uSkyboxTexture", 0);
}

void SkyboxRenderer::render(Scene* scene, Camera* camera, PassType pass, Texture2D* shadowMap){
	if(pass != ePASS_NORMAL){
		return;
	}

	SkyBox* sky = scene->getSkyBox();
	if(sky == NULL){
		return;
	}

	mShader.use();

	// Set the modelview-projection matrix
	glm::mat4 view;
	camera->getCameraMatrix(view);

	view[3][0] = 0.0f;
	view[3][1] = 0.0f;
	view[3][2] = 0.0f;

	glm::mat4 model;
	sky->getTransform().getTransformMatrix(model);

	mShader.setUniformVal("uModelViewProjection", camera->getProjectionMatrix()*(view*model));

	// Bind the sky texture
	glActiveTexture(GL_TEXTURE0);
	sky->bind();

	// Render the sky
	sky->getBatch().render();
}

}; // </wt>