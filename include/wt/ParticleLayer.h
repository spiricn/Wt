#ifndef WT_PARTICLELAYER_H
#define WT_PARTICLELAYER_H

#include "wt/Texture2D.h"
#include "wt/Color.h"
#include "wt/GLBatch.h"
#include "wt/GLShaderProgram.h"

namespace wt{


class ParticleRenderShader : public Gl::ShaderProgram{
public:
	void create();

	void setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj);
}; 

class ParticleCalculationShader : public Gl::ShaderProgram{
public:
	enum Attribute{
		eATTR_TYPE = 0,
		eATTR_POSITION = 1,
		eATTR_LIFE = 2,
		eATTR_VELOCITY = 3,
		eATTR_SIZE = 4,
		eATTR_MAX_LIFE = 5
	};

	void create();
};


class ParticleEffect;

class ParticleLayer{
public:
#pragma pack(push)
#pragma pack(1)

	struct Particle{
		// float type
		float type;

		// vec3 position
		float x,y,z;
		
		// float life
		float life;

		// vec3 velocity
		float vx,vy,vz; 

		// float size
		float size;

		// float maxLife
		float maxLife;
	};
#pragma pack(pop)

	struct EffectDesc{
		static const uint32_t kMAX_COLORS = 4;

		/** Starting velocity each particle is given on emission */
		glm::vec3 localVelocity;

		/**
		 * Randomization velocity factor. Each particle's velocity is randomized at its emssion with
		 * this vector:
		 *	vec3 initialVelocity = localVelocity + ( -randomVelocity + 2 * rand() * randomVelocity )
		 */
		glm::vec3 randomVelocity;

		/**
		 * Bounding volume of the particle emission
		 * Each particle is given a starting point at its emission
		 * vec3 initialPosition = emitterPosition + ( -emissionVolume + 2 * rand() * emissionVolume )
		 */
		glm::vec3 emissionVolume;

		/** Minimum and maximum life each particle can have (random number between these two) */
		float minLife, maxLife;

		/** Minimum and maximum size each particle can have */
		float minSize, maxSize;

		/** Size grow factor. During its lifetime a particle can grow/shrink acording to this paramtere */
		float sizeGrow;

		/** How many particles are emitted per second */
		float emissionRate;

		/** Maximum number of particles emitted at one time */
		uint32_t particleNumber;

		/** Texture used for the particles */
		Texture2D* texture;

		/** Color animation of a single particle. During its lifetime the particle will cycle through all these colors */
		Color colorAnimation[kMAX_COLORS];

		EffectDesc();
	};

	ParticleLayer(ParticleEffect* parent);

	~ParticleLayer();

	ParticleEffect* getParent() const;

	const EffectDesc& getDesc() const;

	void create(const EffectDesc& desc);

	void update();

	void render();

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst);

	void deserialize(AResourceSystem* assets, const LuaPlus::LuaObject& src);
private:
	ParticleEffect* mParent;
	uint8_t mCurrBatch;
	Gl::Batch mBatches[2];
	Texture2D* mParticleTexture;
	uint32_t mPrimittivesWritten;
	EffectDesc mDesc;
	GLuint mQuery;
}; // </ParticleLayer>

}; // </wt>

#endif // </WT_PARTICLELAYER_H>