#ifndef _WT_H_SKY_DOME
#define _WT_H_SKY_DOME

#include "wt/stdafx.h"
#include "wt/GLShaderProgram.h"
#include "wt/Model.h"
#include "wt/Texture2D.h"
#include "wt/Frustum.h"

namespace wt{

class SkyDomeShader : public gl::ShaderProgram{
public:
	enum Streams{
		VERTEX = 0,
		TEXTURE_COORDS = 1,
		NORMALS = 2,
		TANGENT = 3,
		BONE_IDS = 4,
		BONE_WEIGHTS = 5
	};



	void create(){
		createFromFiles("shaders/skydome.vp", "shaders/skydome.fp");

		bindAttribLocation(VERTEX, "inPosition");
		bindAttribLocation(TEXTURE_COORDS, "inTexCoord");
		
		link();

		use();
		setUniformVal("uSkyTexture", 0);
		setUniformVal("uGlowTexture", 1);
	}

	void setModelViewProjMat(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj){
		use();

		setUniformVal("uModelMat", model);
		setUniformVal("uViewMat", view);
		setUniformVal("uProjMat", proj);
	}

}; // </SkyDomeShader >

class SkyDome{
public:
	enum DayPhase{
		DAWN,
		DAY,
		DUSK,
		NIGHT
	};

private:
	Geometry* mDomeMesh;
	SkyDomeShader mShader;
	Texture2D* mSkyTexture;
	Texture2D* mGlowTexture;
	Texture2D* mSunTexture;
	float mDayTime, mDayLength, mDomeScale;
	glm::vec3 mSunPos;

	gl::Batch mPlanetBatch;
	gl::ShaderProgram mPlanetShader;
	glm::vec4 mDayDesc;

public:

	SkyDome();

	void getLightIntensity(float* ambient, float* diffuse) const{
				
		// 0 = dawn
		// 0.25 = noon
		// 0.5 = dusk
		// 0.75 = midnight
		float pos = getCyclePos();

		//				0[dawn]	0.25[noon]	0.5[dusk]	0.75[midnight]
		// ambient		0.1		0.3			0.1			0.01
		// diffuse		0.0.1	0.5			0.0.1		0.0
		float ambientKeyframe[][2] = {{0.0, 0.2}, {0.25, 0.01}, {0.5, 0.2}, {0.75, 0.01},  {1.0, 0.2}};
		for(int i=0; i<4; i++){
			if(pos <= ambientKeyframe[i+1][0] ){
				// interpolate i -> i+1
				float t = (pos-ambientKeyframe[i][0]) / (ambientKeyframe[i+1][0]-ambientKeyframe[i][0]);
				*ambient = ambientKeyframe[i][1] + ( ambientKeyframe[i+1][1]-ambientKeyframe[i][1] ) * t;
				break;
			}
		}

		float diffuseKeyFrame[][2] = {{0.0, 0.0}, {0.25, 2.0}, {0.5, 1}, {0.75, 0.0},  {1.0, 0.0}};
		for(int i=0; i<4; i++){
			if(pos <= diffuseKeyFrame[i+1][0] ){
				// interpolate i -> i+1
				float t = (pos-diffuseKeyFrame[i][0]) / (diffuseKeyFrame[i+1][0]-diffuseKeyFrame[i][0]);
				*diffuse = diffuseKeyFrame[i][1] + ( diffuseKeyFrame[i+1][1]-diffuseKeyFrame[i][1] ) * t;
				break;
			}
		}
	}

	float getCyclePos() const;

	void create(Geometry* domeMesh, Texture2D* skyTexture,
		Texture2D* glowTexture, Texture2D* sunTexture);

	void update(float dt);

	const glm::vec3& getSunPos() const;

	void setTimeOfDay(float p);

	void render(math::Camera& camera, const math::Frustum& frustum);

}; // </SkyDome>



}; //</wt>

#endif