#include "wt/stdafx.h"

#include "wt/DeferredRenderer.h"
#include "wt/IcosphereBuilder.h"
#include "wt/Scene.h"

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

		mTextures[i].setData(width, height, GL_RGB, GL_RGB32F, NULL, GL_FLOAT, false);
		mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0 + i, mTextures + i);
	}

	// Depth/stencil attachment
	mDepthTexture.create();
	mDepthTexture.bind();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	mDepthTexture.setData(width, height, GL_DEPTH_COMPONENT, GL_DEPTH32F_STENCIL8, NULL, GL_FLOAT, false);

	mFrameBuffer.addAttachment(GL_DEPTH_STENCIL_ATTACHMENT, &mDepthTexture);

	// Final texture
	mFinalTexture.create();
	mFinalTexture.bind();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	mFinalTexture.setData(width, height, GL_RGB, GL_RGBA, NULL, GL_FLOAT, false);
	mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT3, &mFinalTexture);

	WT_ASSERT(mFrameBuffer.isComplete(), "Incomplete frame buffer");

	mFrameBuffer.unbindDraw();

		

	// Stencil pass shader
	mStencilPassShader.createFromFiles("shaders/stencil_pass.vp");
	mStencilPassShader.bindAttribLocation(0, "inPosition");
	mStencilPassShader.link();

	// Light pass shader
	for(int i=0; i<eLIGHT_SHADER_MAX; i++){
		if(i == eLIGHT_SHADER_DIRECTIONAL){
			mLightShaders[i].createFromFiles("shaders/light_pass.vp",
				"shaders/dir_light_pass.fp");
		}
		else if(i == eLIGHT_SHADER_POINT){
			mLightShaders[i].createFromFiles("shaders/light_pass.vp",
				"shaders/point_light_pass.fp");
		}
		else{
			continue;
		}
					
		mLightShaders[i].bindAttribLocation(0, "inPosition");

		mLightShaders[i].link();

		mLightShaders[i].use();

		mLightShaders[i].setUniformVal("uPositionMap", 0);
		mLightShaders[i].setUniformVal("uColorMap", 1);
		mLightShaders[i].setUniformVal("uNormalMap", 2);

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

void DeferredRender::resize(uint32_t width, uint32_t height){
	if(mWidth == width && mHeight == height){
		return;
	}

	mWidth = width;
	mHeight = height;

	for(uint32_t i=0; i<eGTEX_MAX; i++){
		mTextures[i].setData(mWidth, mHeight, GL_RGB, GL_RGB32F, NULL, GL_FLOAT, false);
	}

	mDepthTexture.setData(mWidth, mHeight, GL_DEPTH_COMPONENT, GL_DEPTH32F_STENCIL8, NULL, GL_FLOAT, false);

	mFinalTexture.setData(mWidth, mHeight, GL_RGB, GL_RGBA, NULL, GL_FLOAT, false);

	mLightShaders[eLIGHT_SHADER_DIRECTIONAL].use();
	mLightShaders[eLIGHT_SHADER_DIRECTIONAL].setUniformVal("uScreenSize", glm::vec2(mWidth, mHeight));

	mLightShaders[eLIGHT_SHADER_POINT].use();
	mLightShaders[eLIGHT_SHADER_POINT].setUniformVal("uScreenSize", glm::vec2(mWidth, mHeight));
}

void DeferredRender::bindForGeometryPass(){
	// Destination of the geometry pass are the 3 textures (position, color and normals)
	mFrameBuffer.bindDraw();
	const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
	gl( DrawBuffers(3, drawBuffers) );
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
		GL_COLOR_ATTACHMENT3
	);
}

void DeferredRender::bindForLightPass(){
	// Our target is the final buffer
	gl( DrawBuffer(GL_COLOR_ATTACHMENT3) );
		
	// Our source are the 3 textures from the geometry pass
	for(uint32_t i=0; i<eGTEX_MAX; i++){
		gl(ActiveTexture(GL_TEXTURE0 + i));
		mTextures[i].bind();
	}
}

void DeferredRender::bindForFinalPass(){
	// We're rendering into the main framebuffer
	mFrameBuffer.unbindDraw();

	// But we're reading from our final texture
	mFrameBuffer.bindRead();

	glReadBuffer(GL_COLOR_ATTACHMENT3);
}

void DeferredRender::directionalLightPass(Scene* scene, math::Camera* camera){
	bindForLightPass();

	glDisable(GL_CULL_FACE);

	gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_DIRECTIONAL];

	shader.use();

	shader.setUniformVal("uViewMat", glm::mat4(1.0f));
	shader.setUniformVal("uProjMat", glm::mat4(1.0f));
	shader.setUniformVal("uModelMat", glm::mat4(1.0f));

	shader.setUniformVal("uEyePos", camera->getPosition());

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

void DeferredRender::stencilPass(Scene* scene, math::Camera* camera, uint32_t pointLightIndex){
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

	// Acquire the light
	PointLight light;
	scene->getPointLight(pointLightIndex, light);

	math::Transform tf;
	// Position it
	tf.setPosition(light.mPosition);
	// Scale it
	tf.setScale(light.calculateBoundingSphere(), light.calculateBoundingSphere(), light.calculateBoundingSphere());

	// Upload the modelview-projection matrix
	glm::mat4 mvp;
	camera->getMVPMatrix(tf, mvp);
	mStencilPassShader.setUniformVal("uModelViewProj", mvp);

	// Render the sphere
	mSphereBatch.render();
}

	
void DeferredRender::pointLightPass(Scene* scene, math::Camera* camera, uint32_t pointLightIndex){
	// Prepare for the light pass
	bindForLightPass();

	// Fetch the point light shader
	gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_POINT];
	shader.use();

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

	// Acquire the light
	PointLight light;
	scene->getPointLight(pointLightIndex, light);

	math::Transform tf;
	// Position it
	tf.setPosition(light.mPosition);
	// Scale it
	tf.setScale(light.calculateBoundingSphere(), light.calculateBoundingSphere(), light.calculateBoundingSphere());

	// Upload the modelview-projection matrix
	glm::mat4 model;
	tf.getMatrix(model);

	glm::mat4 view;
	camera->getMatrix(view);

	shader.setUniformVal("uViewMat", view);
	shader.setUniformVal("uProjMat", camera->getFrustum().getProjMatrix());
	shader.setUniformVal("uModelMat", model);

	// Render the sphere
	mSphereBatch.render();

	// Cleanup
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}


void DeferredRender::startFrame(){
	mFrameBuffer.bindDraw();

	// We're drawing to our final texture
	gl( DrawBuffer(GL_COLOR_ATTACHMENT3) );

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