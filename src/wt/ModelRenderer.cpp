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
	mShader.createFromFiles("shaders/model_deferred.vp",
		"shaders/model_deferred.fp");
		
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
	mShader.setUniformVal("uNormalMap", 1);

	// Uniform buffer object example (TODO use this)
#if 0
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
	utils::makeCheckboard(bfr, 100, 100, 5, 5, Color(246/255.0, 2/255.0, 134/255.0), Color::Black());
	mInvalidTexture.create();
	mInvalidTexture.setData(100, 100, GL_RGB, GL_RGB, (const GLbyte*)bfr.getData());

}

void ModelRenderer::render(Scene* scene, math::Camera* camera, PassType pass, Texture2D* shadowMap){
	glm::mat4 viewMat;
	camera->getCameraMatrix(viewMat);

	// Setup the shader
	mShader.use();
	mShader.setUniformVal("uViewMat", viewMat);
	mShader.setUniformVal("uProjMat", camera->getProjectionMatrix());
	glActiveTexture(GL_TEXTURE0);


	const Scene::ActorSet& actorSet = scene->getActorSet(ASceneActor::eTYPE_MODELLED);

	// Render the geometry
	for(Scene::ActorSet::const_iterator iter=actorSet.begin(); iter!=actorSet.end(); iter++){
		const ModelledActor* actor = dynamic_cast<const ModelledActor*>(*iter);

		if(actor->getModel() == NULL || !actor->isVisible()){
			// Nothing to render
			continue;
		}

		// Upload skinning matrices
		if(actor->getAnimationPlayer()){
			mShader.setUniformVal("uBones",
				actor->getBoneMatrices().getData(),
				actor->getBoneMatrices().getCapacity()
			);
		}
	
		// Upload model matrix
		glm::mat4 modelMat;
		((ModelledActor*)actor)->getTransformable()->getTransformMatrix(modelMat);
		mShader.setUniformVal("uModelMat", modelMat);

		for(ModelSkin::MeshList::iterator i=actor->getSkin()->getMeshListBeg(); i!=actor->getSkin()->getMeshListEnd(); i++){
			ModelSkin::Mesh* mesh = *i;

			// Setup mesh texture
			gl(ActiveTexture(GL_TEXTURE0));
			if(mesh->texture != NULL){
				mesh->texture->bind();
			}
			else{
				// Use the default one if none is provided
				mInvalidTexture.bind();
			}

			// Handle normal mapping
			if(mesh->normalMap){
				// TODO move to unit 0
				gl(ActiveTexture(GL_TEXTURE1));
				mShader.setUniformVal("uUseNormalMap", 1);
				mesh->normalMap->bind();
			}
			else{
				mShader.setUniformVal("uUseNormalMap", 0);
			}
			
			// Handle face culling
			CullMode cull = mesh->material.getCullMode();
			if(cull != eCULL_NONE){
				gl( Enable(GL_CULL_FACE) );
				gl( CullFace( cull == eCULL_FRONT ? GL_FRONT : cull == eCULL_BACK ? GL_BACK : GL_FRONT_AND_BACK ) );
			}
			else{
				gl( Disable(GL_CULL_FACE) );
			}

			// Alpha testing
			mShader.setUniformVal("uUseAlphaTest", mesh->material.isAlphaTested() ? 1 : 0);

			// Blending
#if 0
			// TODO
			if(mesh->material.getBlend()){
				gl( Enable(GL_BLEND) );
				gl( BlendEquation(GL_FUNC_ADD) );
				gl( BlendFunc(GL_SRC_ALPHA, GL_ONE) );
				gl( DepthMask(false) );
			}
			else{
				gl( Disable(GL_BLEND) );
			}
#endif

			// Render the geometry
			actor->getModel()->getBatch().bind();
			mesh->geometry->render();
		}
	}
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
	actor->getTransformable()->getTransformMatrix(modelMat);
	mShader.setUniformVal("uModelMat", modelMat);

	// TODO materials, normal maps, blending not yet implemented
	actor->getModel()->getBatch().bind();

	for(ModelSkin::MeshList::iterator i=actor->getSkin()->getMeshListBeg(); i!=actor->getSkin()->getMeshListEnd(); i++){
		ModelSkin::Mesh* mesh = *i;

		// Use the material if the mesh has one
		Texture2D* texture = mesh->texture;

		if(texture != NULL){
			texture->bind();
		}
		else{
			// Use the default one if none is provided
			mInvalidTexture.bind();
		}

		// Render the geometry
		mesh->geometry->render();
	}
}

} // </wt>
