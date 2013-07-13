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
		POSITION,
		COLOR,
		LIFE,
		SIZE,
		VELOCITY,
	};

	void create(const String& userProgPath){
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

	void setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj){
		setUniformVal("uModelViewMat", modelView);
		setUniformVal("uProjMat", proj);
	}
};


class ParticleEffect : public ASceneActor{
public:
	
	struct Particle{
		float x,y,z; // vec3
		float r,g,b; // vec3 
		float size; // float
		float life, maxLife; // vec2
		float vx,vy,vz; // vec3
	};

	uint8_t mCurrBatch;
	Gl::Batch mBatches[2];
	Texture2D* mParticleTexture;
	uint32_t mNumParticles;
	
	bool mEnabled;


	ParticleShader mShader;

	ParticleEffect(Scene* parent, uint32_t id, const String& name="") : mEnabled(true), ASceneActor(parent, ASceneActor::eTYPE_PARTICLE_EFFECT, id, name){
	}

	void setEnable(bool enabled){
		mEnabled = enabled;
	}

	bool isEnabled() const{
		return mEnabled;
	}

	void update(float dt){
		mShader.use();
		mShader.setUniformVal("uDt", dt);
	}

	void create(uint32_t numParticles, Texture2D* particleTexture, const String& userProg){
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);

		
		mCurrBatch = 0;
		mShader.create(userProg);
		mNumParticles = numParticles;
		mParticleTexture = particleTexture;

		Particle* particles = new Particle[numParticles];
		memset(particles, 0x00, sizeof(Particle)*numParticles);
		uint32_t* indices = new uint32_t[numParticles];

		for(uint32_t i=0; i<numParticles; i++){
			indices[i] = i;
			Particle& p = particles[i];
			p.life = 1.0f;
			p.x = math::random()*100;
			p.y = math::random()*100;
			p.z = math::random()*100;

		}

		for(uint32_t i=0; i<2; i++){
			mBatches[i].create(i==0?particles:0, numParticles, sizeof(Particle),
				indices, numParticles, sizeof(uint32_t),
				GL_POINTS);

			// position
			mBatches[i].setVertexAttribute(
				ParticleShader::POSITION, 3, GL_FLOAT, offsetof(Particle, x));
			// color
			mBatches[i].setVertexAttribute(
				ParticleShader::COLOR, 3, GL_FLOAT, offsetof(Particle, r));
			// life/maxlife
			mBatches[i].setVertexAttribute(
				ParticleShader::LIFE, 2, GL_FLOAT, offsetof(Particle, life));
			// velocity
			mBatches[i].setVertexAttribute(
				ParticleShader::VELOCITY, 3, GL_FLOAT, offsetof(Particle, vx));
			// size
			mBatches[i].setVertexAttribute(
				ParticleShader::SIZE, 1, GL_FLOAT, offsetof(Particle, size));
		}

		delete[] particles;
		delete[] indices;
	}
};




}; // </wt>

#endif // </WT_PARTICLEEFFECT_H>