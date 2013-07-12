#include "wt/stdafx.h"

#include "wt/ParticleEngine.h"
#include "wt/Transform.h"

#define TD_TRACE_TAG "ParticleEngine"

namespace wt{

void ParticleShader::create(const String& userProgPath){
	String vertexBase;
	Utils::readFile("shaders/particle.vp", vertexBase);

	vertexBase.append("\n");

	String userProg;
	Utils::readFile(userProgPath, userProg);
	vertexBase.append(userProg);

	String fragmentProg;
	Utils::readFile("shaders/particle.fp", fragmentProg);

	createFromSources(vertexBase, fragmentProg);


	bindAttribLocation(POSITION, "inPosition");
	bindAttribLocation(COLOR, "inColor");
	bindAttribLocation(LIFE, "inLife");
	bindAttribLocation(VELOCITY, "inVelocity");
	bindAttribLocation(SIZE, "inPointSize");
		

	setTransformFeedbackVaryings(5,
		"outPosition", "outColor", "outPointSize", "outLife", "outVelocity"
		);

	link();
}

void ParticleShader::setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj){
	setUniformVal("uModelViewMat", modelView);
	setUniformVal("uProjMat", proj);

}

ParticleEmitter::ParticleEmitter(){
}

ParticleEffect* ParticleEmitter::create(Uint32 numParticles, Texture2D* particleTexture, const String& userProg){
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	ParticleEffect* effect = new ParticleEffect();

	effect->mCurrBatch = 0;
	effect->mShader.create(userProg);
	effect->mNumParticles = numParticles;
	effect->mParticleTexture = particleTexture;

	Particle* particles = new Particle[numParticles];
	memset(particles, 0x00, sizeof(Particle)*numParticles);
	Uint32* indices = new Uint32[numParticles];

	for(Uint32 i=0; i<numParticles; i++){
		indices[i] = i;
		Particle& p = particles[i];
		p.life = 1.0f;
		p.x = math::random()*100;
		p.y = math::random()*100;
		p.z = math::random()*100;

	/*	

		p.size = 2.0f + math::random();

		p.life = 0.0;
		p.maxLife =  3+math::random()*5;

		p.vx = -0.5 + math::random();
		p.vy = 2+math::random()*3;
		p.vz = -0.5 +math::random();*/


	}

	for(Uint32 i=0; i<2; i++){
		effect->mBatches[i].create(i==0?particles:0, numParticles, sizeof(Particle),
			indices, numParticles, sizeof(Uint32),
			GL_POINTS);

		// position
		effect->mBatches[i].setVertexAttribute(
			ParticleShader::POSITION, 3, GL_FLOAT, offsetof(Particle, x));
		// color
		effect->mBatches[i].setVertexAttribute(
			ParticleShader::COLOR, 3, GL_FLOAT, offsetof(Particle, r));
		// life/maxlife
		effect->mBatches[i].setVertexAttribute(
			ParticleShader::LIFE, 2, GL_FLOAT, offsetof(Particle, life));
		// velocity
		effect->mBatches[i].setVertexAttribute(
			ParticleShader::VELOCITY, 3, GL_FLOAT, offsetof(Particle, vx));
		// size
		effect->mBatches[i].setVertexAttribute(
			ParticleShader::SIZE, 1, GL_FLOAT, offsetof(Particle, size));
	}

	delete[] particles;
	delete[] indices;

	mEffects.push_back(effect);

	return effect;
}

float rainbowFactor=0.0f;

void ParticleEmitter::update(float dt){
	rainbowFactor = fmod(rainbowFactor+1/60.0f, 5.0f);

	for(Uint32 i=0; i<mEffects.size(); i++){
		mEffects[i]->mShader.use();
		mEffects[i]->mShader.setUniformVal("uDt", dt);
		mEffects[i]->mShader.setUniformVal("uRainbowFactor", rainbowFactor/5.0f);
	}

}

void ParticleEmitter::render(math::Camera* camera, const math::Frustum* frustum){
	gl( Enable(GL_BLEND) );

	gl( BlendEquation(GL_FUNC_ADD) );
	gl( BlendFunc(GL_SRC_ALPHA, GL_ONE) );
	gl( DepthMask(false) );

	for(Uint32 i=0; i<mEffects.size(); i++){
		if(!mEffects[i]->isEnabled()){
			continue;
		}

		mEffects[i]->mShader.use();

		// modelview
		glm::mat4 view;
		camera->getMatrix(view);

	
		mEffects[i]->mShader.setUniformVal("uCamPos", camera->getPosition());

		// texture
		gl( ActiveTexture(GL_TEXTURE0) );
		mEffects[i]->mShader.setUniformVal("uParticleTexture", 0);

		// modelview projection
		mEffects[i]->mShader.setModelViewProj(view, frustum->getProjMatrix());

		mEffects[i]->mShader.setUniformVal("uSeed", math::random(0, 1000));

		mEffects[i]->mParticleTexture->bind();
		mEffects[i]->mBatches[mEffects[i]->mCurrBatch].render( &mEffects[i]->mBatches[(mEffects[i]->mCurrBatch+1)%2].getVertexBuffer() );//;
		mEffects[i]->mCurrBatch = (mEffects[i]->mCurrBatch+1)%2;
	}

	gl( DepthMask(true) );
}

}; // </wt>
