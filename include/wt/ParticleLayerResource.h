#ifndef WT_PARTICLELAYERRESOURCE_H
#define WT_PARTICLELAYERRESOURCE_H


#include "wt/stdafx.h"

#include "wt/AResourceManager.h"
#include "wt/Color.h"
#include "wt/Texture2D.h"

namespace wt{

class ParticleEffectResource;

class ParticleLayerResource{
friend class ParticleEffectResource;

public:
	struct LayerDesc{
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

		LayerDesc();
	};

	LayerDesc& getDesc();

	const LayerDesc& getDesc() const;

	ParticleEffectResource* getParent() const;

	const String& getName() const;

protected:

	/** Can only be instantiated as a part of ParticleEffect object */
	ParticleLayerResource(ParticleEffectResource* parent, const String& name);

	/** For renaming purposes */
	void setName(const String& name);

private:
	/** Prevent copy constructors */
	ParticleLayerResource(const ParticleLayerResource&);

	LayerDesc mDesc;

	String mName;

	ParticleEffectResource* mParent;
}; // </ParticleLayerResource>


}; // </wt>

#endif // </WT_PARTICLELAYERRESOURCE_H>