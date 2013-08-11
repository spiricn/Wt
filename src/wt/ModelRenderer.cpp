#include "wt/stdafx.h"

#include "wt/ModelRenderer.h"
#include "wt/UniformBufferObject.h"
#include "wt/FrameBuffer.h"
#include "wt/IcosphereBuilder.h"

#define  TD_TRACE_TAG "ModelRenderer"

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
		eGTEX_TEXCOORD,
		eGTEX_DEPTH,
		eGTEX_MAX
	};

	enum LightPassShader{
		eLIGHT_SHADER_NONE=-1,
		eLIGHT_SHADER_DIRECTIONAL = 0,
		eLIGHT_SHADER_POINT,
		eLIGHT_SHADER_SPOT,
		eLIGHT_SHADER_MAX,
	};

	Texture2D mTextures[eGTEX_MAX];
	gl::FrameBuffer mFrameBuffer;
	uint32_t mWidth, mHeight;
	gl::ShaderProgram mShader;

	gl::ShaderProgram mLightShaders[eLIGHT_SHADER_MAX];

	gl::Batch mQuadBatch;
	gl::Batch mSphereBatch;

	void bindTextures(){
		for(uint32_t i=0; i<eGTEX_MAX-1; i++){
			gl(ActiveTexture(GL_TEXTURE0 + i));
			mTextures[i].bind();
		}
	}

	DeferredRender() : mWidth(1280), mHeight(720){
		// Framebuffer
		mFrameBuffer.create();
		mFrameBuffer.bindDraw();

		for(uint32_t i=0; i<eGTEX_MAX; i++){
			mTextures[i].create();

			// Disable unecessary interpolation
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			if(i == eGTEX_DEPTH){
				mTextures[i].setData(mWidth, mHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, NULL, GL_FLOAT);
				mFrameBuffer.addAttachment(GL_DEPTH_ATTACHMENT, mTextures + i);
			}
			else{
				mTextures[i].setData(mWidth, mHeight, GL_RGB, GL_RGB32F, NULL, GL_FLOAT);
				mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0 + i, mTextures + i);
			}
		}

		const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 }; 
		gl( DrawBuffers(4, drawBuffers) );

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

			IcosphereBuilder(&vertices, &indices, &numVertices, &numIndices, 2);

			// TODO if IcosphereBuilder::Vertex structure changes this will break
			mSphereBatch.create(GL_TRIANGLES, vertices, numVertices, sizeof(IcosphereBuilder::Vertex),
				indices, numIndices, sizeof(uint32_t), GL_UNSIGNED_INT);

			mSphereBatch.setVertexAttribute(0, 3, GL_FLOAT, offsetof(IcosphereBuilder::Vertex, x));
		}
	}

	void setReadBuffer(TextureType texture){
		gl( ReadBuffer(GL_COLOR_ATTACHMENT0 + texture) );
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

	void render(Scene* scene, math::Camera* camera){
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		bindTextures();

		for(int i=0; i<eLIGHT_SHADER_MAX; i++){
			gl::ShaderProgram& shader = mLightShaders[i];

			shader.use();

			if(i == eLIGHT_SHADER_DIRECTIONAL){
				mQuadBatch.render();
			}
			else if(i == eLIGHT_SHADER_POINT){
				break;
				for(uint32_t i=0; i<scene->getNumPointLights(); i++){
					PointLight light;
					scene->getPointLight(i, light);

					if(!light.mActive){
						continue;
					}

					math::Transform tf;
					tf.setPosition(light.mPosition);
					tf.setScale(light.calculateBoundingSphere(), light.calculateBoundingSphere(), light.calculateBoundingSphere());

					glm::mat4 model;
					tf.getMatrix(model);

					glm::mat4 view;
					camera->getMatrix(view);

					shader.setUniformVal("uViewMat", view);
					shader.setUniformVal("uProjMat", camera->getFrustum().getProjMatrix());
					shader.setUniformVal("uModelMat", model);

					mSphereBatch.render();
				}
				break;
			}
		}
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

	IcosphereBuilder::Vertex* vertices;
	uint32_t* indices;
	uint32_t numIndices, numVertices;

	IcosphereBuilder(&vertices, &indices, &numVertices, &numIndices, 3);

	Geometry::Vertex* geoVertices = new Geometry::Vertex[numVertices];
	//memset(geoVertices, 0x00, sizeof(Geometry::Vertex)*numVertices);

	for(uint32_t i=0; i<numVertices; i++){
		geoVertices[i].x = vertices[i].x;
		geoVertices[i].y = vertices[i].y;
		geoVertices[i].z = vertices[i].z;

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

	// Create a default (invalid) texture 
	Buffer<unsigned char> bfr;
	utils::makeCheckboard(bfr, 100, 100, 5, 5, Color(246/255.0, 2/255.0, 134/255.0), Color::black());
	mInvalidTexture.create();
	mInvalidTexture.setData(100, 100, GL_RGB, GL_RGB, (const GLbyte*)bfr.getData());

	mDeferredRender = new DeferredRender;
}

void ModelRenderer::render(Scene* scene, math::Camera* camera, PassType pass){
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

#if 1

	mDeferredRender->mFrameBuffer.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDeferredRender->mShader.use();
	mDeferredRender->mShader.setUniformVal("uViewMat", viewMat);
	mDeferredRender->mShader.setUniformVal("uProjMat", camera->getFrustum().getProjMatrix());

	// Geometry pass
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

		

		// TODO materials, normal maps, blending not yet implemented

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

	//// Debug blit
	gl::FrameBuffer::unbind();
	////mDeferredRender->debugBlit(glm::vec2(1280, 720));
	//
	GLint doubleBuffered; 
	gl( GetIntegerv(GL_DOUBLEBUFFER, &doubleBuffered) );

	GLenum defaultBfrs[1];
	defaultBfrs[0] = doubleBuffered? GL_BACK : GL_FRONT;

	gl( DrawBuffers(1, defaultBfrs) );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	mDeferredRender->render(scene, camera);


#else
	glDisable(GL_CULL_FACE);
	for(Scene::ModelledActorSet::const_iterator iter=scene->getModelledActors().cbegin(); iter!=scene->getModelledActors().cend(); iter++){
		render(scene, *iter, camera, pass);
	}
#endif
}

gl::ShaderProgram* ModelRenderer::setupSceneLighting(){
	//return &mShader;
	return &mDeferredRender->mLightShaders[DeferredRender::eLIGHT_SHADER_POINT];
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

	actor->getModel()->getBatch().bind();

	// TODO materials, normal maps, blending not yet implemented
	
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
