#include "wt/stdafx.h"

#include "wt/DeferredRenderer.h"
#include "wt/IcosphereBuilder.h"
#include "wt/Scene.h"
#include "wt/ShaderFactory.h"

#define TD_TRACE_TAG "DeferredRender"

static const glm::mat4 kBIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

namespace wt
{

DeferredRender::DeferredRender(uint32_t width, uint32_t height) : mWidth(0), mHeight(0){
	// Framebuffer
	mFrameBuffer.create();
	mFrameBuffer.bindDraw();

	for(uint32_t i=0; i<eGTEX_MAX; i++){
		mTextures[i].create();

		mTextures[i].bind();

		// Disable unecessary interpolation
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if(i== eGTEX_GODRAY){
			mTextures[i].setData(width, height, GL_RGBA, GL_RGBA32F, NULL, GL_FLOAT, false);
		}
		else{
			mTextures[i].setData(width, height, GL_RGB, GL_RGB32F, NULL, GL_FLOAT, false);
		}
		mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0 + i, mTextures + i);
	}

	// Depth/stencil attachment
	for(uint32_t i=0; i<eDEPTH_TEXTURE_MAX; i++){
		mDepthTexture[i].create();
		mDepthTexture[i].bind();
		gl( TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
		gl( TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
		gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) );
		gl( TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) );

		if(i == eDEPTH_TEXTURE_NORMAL){
			mDepthTexture[i].setData(width, height, GL_DEPTH_COMPONENT, GL_DEPTH32F_STENCIL8, NULL, GL_FLOAT, false);
		}
		else if(i == eDEPTH_TEXTURE_SHADOW_MAP){
			mDepthTexture[i].setData(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, NULL, GL_FLOAT, false);
		}
	}

	useDepthTexture(eDEPTH_TEXTURE_NORMAL);

	// Final texture
	mFinalTexture.create();
	mFinalTexture.bind();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	mFinalTexture.setData(width, height, GL_RGB, GL_RGBA, NULL, GL_FLOAT, false);
	mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT4, &mFinalTexture);

	WT_ASSERT(mFrameBuffer.isComplete(), "Incomplete frame buffer");

	mFrameBuffer.unbindDraw();

	// Stencil pass shader
	LOGV("Compiling stencil pass shader");
	ShaderFactory::createShader(mStencilPassShader, "stencil_pass.vp");

	mStencilPassShader.bindAttribLocation(0, "inPosition");
	mStencilPassShader.link();

	// Light pass shader
	for(int i=0; i<eLIGHT_SHADER_MAX; i++){
		if(i == eLIGHT_SHADER_DIRECTIONAL){
			LOGV("Compiling directional light shader");
			ShaderFactory::createShader(mLightShaders[i], "light_pass.vp", "dir_light_pass.fp");
		}
		else if(i == eLIGHT_SHADER_POINT){
			LOGV("Compiling point light shader");
			ShaderFactory::createShader(mLightShaders[i], "light_pass.vp", "point_light_pass.fp");
		}
		else{
			continue;
		}
					
		mLightShaders[i].bindAttribLocation(0, "inPosition");

		mLightShaders[i].link();

		mLightShaders[i].use();

		mLightShaders[i].setUniformVal("uLighting.material.ambientColor", Color::White());
		mLightShaders[i].setUniformVal("uLighting.material.diffuseColor", Color::White());
		mLightShaders[i].setUniformVal("uLighting.material.specularColor", Color::Black());
		mLightShaders[i].setUniformVal("uLighting.material.shininess", 0.0f);

		mLightShaders[i].setUniformVal("uPositionMap", 0);
		mLightShaders[i].setUniformVal("uColorMap", 1);
		mLightShaders[i].setUniformVal("uNormalMap", 2);
		mLightShaders[i].setUniformVal("uLighting.shadowMap", 3);

		mLightShaders[i].setUniformVal("uViewMat", glm::mat4(1.0f));
		mLightShaders[i].setUniformVal("uProjMat", glm::mat4(1.0f));
		mLightShaders[i].setUniformVal("uModelMat", glm::mat4(1.0f));
	}
	{
		// Create a quad batch (used for directional lighting)
		const float z = 0;
		const float vertices[6*3] = {
			-1, 1, z,
			1, 1, z,
			1, -1, z,
			-1, -1, z,
			-1, 1, z,
			1, -1, z,
		};

		mQuadBatch.create(GL_TRIANGLES, vertices, 6, 3*sizeof(float));
		mQuadBatch.setVertexAttribute(0, 3, GL_FLOAT, 0);
	}

	{
		// Create a sphere batch (used for point lighting)
		IcosphereBuilder::Vertex* vertices;
		uint32_t* indices;
		uint32_t numIndices, numVertices;

		IcosphereBuilder(&vertices, &indices, &numVertices, &numIndices, 1);

		// TODO if IcosphereBuilder::Vertex structure changes this will break
		mSphereBatch.create(GL_TRIANGLES, vertices, numVertices, sizeof(IcosphereBuilder::Vertex),
			indices, numIndices, sizeof(uint32_t), GL_UNSIGNED_INT);

		mSphereBatch.setVertexAttribute(0, 3, GL_FLOAT, offsetof(IcosphereBuilder::Vertex, position));
	}

	resize(width, height);
}

