#include "wt/stdafx.h"

#include "wt/ModelRenderer.h"
#include "wt/UniformBufferObject.h"
#include "wt/FrameBuffer.h"
#include "wt/IcosphereBuilder.h"

#define  TD_TRACE_TAG "ModelRenderer"

#include "wt/Renderer.h"

#define DEFERRED_SHADING

namespace wt
{


gl::Batch gSphere;


class DeferredRender{
public:
	enum TextureType{
		eGTEX_INVALID = -1,

		eGTEX_POSITION = 0,
		eGTEX_DIFFUSE,
		eGTEX_NORMAL,

		eGTEX_MAX
	};

	enum LightPassShader{
		eLIGHT_SHADER_NONE=-1,

		eLIGHT_SHADER_DIRECTIONAL = 0,
		eLIGHT_SHADER_POINT,

		eLIGHT_SHADER_MAX,
	};

	Texture2D mTextures[eGTEX_MAX];
	Texture2D mDepthTexture;
	Texture2D mFinalTexture;

	gl::FrameBuffer mFrameBuffer;
	uint32_t mWidth, mHeight;
	gl::ShaderProgram mShader;

	gl::ShaderProgram mLightShaders[eLIGHT_SHADER_MAX];
	gl::ShaderProgram mStencilPassShader;

	gl::Batch mQuadBatch;
	gl::Batch mSphereBatch;

	DeferredRender() : mWidth(1280), mHeight(720){
		// Framebuffer
		mFrameBuffer.create();
		mFrameBuffer.bindDraw();

		for(uint32_t i=0; i<eGTEX_MAX; i++){
			mTextures[i].create();

			mTextures[i].bind();

			// Disable unecessary interpolation
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			mTextures[i].setData(mWidth, mHeight, GL_RGB, GL_RGB32F, NULL, GL_FLOAT, false);
			mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0 + i, mTextures + i);
		}

		// Depth/stencil attachment
		mDepthTexture.create();
		mDepthTexture.bind();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		mDepthTexture.setData(mWidth, mHeight, GL_DEPTH_COMPONENT, GL_DEPTH32F_STENCIL8, NULL, GL_FLOAT, false);

		mFrameBuffer.addAttachment(GL_DEPTH_STENCIL_ATTACHMENT, &mDepthTexture);

		// Final texture
		mFinalTexture.create();
		mFinalTexture.bind();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		mFinalTexture.setData(mWidth, mHeight, GL_RGB, GL_RGBA, NULL, GL_FLOAT, false);
		mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT3, &mFinalTexture);

		WT_ASSERT(mFrameBuffer.isComplete(), "Incomplete frame buffer");

		mFrameBuffer.unbindDraw();

		// Geometry pass shader
		mShader.createFromFiles("shaders/geometry_pass.vp",
			"shaders/geometry_pass.fp");
		
		mShader.bindAttribLocation(Model::eATTRIB_POSITION, "inVertex");
		mShader.bindAttribLocation(Model::eATTRIB_TEXCOORD, "inTexCoord");
		mShader.bindAttribLocation(Model::eATTRIB_NORMAL, "inNormals");
		mShader.bindAttribLocation(Model::eATTRIB_TANGENT, "inTangent");
		mShader.bindAttribLocation(Model::eATTRIB_BONE_ID, "inBoneIds");
		mShader.bindAttribLocation(Model::eATTRIB_BONE_WEIGHT, "inWeights");

		mShader.link();

