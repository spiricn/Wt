#include "wt/stdafx.h"

#include "wt/ModelRenderer.h"
#include "wt/UniformBufferObject.h"
#include "wt/FrameBuffer.h"
#include "wt/IcosphereBuilder.h"

#define  TD_TRACE_TAG "ModelRenderer"

#include "wt/Renderer.h"

#define DEFERRED_SHADING

#include "wt/DeferredRenderer.h"

namespace wt
{


gl::Batch gSphere;


ModelRenderer::ModelRenderer(){
}

bool ModelRenderer::isDeferred() const{
	return true;
}

void ModelRenderer::create(){
	// Compile the shader
	LOGV("Compiling model shader...");

#ifndef DEFERRED_SHADING

	mShader.createFromFiles("shaders\\model.vp",
		"shaders\\model.fp");

	mShader.bindAttribLocation(Model::eATTRIB_POSITION, "inVertex");
	mShader.bindAttribLocation(Model::eATTRIB_TEXCOORD, "inTexCoord");
	mShader.bindAttribLocation(Model::eATTRIB_NORMAL, "inNormals");
	mShader.bindAttribLocation(Model::eATTRIB_TANGENT, "inTangent");
	mShader.bindAttribLocation(Model::eATTRIB_BONE_ID, "inBoneIds");
	mShader.bindAttribLocation(Model::eATTRIB_BONE_WEIGHT, "inWeights");

#else
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
#endif

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

	mDeferredRender = new DeferredRender(1280, 720);
}

void ModelRenderer::onSceneLightingChanged(Scene* scene, Renderer* renderer){
#ifdef DEFERRED_SHADING
	renderer->setShaderLightUniforms(scene, *mDeferredRender->getLightShader(DeferredRender::eLIGHT_SHADER_DIRECTIONAL) );
	renderer->setShaderLightUniforms(scene, *mDeferredRender->getLightShader(DeferredRender::eLIGHT_SHADER_POINT) );
#else
	renderer->setShaderLightUniforms(scene, mShader);
#endif
}

void ModelRenderer::render(Scene* scene, math::Camera* camera, PassType pass){
#ifdef DEFERRED_SHADING

	
	glm::mat4 viewMat;
	camera->getMatrix(viewMat);

	


	// Setup the shader
	mShader.use();
	mShader.setUniformVal("uViewMat", viewMat);
	mShader.setUniformVal("uProjMat", camera->getFrustum().getProjMatrix());
	glActiveTexture(GL_TEXTURE0);

	// Render the geometry
	for(Scene::ModelledActorSet::const_iterator iter=scene->getModelledActors().cbegin(); iter!=scene->getModelledActors().cend(); iter++){
		const ModelledActor* actor = *iter;
		mShader.use();

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

	

	//mDeferredRender->mDepthTexture.dump("depth.bmp");

#if 0
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
