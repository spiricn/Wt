#include "wt/stdafx.h"

#include "wt/ModelRenderer.h"
#include "wt/UniformBufferObject.h"

#define  TD_TRACE_TAG "ModelRenderer"

namespace wt
{

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
	Utils::makeCheckboard(bfr, 100, 100, 5, 5, Color(246/255.0, 2/255.0, 134/255.0), Color::black());
	mInvalidTexture.create();
	mInvalidTexture.setData(100, 100, GL_RGB, GL_RGB, (const GLbyte*)bfr.getData());
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

	for(Scene::ModelledActorSet::const_iterator iter=scene->getModelledActors().cbegin(); iter!=scene->getModelledActors().cend(); iter++){
		render(scene, *iter, camera, pass);
	}
}

gl::ShaderProgram* ModelRenderer::setupSceneLighting(){
	return &mShader;
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
