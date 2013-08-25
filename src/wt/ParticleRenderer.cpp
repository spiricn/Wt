#include "wt/stdafx.h"

#include "wt/ParticleRenderer.h"

#define  TD_TRACE_TAG "ParticleRenderer"

namespace wt{

ParticleRenderer::ParticleRenderer(){
}

bool ParticleRenderer::isDeferred() const{
	return false;
}

void ParticleRenderer::create(){
	// Compile rendering shader
	LOGV("Compiling particle shader..");

	mParticleRenderShader.createFromFiles("shaders/particle_render.vp", "shaders/particle.fp");

	mParticleRenderShader.bindAttribLocation(ParticleCalculationShader::eATTR_TYPE, "inType");
	mParticleRenderShader.bindAttribLocation(ParticleCalculationShader::eATTR_POSITION, "inPosition");
	mParticleRenderShader.bindAttribLocation(ParticleCalculationShader::eATTR_LIFE, "inLife");
	mParticleRenderShader.bindAttribLocation(ParticleCalculationShader::eATTR_VELOCITY, "inVelocity");
	mParticleRenderShader.bindAttribLocation(ParticleCalculationShader::eATTR_SIZE, "inSize");
	mParticleRenderShader.bindAttribLocation(ParticleCalculationShader::eATTR_MAX_LIFE, "inMaxLife");

	mParticleRenderShader.link();

	// Compile calculation shader
	mParticleCalcShader.create();

	// Generate a random texture used for calculations
	mRandomTexture.create();

	const uint32_t w = 200;
	const uint32_t h = 200;


	Buffer<float> bfr(w*h*3);


	for(int i=0; i<w*h*3; i++){
		bfr[i] = math::random();
	}

	mRandomTexture.setData(w, h, GL_RGB, GL_RGB, (const GLbyte*)bfr.getData(), GL_FLOAT);
}

void ParticleRenderer::render(Scene* scene, math::Camera* camera, PassType pass){
	if(pass != ePASS_NORMAL){
		return;
	}

	for(Scene::ParticleEffectSet::const_iterator iter=scene->getParticleEffects().cbegin(); iter!=scene->getParticleEffects().cend(); iter++){
		// TODO fix this
		ParticleEffect* effect = const_cast<ParticleEffect*>(*iter);

		glm::mat4 modelMat;
		effect->getTransformable()->getTransformMatrix(modelMat);


		for(ParticleEffect::LayerMap::iterator i=effect->getLayerMap().begin(); i!=effect->getLayerMap().end(); i++){
			ParticleLayer* layer = i->second;

			// Update particles (TODO move this elsewhere)
			mParticleCalcShader.use();

			// Upload uniforms
			//mParticleCalcShader.setUniformVal("uPosition", effect->getTransform().getPosition());
			mParticleCalcShader.setUniformVal("uDt", effect->getTimeDelta());
			mParticleCalcShader.setUniformVal("uSeed", math::random(0, 200));

			mParticleCalcShader.setUniformVal("uMaxLife", layer->getLayerResource()->getDesc().maxLife);
			mParticleCalcShader.setUniformVal("uMinLife", layer->getLayerResource()->getDesc().minLife);

			mParticleCalcShader.setUniformVal("uMaxSize", layer->getLayerResource()->getDesc().maxSize);
			mParticleCalcShader.setUniformVal("uMinSize", layer->getLayerResource()->getDesc().minSize);

			mParticleCalcShader.setUniformVal("uLocalVelocity", layer->getLayerResource()->getDesc().localVelocity);
			mParticleCalcShader.setUniformVal("uRandomVelocity", layer->getLayerResource()->getDesc().randomVelocity);

			mParticleCalcShader.setUniformVal("uEmissionRate", layer->getLayerResource()->getDesc().emissionRate);

			mParticleCalcShader.setUniformVal("uEmissionVolume", layer->getLayerResource()->getDesc().emissionVolume);

			mParticleCalcShader.setUniformVal("uWorldSpaceSim", layer->getLayerResource()->getDesc().simulateInWorldSpace);

			if(layer->getLayerResource()->getDesc().simulateInWorldSpace){
				mParticleCalcShader.setUniformVal("uModelMat", modelMat*layer->getLayerResource()->getDesc().transform);
			}

			gl( ActiveTexture(GL_TEXTURE0) );
			mRandomTexture.bind();

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

			mParticleCalcShader.setUniformVal("uRandomTexture", 0);

			// WARNING: do NOT enable this sooner, it may cause some serious problems..
			glEnable(GL_RASTERIZER_DISCARD);
			layer->update();
			glDisable(GL_RASTERIZER_DISCARD);
		}	

		{
			for(ParticleEffect::LayerMap::iterator i=effect->getLayerMap().begin(); i!=effect->getLayerMap().end(); i++){
				ParticleLayer* layer = i->second;

				glm::mat4 view;
				camera->getCameraMatrix(view);

				// Render particles
				mParticleRenderShader.use();
				mParticleRenderShader.setUniformVal("uParticleTexture", 0);
				mParticleRenderShader.setUniformVal("uWorldSpaceSim", layer->getLayerResource()->getDesc().simulateInWorldSpace);

				if(!layer->getLayerResource()->getDesc().simulateInWorldSpace){
					mParticleRenderShader.setUniformVal("uModelMat", modelMat*layer->getLayerResource()->getDesc().transform);
				}

				glm::vec3 eyePos;
				camera->getTranslation(eyePos);

				mParticleRenderShader.setUniformVal("uCamPos", eyePos);
				mParticleRenderShader.setUniformVal("uSizeGrow", layer->getLayerResource()->getDesc().sizeGrow);
				mParticleRenderShader.setUniformVal("uModelViewMat", view);
				mParticleRenderShader.setUniformVal("uProjMat", camera->getProjectionMatrix());

				for(int i=0; i<ParticleLayerResource::LayerDesc::kMAX_COLORS; i++){
					char name[64];
					sprintf(name, "uColorAnimation[%d]", i);
					mParticleRenderShader.setUniformVal(name, layer->getLayerResource()->getDesc().colorAnimation[i]);
				}

				// Setup OpenGL
				gl( Enable(GL_BLEND) );
				gl( BlendEquation(GL_FUNC_ADD) );
				gl( BlendFunc(GL_SRC_ALPHA, GL_ONE) );
				gl( DepthMask(false) );


				// Texture
				gl( ActiveTexture(GL_TEXTURE0) );
				if(layer->getLayerResource()->getDesc().texture){
					layer->getLayerResource()->getDesc().texture->bind();
				}
				else{
					glBindTexture(GL_TEXTURE_2D, 0);
				}

				layer->render();
			}
		}
	}

	gl( DepthMask(true) );
}

}; // </wt>
