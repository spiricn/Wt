#ifndef WT_SKYDOME_H
#define WT_SKYDOME_H

#include "wt/stdafx.h"
#include "wt/GLShaderProgram.h"
#include "wt/Model.h"
#include "wt/Texture2D.h"
#include "wt/ASceneActor.h"

namespace wt{


class SkyDomeShader;

class SkyDome : public ASceneActor{
public:
	enum DayPhase{
		ePHASE_INVALID = -1,
		ePHASE_DAWN = 0,
		ePHASE_DAY,
		ePHASE_DUSK,
		ePHASE_NIGHT,
		ePHASE_MAX
	}; // </DayPhase>

	SkyDome();

	~SkyDome();

	Color getFragmentColor(const glm::vec3& pos) const;

	void getLightIntensity(float* ambient, float* diffuse) const;

	float getCyclePos() const;

	float getCycleLength() const;

	void advance(float dt);

	void create(Geometry* domeMesh, Texture2D* skyTexture, Texture2D* glowTexture, Texture2D* sunTexture);

	void setPaused(bool state);

	void update(float dt);

	const glm::vec3& getSunPos() const;

	void setTimeOfDay(float p);

	void render(Camera& camera);

	ATransformable* getTransformable(void) const;

private:
	Geometry* mDomeMesh;
	SkyDomeShader* mShader;
	Texture2D* mSkyTexture;
	Texture2D* mGlowTexture;
	Texture2D* mSunTexture;
	bool mPaused;
	float mDayTime, mDayLength, mDomeScale;
	glm::vec3 mSunPos;

	gl::Batch mPlanetBatch;
	gl::ShaderProgram mPlanetShader;
	glm::vec4 mDayDesc;

	math::Transform mTransform;

}; // </SkyDome>


} //</wt>

#endif // </WT_SKYDOME_H>