Texture2D* DeferredRender::getDepthTexture(DepthTexture type) const{
	// TODO
	return &const_cast<DeferredRender*>(this)->mDepthTexture[type];
}

void DeferredRender::useDepthTexture(DepthTexture type){
	if(type == eDEPTH_TEXTURE_NORMAL){
		mFrameBuffer.removeAttachments(GL_DEPTH_ATTACHMENT);
		mFrameBuffer.addAttachment(GL_DEPTH_STENCIL_ATTACHMENT, &mDepthTexture[type]);

	}
	else{
		mFrameBuffer.removeAttachments(GL_DEPTH_STENCIL_ATTACHMENT);
		mFrameBuffer.addAttachment(GL_DEPTH_ATTACHMENT, &mDepthTexture[type]);
	}
}

Texture2D* DeferredRender::getGTexture(TextureType type){
	return &mTextures[type];
}

void DeferredRender::uploadPointLight(const ShaderPointLight& light){
	gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_POINT];

	shader.use();

	shader.setUniformValFmt(light.light->getDesc().color, "uLighting.pointLights[%d].color", light.index);

	shader.setUniformValFmt(light.light->getDesc().ambientIntensity, "uLighting.pointLights[%d].ambientItensity", light.index);
	shader.setUniformValFmt(light.light->getDesc().diffuseIntensity, "uLighting.pointLights[%d].diffuseItensity", light.index);
	shader.setUniformValFmt(light.light->getDesc().position, "uLighting.pointLights[%d].position", light.index);

	shader.setUniformValFmt(light.light->getDesc().attenuation.linear, "uLighting.pointLights[%d].attenuation.linear", light.index);
	shader.setUniformValFmt(light.light->getDesc().attenuation.constant, "uLighting.pointLights[%d].attenuation.constant", light.index);
	shader.setUniformValFmt(light.light->getDesc().attenuation.quadratic, "uLighting.pointLights[%d].attenuation.exponential", light.index);
}


void DeferredRender::uploadDirectionalLight(const DirectionalLight* light){
	gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_DIRECTIONAL];

	shader.use();

	shader.setUniformVal("uLighting.directionalLight.ambientItensity",
		light->getDesc().ambientIntensity);

	shader.setUniformVal("uLighting.directionalLight.color",
		light->getDesc().color);

	shader.setUniformVal("uLighting.directionalLight.diffuseItensity",
		light->getDesc().diffuseIntensity);

	shader.setUniformVal("uLighting.directionalLight.direction",
		light->getDesc().direction);
}



DeferredRender::ShaderPointLight* DeferredRender::findShaderPointLight(const PointLight* light){
	for(int i=0; i<10; i++){
		if(mShaderPointLights[i].active && mShaderPointLights[i].light == light){
			return &mShaderPointLights[i];
		}
	}

	return NULL;
}

