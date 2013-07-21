#ifndef WT_PARTICLELAYER_H
#define WT_PARTICLELAYER_H

#include "wt/Texture2D.h"
#include "wt/Color.h"
#include "wt/GLBatch.h"
#include "wt/GLShaderProgram.h"
#include "wt/GLQuery.h"
#include "wt/ParticleLayerResource.h"

namespace wt{


class ParticleRenderShader : public gl::ShaderProgram{
public:
	void create();

	void setModelViewProj(const glm::mat4& modelView, const glm::mat4& proj);
}; 

class ParticleCalculationShader : public gl::ShaderProgram{
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
friend class ParticleEffect;

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

	~ParticleLayer();

	ParticleEffect* getParent() const;

	const String& getName() const;

	ParticleLayerResource* getLayerResource() const;

	void update();

	void render();

	void serialize(AResourceSystem* assets, LuaPlus::LuaObject& dst);

protected:
	/** Can only be instantiated as a part of ParticleEffect object */
	ParticleLayer(ParticleEffect* parent, const String& name, ParticleLayerResource* layerRsrc);

	/** For renaming purposes */
	void setName(const String& name);

private:
	void create(ParticleLayerResource* desc);

	ParticleEffect* mParent;
	uint8_t mCurrBatch;
	gl::Batch mBatches[2];
	Texture2D* mParticleTexture;
	uint32_t mPrimittivesWritten;
	ParticleLayerResource* mLayerRsrc;
	gl::Query mQuery;
	String mName;

	/** Copy constructor disabled */
	ParticleLayer(const ParticleLayer&);
}; // </ParticleLayer>

}; // </wt>

#endif // </WT_PARTICLELAYER_H>