		mShader.use();
		mShader.setUniformVal("uTextureSampler", 0);

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
	}

	void bindForGeometryPass(){
		// Destination of the geometry pass are the 3 textures (position, color and normals)
		mFrameBuffer.bindDraw();
		const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
		gl( DrawBuffers(3, drawBuffers) );
	}

	void debugBlit(const glm::vec2& screenSize){
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

	void bindForLightPass(){
		// Our target is the final buffer
		gl( DrawBuffer(GL_COLOR_ATTACHMENT3) );
		
		// Our source are the 3 textures from the geometry pass
		for(uint32_t i=0; i<eGTEX_MAX; i++){
			gl(ActiveTexture(GL_TEXTURE0 + i));
			mTextures[i].bind();
		}
	}

	void bindForFinalPass(){
		// We're rendering into the main framebuffer
		mFrameBuffer.unbindDraw();

		// But we're reading from our final texture
		mFrameBuffer.bindRead();

		glReadBuffer(GL_COLOR_ATTACHMENT3);
	}

	void directionalLightPass(Scene* scene, math::Camera* camera){
		bindForLightPass();

		glDisable(GL_CULL_FACE);

		gl::ShaderProgram& shader = mLightShaders[eLIGHT_SHADER_DIRECTIONAL];

		shader.use();

		shader.setUniformVal("uViewMat", glm::mat4(1.0f));
		shader.setUniformVal("uProjMat", glm::mat4(1.0f));
		shader.setUniformVal("uModelMat", glm::mat4(1.0f));

		shader.setUniformVal("uEyePos", camera->getPosition());

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);

		// Enable blending
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		mQuadBatch.render();
	}

	

	void bindForStencilPass(){
		// Disable color write so that our stencil pass does not black out our textures
		glDrawBuffer(GL_NONE);
	}

	void stencilPass(Scene* scene, math::Camera* camera, uint32_t pointLightIndex){
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

	
	void pointLightPass(Scene* scene, math::Camera* camera, uint32_t pointLightIndex){
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


	void startFrame(){
		mFrameBuffer.bindDraw();

		// We're drawing to our final texture
		gl( DrawBuffer(GL_COLOR_ATTACHMENT3) );

		// Clear our final texture (depth buffer and the other textures are taken care of in geometry pass)
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}; // </DeferredRender>

ModelRenderer::ModelRenderer(){
}

void ModelRenderer::create(){
	// Compile the shader
	LOGV("Compiling model shader...");

	mShader.createFromFiles("shaders\\model.vp",
		"shaders\\model.fp");

	mShader.bindAttribLocation(Model::eATTRIB_POSITION, "inVertex");
	mShader.bindAttribLocation(Model::eATTRIB_TEXCOORD, "inTexCoord");
	mShader.bindAttribLocation(Model::eATTRIB_NORMAL, "inNormals");
	mShader.bindAttribLocation(Model::eATTRIB_TANGENT, "inTangent");
	mShader.bindAttribLocation(Model::eATTRIB_BONE_ID, "inBoneIds");
	mShader.bindAttribLocation(Model::eATTRIB_BONE_WEIGHT, "inWeights");

	mShader.link();

	mShader.use();
	mShader.setUniformVal("uSampler", 0);
	mShader.setUniformVal("uNormalMap", 1);

#if 0
	// Uniform buffer object example (TODO use this)
	{
		// Get an array of uniform indices
		GLuint indices[3];

		mShader.getUniformIndices(3, indices,
			"TestBlock.r",
			"TestBlock.g",
			"TestBlock.b"
		);

		// Get an array of uniform offsets in memory
		GLint uniformOffsets[3];
		mShader.getActiveUniforms(3, indices, GL_UNIFORM_OFFSET, uniformOffsets);


		// Allocate some memory for the UBO
		char* bfr = new char[3*sizeof(float)];
		memset(bfr, 0x00, 3*sizeof(float));

		// Set some uniform values
		*((float*)(bfr + uniformOffsets[0])) = 1.0f;
		*((float*)(bfr + uniformOffsets[1])) = 0.3f;
		*((float*)(bfr + uniformOffsets[2])) = 1.0f;

		// Setup shader binding for the uniform block
		const int kTEST_BLOCK_BINDING = 0;
		mShader.createUniformBlockBindPoint("TestBlock", kTEST_BLOCK_BINDING);

		// Create the actual UBO
		static gl::UniformBufferObject ubo;
		ubo.setData(bfr, 3*sizeof(float));

		// Set its bind point
		ubo.setBindPoint(kTEST_BLOCK_BINDING);
	}
#endif
	// Create a default (invalid) texture 
	Buffer<unsigned char> bfr;
	utils::makeCheckboard(bfr, 100, 100, 5, 5, Color(246/255.0, 2/255.0, 134/255.0), Color::black());
	mInvalidTexture.create();
	mInvalidTexture.setData(100, 100, GL_RGB, GL_RGB, (const GLbyte*)bfr.getData());

	mDeferredRender = new DeferredRender;
}

void ModelRenderer::onSceneLightingChanged(Scene* scene, Renderer* renderer){
#ifdef DEFERRED_SHADING
	renderer->setShaderLightUniforms(scene, mDeferredRender->mLightShaders[0]);
	renderer->setShaderLightUniforms(scene, mDeferredRender->mLightShaders[1]);
#else
	renderer->setShaderLightUniforms(scene, mShader);
#endif
}

void ModelRenderer::render(Scene* scene, math::Camera* camera, PassType pass){
#ifdef DEFERRED_SHADING

	
	glm::mat4 viewMat;
	camera->getMatrix(viewMat);

	mDeferredRender->startFrame();

	// WARNING: it's very important for the front face to be set as counter-clockwise
	// otherwise this whole concept breaks
	glFrontFace(GL_CCW);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Do the geometry pass (populate the G buffer)
	gl( DepthMask(true) );
	gl( Enable(GL_DEPTH_TEST) );

	// Bind the drawing buffers (3 textures)
	mDeferredRender->bindForGeometryPass();

	// Clear them all, along with the depth/stencil buffer
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the shader
	mDeferredRender->mShader.use();
	mDeferredRender->mShader.setUniformVal("uViewMat", viewMat);
	mDeferredRender->mShader.setUniformVal("uProjMat", camera->getFrustum().getProjMatrix());
	glActiveTexture(GL_TEXTURE0);

	// Render the geometry
	for(Scene::ModelledActorSet::const_iterator iter=scene->getModelledActors().cbegin(); iter!=scene->getModelledActors().cend(); iter++){
		const ModelledActor* actor = *iter;
		mDeferredRender->mShader.use();

		// Upload skinning matrices
		if(actor->getAnimationPlayer()){
			mDeferredRender->mShader.setUniformVal("uBones",
				actor->getAnimationPlayer()->getBoneMatrices(),
				actor->getAnimationPlayer()->getNumBones()
			);
		}
	
		// Upload model matrix
		glm::mat4 modelMat;
		actor->getTransform().getMatrix(modelMat);
		mDeferredRender->mShader.setUniformVal("uModelMat", modelMat);

		for(Model::GeometrySkin::MeshList::iterator i=actor->getSkin()->getMeshList().begin(); i!=actor->getSkin()->getMeshList().end(); i++){
			// Use the material if the mesh has one
			Texture2D* texture = i->texture;

			if(texture != NULL){
				texture->bind();
			}
			else{
				// Use the default one if none is provided
				mInvalidTexture.bind();
			}

			// Render the geometry
			actor->getModel()->getBatch().bind();
			i->geometry->render();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
    gl( DepthMask(GL_FALSE) );
	gl( Enable(GL_STENCIL_TEST ));

	for(uint32_t i=0; i<scene->getNumPointLights(); i++){
		PointLight light;

		scene->getPointLight(i, light);
		if(!light.mActive){
			continue;
		}

		mDeferredRender->stencilPass(scene, camera, i);
		mDeferredRender->pointLightPass(scene, camera, i);
	}

	// The directional light does not need a stencil test because its volume
    // is unlimited and the final pass simply copies the texture.

	gl( Disable(GL_STENCIL_TEST ));

	mDeferredRender->directionalLightPass(scene, camera);

	//mDeferredRender->mDepthTexture.dump("depth.bmp");

#if 1
	// Final pass (render the resulting texture to the screen)
	mDeferredRender->bindForFinalPass();
	mDeferredRender->mFrameBuffer.blit(
		glm::vec4(0, 0, 1280, 720),
		glm::vec4(0, 0, 1280, 720), GL_COLOR_ATTACHMENT3, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#else
	//// Debug blit
	gl::FrameBuffer::unbind();

	GLint doubleBuffered; 
	gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

	GLenum defaultBfrs[1];
	defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

	gl( DrawBuffers(1, defaultBfrs) );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDeferredRender->debugBlit(glm::vec2(1280, 720));
#endif

	gl::FrameBuffer::unbind();

#else

IcosphereBuilder::Vertex* vertices;
	uint32_t* indices;
	uint32_t numIndices, numVertices;

	IcosphereBuilder(&vertices, &indices, &numVertices, &numIndices, 1);

	Geometry::Vertex* geoVertices = new Geometry::Vertex[numVertices];

	for(uint32_t i=0; i<numVertices; i++){
		geoVertices[i].x = vertices[i].position.x;
		geoVertices[i].y = vertices[i].position.y;
		geoVertices[i].z = vertices[i].position.z;

		geoVertices[i].nx = vertices[i].normal.x;
		geoVertices[i].ny = vertices[i].normal.y;
		geoVertices[i].nz = vertices[i].normal.z;

		geoVertices[i].s = vertices[i].texture.s;
		geoVertices[i].t = vertices[i].texture.t;

	}

	gSphere.create(GL_TRIANGLES, geoVertices, numVertices, sizeof(Geometry::Vertex),
		indices, numIndices, sizeof(uint32_t), GL_UNSIGNED_INT);

	/* position stream */
	gSphere.setVertexAttribute(Model::eATTRIB_POSITION, 3, GL_FLOAT, offsetof(Geometry::Vertex, x));

	/* texture coordinate stream */
	gSphere.setVertexAttribute(Model::eATTRIB_TEXCOORD, 2, GL_FLOAT, offsetof(Geometry::Vertex, s));

	/* bone IDs stream */
	gSphere.setVertexAttribute(Model::eATTRIB_BONE_ID, 4, GL_UNSIGNED_BYTE, offsetof(Geometry::Vertex, bones));

	/* normal stream */
	gSphere.setVertexAttribute(Model::eATTRIB_NORMAL, 3, GL_FLOAT, offsetof(Geometry::Vertex, nx));

	/* tangent stream */
	gSphere.setVertexAttribute(Model::eATTRIB_TANGENT, 3, GL_FLOAT, offsetof(Geometry::Vertex, tx));

	/* bone weight stream  */
	gSphere.setVertexAttribute(Model::eATTRIB_BONE_WEIGHT, 4, GL_FLOAT, offsetof(Geometry::Vertex, weights));

	delete[] geoVertices;
	delete[] vertices;
	delete[] indices;

	mShader.use();

	gl( ActiveTexture(GL_TEXTURE0) );
	mShader.setUniformVal("uPassType", pass);
	
	glm::mat4 viewMat;
	camera->getMatrix(viewMat);

	mShader.setUniformVal("uViewMat", viewMat);
	mShader.setUniformVal("uProjMat", camera->getFrustum().getProjMatrix());


	gl( Disable(GL_BLEND) );
	gl( Enable(GL_DEPTH_TEST) );
	gl( Enable(GL_CULL_FACE) );
	glDisable(GL_CULL_FACE);
	for(Scene::ModelledActorSet::const_iterator iter=scene->getModelledActors().cbegin(); iter!=scene->getModelledActors().cend(); iter++){
		render(scene, *iter, camera, pass);
	}
#endif
}

void ModelRenderer::render(Scene* scene, ModelledActor* actor, math::Camera* camera, PassType pass){
	if(!actor->getModel()){
		return;
	}

	// Upload skinning matrices
	if(actor->getAnimationPlayer()){
		mShader.setUniformVal("uBones",
			actor->getAnimationPlayer()->getBoneMatrices(),
			actor->getAnimationPlayer()->getNumBones()
		);
	}
	
	// Upload model matrix
	glm::mat4 modelMat;
	actor->getTransform().getMatrix(modelMat);
	mShader.setUniformVal("uModelMat", modelMat);

	// TODO materials, normal maps, blending not yet implemented
	actor->getModel()->getBatch().bind();

	for(Model::GeometrySkin::MeshList::iterator i=actor->getSkin()->getMeshList().begin(); i!=actor->getSkin()->getMeshList().end(); i++){
		// Use the material if the mesh has one
		Texture2D* texture = i->texture;

		if(texture != NULL){
			texture->bind();
		}
		else{
			// Use the default one if none is provided
			mInvalidTexture.bind();
		}

		// Render the geometry
		i->geometry->render();
	}
}

} // </wt>
