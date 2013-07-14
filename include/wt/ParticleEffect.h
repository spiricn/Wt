#pragma once

#ifndef WT_PARTICLEEFFECT_H
#define WT_PARTICLEEFFECT_H

#include "wt/Texture2D.h"
#include "wt/GLBatch.h"
#include "wt/GLShaderProgram.h"
#include "wt/Math.h"
#include "wt/ASceneActor.h"

namespace wt{

class ParticleShader : public Gl::ShaderProgram{
public:
	enum Attribute{
		eATTR_POSITION = 0,
		eATTR_COLOR = 1,
		eATTR_LIFE = 2,
		eATTR_VELOCITY = 3,
	};

	void create(){
		Gl::ShaderProgram::createFromFiles("shaders/particle.vp", "shaders/particle.fp");

		bindAttribLocation(eATTR_POSITION, "inPosition");
		bindAttribLocation(eATTR_COLOR, "inColor");
		bindAttribLocation(eATTR_LIFE, "inLife");
		bindAttribLocation(eATTR_VELOCITY, "inVelocity");

		setTransformFeedbackVaryings(4,
			"outPosition", "outColor", "outLife", "outVelocity"
			);

		link();
	}

	void setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj){
		setUniformVal("uModelViewMat", modelView);
		setUniformVal("uProjMat", proj);
	}
};


class ParticleEffect : public ASceneActor{
public:
	
	struct Particle{
		// vec3 position
		float x,y,z;

		// vec3 color
		float r,g,b;
		
		// float life
		float life;

		// vec3 velocity
		float vx,vy,vz; 
	};

	/**
	per effect attributes:
		color
		texture
		particleNumber
		effectDuration

	per particle attributes:
		currentLife
		totalLife
		position
	*/
	struct EffectDesc{
		float duration;
		float size;
		float life;		
		float velocity;
		Color color;
		Texture2D* texture;
		uint32_t maxNumber;
	};

	ParticleEffect(Scene* parent, uint32_t id, const String& name="") : mEnabled(true), ASceneActor(parent, ASceneActor::eTYPE_PARTICLE_EFFECT, id, name),
		mTimeDelta(0.0f){
	}

	void setEnable(bool enabled){
		mEnabled = enabled;
	}

	bool isEnabled() const{
		return mEnabled;
	}

	void update(float dt){
		mTimeDelta = dt;
	}

	void create(const EffectDesc& desc){
		mDesc = desc;
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);

		mCurrBatch = 0;

		// Initialize particles
		Particle* particles = new Particle[desc.maxNumber];
		memset(particles, 0x00, sizeof(Particle)*desc.maxNumber);

		uint32_t* indices = new uint32_t[desc.maxNumber];
		for(uint32_t i=0; i<desc.maxNumber; i++){
			indices[i] = i;
		}

		for(uint32_t i=0; i<desc.maxNumber; i++){
			particles[i].life = 0;
			
			// Not the actual position but rather a seed for initial directoin
			particles[i].x = math::random() * 1000;
			particles[i].y = math::random() * 1000;
			particles[i].z = math::random() * 1000;
		}

		// Create two batches for double-buffered transform feedback
		for(uint32_t i=0; i<2; i++){
			mBatches[i].create(i==0?particles:0, desc.maxNumber, sizeof(Particle),
				indices, desc.maxNumber, sizeof(uint32_t),
				GL_POINTS);

			// position
			mBatches[i].setVertexAttribute(
				ParticleShader::eATTR_POSITION, 3, GL_FLOAT, offsetof(Particle, x));
			// color
			mBatches[i].setVertexAttribute(
				ParticleShader::eATTR_COLOR, 3, GL_FLOAT, offsetof(Particle, r));
			// life
			mBatches[i].setVertexAttribute(
				ParticleShader::eATTR_LIFE, 1, GL_FLOAT, offsetof(Particle, life));
			// velocity
			mBatches[i].setVertexAttribute(
				ParticleShader::eATTR_VELOCITY, 3, GL_FLOAT, offsetof(Particle, vx));
		}

		delete[] particles;
		delete[] indices;
	}

	const EffectDesc& getDesc() const{
		return mDesc;
	}

	void render(){
		mBatches[mCurrBatch].render( &mBatches[(mCurrBatch+1)%2].getVertexBuffer() );
		mCurrBatch = (mCurrBatch+1)%2;
	}

	float getTimeDelta() const{
		return mTimeDelta;
	}

private:
	EffectDesc mDesc;

	uint8_t mCurrBatch;
	Gl::Batch mBatches[2];
	Texture2D* mParticleTexture;
	uint32_t mNumParticles;
	
	bool mEnabled;
	float mTimeDelta;


}; // </ParticleEffect>




}; // </wt>

#endif // </WT_PARTICLEEFFECT_H>