void DeferredRender::onSceneShadowMappingParamsChanged(Scene* scene, const Scene::ShadowMappingDesc& desc){
	for(uint32_t i=0; i<eLIGHT_SHADER_MAX; i++){
		gl::ShaderProgram& shader = mLightShaders[i];

		shader.use();

		shader.setUniformVal("uLighting.shadowMappingEnabled", desc.enabled);
		shader.setUniformVal("uLighting.shadowIntensity", desc.shadowIntensity);

		if(desc.enabled){
			glm::mat4 mvp;
			desc.casterSource.getCameraMatrix(mvp);

			mvp = desc.casterSource.getProjectionMatrix() * mvp;

			setShadowCasterMatrix(mvp);
		}
	}
}

void DeferredRender::onSceneLightUpdated(Scene* scene, const ALight& aLight){
	if(aLight.getType() == ALight::eTYPE_POINT){
		const PointLight* light = dynamic_cast<const PointLight*>( &aLight );

		ShaderPointLight* shaderLight = findShaderPointLight(light);
		WT_ASSERT(shaderLight != NULL, "Sanity check fail");

		uploadPointLight(*shaderLight);
	}
	else if(aLight.getType() == ALight::eTYPE_DIRECTIONAL){
		uploadDirectionalLight( dynamic_cast<const DirectionalLight*>( &aLight ) );
	}
}

void DeferredRender::onSceneLightCreated(Scene* scene, const ALight& aLight){
	if(aLight.getType() == ALight::eTYPE_POINT){
		const PointLight* light = dynamic_cast<const PointLight*>( &aLight );

		for(uint32_t i=0; i<10; i++){
			if(!mShaderPointLights[i].active){
				mShaderPointLights[i].active = true;
				mShaderPointLights[i].light = light;
				mShaderPointLights[i].index = i;
				uploadPointLight( mShaderPointLights[i] );
				break;
			}
		}
	}
	else if(aLight.getType() == ALight::eTYPE_DIRECTIONAL){
		uploadDirectionalLight( dynamic_cast<const DirectionalLight*>( &aLight ) );
	}
	else{
		// TODO
	}
}

void DeferredRender::onSceneFogParamsChanged(Scene* scene, const FogDesc& desc){
	gl::ShaderProgram* shader = &mLightShaders[eLIGHT_SHADER_DIRECTIONAL];

	shader->use();
	shader->setUniformVal("uLighting.fog.density", desc.density);
	shader->setUniformVal("uLighting.fog.color", desc.color);
	shader->setUniformVal("uLighting.fog.enabled", desc.enabled);
}

void DeferredRender::onSceneLightDeleted(Scene* scene, const ALight& aLight){
	if(aLight.getType() == ALight::eTYPE_POINT){
		const PointLight* light = dynamic_cast<const PointLight*>(&aLight);

		ShaderPointLight* shaderLight = findShaderPointLight(light);
		WT_ASSERT(shaderLight != NULL, "Sanity check fail");

		mShaderPointLights[shaderLight->index].active = false;
	}
	else{
		// TODO
	}
}

void DeferredRender::resize(uint32_t width, uint32_t height){
	if(mWidth == width && mHeight == height){
		return;
	}

	mWidth = width;
	mHeight = height;

	for(uint32_t i=0; i<eGTEX_MAX; i++){
		if(i== eGTEX_GODRAY){
			mTextures[i].setData(width, height, GL_RGBA, GL_RGBA32F, NULL, GL_FLOAT, false);
		}
		else{
			mTextures[i].setData(width, height, GL_RGB, GL_RGB32F, NULL, GL_FLOAT, false);
		}
	}

	mDepthTexture[eDEPTH_TEXTURE_NORMAL].setData(width, height, GL_DEPTH_COMPONENT, GL_DEPTH32F_STENCIL8, NULL, GL_FLOAT, false);
	mDepthTexture[eDEPTH_TEXTURE_SHADOW_MAP].setData(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, NULL, GL_FLOAT, false);

	mFinalTexture.setData(mWidth, mHeight, GL_RGB, GL_RGBA, NULL, GL_FLOAT, false);

	mLightShaders[eLIGHT_SHADER_DIRECTIONAL].use();
	mLightShaders[eLIGHT_SHADER_DIRECTIONAL].setUniformVal("uScreenSize", glm::vec2(mWidth, mHeight));

	mLightShaders[eLIGHT_SHADER_POINT].use();
	mLightShaders[eLIGHT_SHADER_POINT].setUniformVal("uScreenSize", glm::vec2(mWidth, mHeight));
}

