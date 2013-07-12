#ifndef WT_PARTICLEENGINE_H
#define WT_PARTICLEENGINE_H


#include "wt/stdafx.h"

#include "wt/GlShaderProgram.h"
#include "wt/GLBatch.h"
#include "wt/Camera.h"
#include "wt/Frustum.h"
#include "wt/Texture2D.h"

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

	void create(const String& userProgPath);

	void setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj);
};

struct Particle{
	float x,y,z; // vec3
	float r,g,b; // vec3 
	float size; // float
	float life, maxLife; // vec2
	float vx,vy,vz; // vec3
};


class ParticleEmitter;

class ParticleEffect{
friend class ParticleEmitter;

protected:
	Uint8 mCurrBatch;
	Gl::Batch mBatches[2];
	Texture2D* mParticleTexture;
	Uint32 mNumParticles;
	ParticleShader mShader;
	bool mEnabled;

public:
	ParticleEffect() : mEnabled(true){
	}

	template<typename T>
	ParticleEffect* setParam(const String& name, const T& value){
		mShader.use();
		mShader.setUniformVal(name, value);

		return this;
	}

	void setEnable(bool enabled){
		mEnabled = enabled;
	}

	bool isEnabled() const{
		return mEnabled;
	}
};


class ParticleEmitter{
private:

	typedef std::vector<ParticleEffect*> EffectList;

	EffectList mEffects;

public:
	ParticleEmitter();

	ParticleEffect* create(Uint32 numParticles, Texture2D* particleTexture, const String& userProg);
	
	void update(float dt);

	void render(math::Camera* camera, const math::Frustum* frustum);


}; // </ParticleEmitter>



}; // </wt>

#endif