void DeferredRender::bindForGeometryPass(){
	// Destination of the geometry pass are the 4 textures (position, color, normals, godray geometry)
	mFrameBuffer.bindDraw();
	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3}; 
	gl( DrawBuffers(4, drawBuffers) );
}

void DeferredRender::debugBlit(const glm::vec2& screenSize){
	mFrameBuffer.blit(
		glm::vec4(0, 0,  mWidth, mHeight),
		glm::vec4(0, 0, screenSize.x/2, screenSize.y/2),
		GL_COLOR_ATTACHMENT0
	);

	mFrameBuffer.blit(
		glm::vec4(0, 0,  mWidth, mHeight),
		glm::vec4(screenSize.x/2, 0, screenSize.x, screenSize.y/2),
		GL_COLOR_ATTACHMENT1
	);

	mFrameBuffer.blit(
		glm::vec4(0, 0,  mWidth, mHeight),
		glm::vec4(screenSize.x/2, screenSize.y/2, screenSize.x, screenSize.y),
		GL_COLOR_ATTACHMENT2
	);

	mFrameBuffer.blit(
		glm::vec4(0, 0,  mWidth, mHeight),
		glm::vec4(0, screenSize.y/2, screenSize.x/2, screenSize.y),
		GL_COLOR_ATTACHMENT4
	);
}

void DeferredRender::bindForLightPass(){
	// Our target is the final buffer
	gl( DrawBuffer(GL_COLOR_ATTACHMENT4) );
		
	// Our source are the 3 textures from the geometry pass (we're not using the godray one)
	gl(ActiveTexture(GL_TEXTURE0));
	mTextures[eGTEX_POSITION].bind();

	gl(ActiveTexture(GL_TEXTURE1));
	mTextures[eGTEX_DIFFUSE].bind();

	gl(ActiveTexture(GL_TEXTURE2));
	mTextures[eGTEX_NORMAL].bind();

	gl(ActiveTexture(GL_TEXTURE3));
	mDepthTexture[eDEPTH_TEXTURE_SHADOW_MAP].bind();
}

void DeferredRender::bindForFinalPass(){
	// We're rendering into the main framebuffer
	mFrameBuffer.unbindDraw();
	// But we're reading from our final texture
	mFrameBuffer.bindRead();

	glReadBuffer(GL_COLOR_ATTACHMENT4);
}

void DeferredRender::directionalLightPass(Scene* scene, math::Camera* camera){
	bindForLightPass();

	glDisable(GL_CULL_FACE);

	gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_DIRECTIONAL];

	shader.use();

	shader.setUniformVal("uLighting_depthBiasLightMVP", mShadowCasterMatrix);
	shader.setUniformVal("uViewMat", glm::mat4(1.0f));
	shader.setUniformVal("uProjMat", glm::mat4(1.0f));
	shader.setUniformVal("uModelMat", glm::mat4(1.0f));

	glm::vec3 eyePos;
	camera->getTranslation(eyePos);

	shader.setUniformVal("uLighting.eyePos", eyePos);

	glDisable(GL_DEPTH_TEST);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	mQuadBatch.render();
}

void DeferredRender::bindForStencilPass(){
	// Disable color write so that our stencil pass does not black out our textures
	glDrawBuffer(GL_NONE);
}

void DeferredRender::stencilPass(Scene* scene, math::Camera* camera, const PointLight* light){
	bindForStencilPass();

	mStencilPassShader.use();

	glEnable(GL_STENCIL_TEST);

	glDepthMask(false);

	// Enable depth test because stencil operation depends on it
	gl( Enable(GL_DEPTH_TEST) );

	// Disable face culling ( we actually need both faces to be rendered
	gl( Disable(GL_CULL_FACE) );

	glClearStencil(1);

	// Clear the stencil buffer (previous light might have altered it)
	gl( Clear(GL_STENCIL_BUFFER_BIT) );

	// Enable stencil test and make it succeed always by setting ref and mask to zero
	gl( StencilFunc(GL_ALWAYS, 0, 0) );

	glStencilOpSeparate(
		GL_BACK,	// backface (clockwise)
		GL_KEEP,	// ignore
		GL_INCR,	// when depth fails
		GL_KEEP		// ignore
	);

	glStencilOpSeparate(
		GL_FRONT,	// front face (counter-clockwise)
		GL_KEEP,	// ignore
		GL_DECR,	// when depth fails
		GL_KEEP		// ignore 
	);

	math::Transform tf;
	// Position it
	tf.setPosition(light->getDesc().position);
	// Scale it
	tf.setScale(light->getDesc().calculateBoundingSphere(), light->getDesc().calculateBoundingSphere(), light->getDesc().calculateBoundingSphere());

	// Upload the modelview-projection matrix
	glm::mat4 model, view;
	tf.getTransformMatrix(model);

	camera->getCameraMatrix(view);

	mStencilPassShader.setUniformVal("uModelViewProj", camera->getProjectionMatrix() * (view * model) );

	// Render the sphere
	mSphereBatch.render();
}

void DeferredRender::setShadowCasterMatrix(const glm::mat4& matrix){
	mShadowCasterMatrix = kBIAS_MATRIX * matrix;
}

void DeferredRender::pointLightPass(Scene* scene, math::Camera* camera, const PointLight* light){
	ShaderPointLight* shaderPointLight = findShaderPointLight(light);
	if(shaderPointLight == NULL){
		// Event has not reached us yet
		return;
	}

	// Prepare for the light pass
	bindForLightPass();

	// Fetch the point light shader
	gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_POINT];
	shader.use();

	shader.setUniformVal("uLighting_depthBiasLightMVP", mShadowCasterMatrix);

	// Render only pixels where stencil value is != 0
	// if ( ref & mask ) != ( stencil & mask ).
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

	// We don't need the depth test (handled by stencil test)
	glDisable(GL_DEPTH_TEST);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	// Enable front-face culling
	// The reason why we do that is because the camera may be inside the light volume
	// and if we do back face culling as we normally do we will not see the light until we exit its volume
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	math::Transform tf;
	// Position it
	tf.setPosition(light->getDesc().position);
	// Scale it
	tf.setScale(light->getDesc().calculateBoundingSphere(), light->getDesc().calculateBoundingSphere(), light->getDesc().calculateBoundingSphere());

	// Upload the modelview-projection matrix
	glm::mat4 model;
	tf.getMatrix(model);

	glm::mat4 view;
	camera->getCameraMatrix(view);

	shader.setUniformVal("uViewMat", view);
	shader.setUniformVal("uProjMat", camera->getProjectionMatrix());
	shader.setUniformVal("uModelMat", model);


	shader.setUniformVal("uPointLightIndex", (int)shaderPointLight->index);

	// Render the sphere
	mSphereBatch.render();

	// Cleanup
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

void DeferredRender::doLightPass(Scene* scene, math::Camera* camera){
	// When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    gl( DepthMask(GL_FALSE) );
	gl( Enable(GL_STENCIL_TEST ));

	

	for(Scene::ActorSet::const_iterator iter=scene->getActorSet(ASceneActor::eTYPE_POINT_LIGHT).begin(); iter!=scene->getActorSet(ASceneActor::eTYPE_POINT_LIGHT).end(); iter++){
		const PointLight* light = dynamic_cast<const PointLight*>(*iter);
		if(light->getDesc().enabled){
			stencilPass(scene, camera, light);
			pointLightPass(scene, camera, light);
		}
	}
	
	// The directional light does not need a stencil test because its volume
    // is unlimited and the final pass simply copies the texture.

	gl( Disable(GL_STENCIL_TEST ));

	directionalLightPass(scene, camera);
}

void DeferredRender::startFrame(){
	mFrameBuffer.bindDraw();

	// We're drawing to our final texture
	gl( DrawBuffer(GL_COLOR_ATTACHMENT4) );

	// Clear our final texture (depth buffer and the other textures are taken care of in geometry pass)
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

gl::ShaderProgram* DeferredRender::getLightShader(LightPassShader shader){
	return &mLightShaders[shader];
}

gl::FrameBuffer* DeferredRender::getFrameBuffer(){
	return &mFrameBuffer;
}

} // </wt>