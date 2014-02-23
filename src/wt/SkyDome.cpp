#include "wt/stdafx.h"
#include "wt/SkyDome.h"
#include "wt/ShaderFactory.h"

#define TD_TRACE_TAG "SkyDome"

namespace wt
{

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
		ShaderFactory::createShader(*this, "skydome.vp", "skydome.fp");

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

SkyDome::SkyDome() : mDayTime(0.0f), mDayLength(30.0), mDomeScale(500.0f), ASceneActor(NULL, ASceneActor::eTYPE_CUSTOM, 0, ""), mPaused(false){
	mShader = new SkyDomeShader;
}

SkyDome::~SkyDome(){
	delete mShader;
}

ATransformable* SkyDome::getTransformable(void) const{
	return const_cast<math::Transform*>(&mTransform);
}

Color SkyDome::getFragmentColor(const glm::vec3& pos) const{
	// Code copied from the skydome.fp shader so it should be kept up-to-date

	// Normalized fragment position
	glm::vec3 fragPos = glm::normalize(pos); 

	// Normalized sun's position (moving around a normalized sphere)
    glm::vec3 sunPos = glm::normalize(mSunPos);

	//This dot product gives a large value near the sun, and a small one far away, and thus models a glow centered at the sun position
    float fragDistance = glm::dot(fragPos, sunPos); 

    // Look up the sky color and glow colors.

	glm::vec4 skyColor = mSkyTexture->sample(
		(sunPos.y + 1.0) / 2.0, // s - (sun vertical position moved to [0, 1] range from [-1 1]
		1-fragPos.y // t - (fragment's distance from the horizon
	).asVec4();

	glm::vec4 glowColor = mGlowTexture->sample(
		(sunPos.y + 1.0) / 2.0, // s
		1-fragDistance).asVec4() // t
		 ;

    // Combine the color and glow giving the pixel value.

	
	glm::vec3 skyRgb = skyColor.swizzle(glm::R, glm::G, glm::B);
	glm::vec3 glowRgb = glowColor.swizzle(glm::R, glm::G, glm::B);

	glm::vec3 resRgb = skyRgb + glowRgb * glowColor.a / 2.0f;
	
	Color res(resRgb);
	res.alpha = skyColor.a;

	return res;
}

void SkyDome::setPaused(bool state){
	mPaused = state;
}

void SkyDome::getLightIntensity(float* ambient, float* diffuse) const{
				
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


float SkyDome::getCyclePos() const{
	return mDayTime/mDayLength;
}

void SkyDome::create(Geometry* domeMesh, Texture2D* skyTexture,
	Texture2D* glowTexture, Texture2D* sunTexture){
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	mDomeMesh = domeMesh;
	mSkyTexture = skyTexture;
	mGlowTexture = glowTexture;
	mSunTexture = sunTexture;
	mShader->create();


	ShaderFactory::createShader(mPlanetShader, "planet.vp", "planet.fp");
	mPlanetShader.link();

	float vertices[3] = {0.0, 0.0, 0.0};
	uint32_t indices[1] = {0};

	mPlanetBatch.create(GL_POINTS,
		vertices, 1, 3*sizeof(float),
		indices, 1, sizeof(uint32_t)
	);

	mPlanetBatch.setVertexAttribute(0, 3, GL_FLOAT, 0);

	mPlanetShader.bindAttribLocation(0, "inPosition");
}

float SkyDome::getCycleLength() const{
	return mDayLength;
}

void SkyDome::advance(float dt){
	mDayTime += dt;

	if(mDayTime > mDayLength){
		mDayTime = 0.0f;
	}
	else if(mDayTime < 0.0f){
		mDayTime = mDayLength;
	}
}

void SkyDome::update(float dt){
	if(!mPaused){
		advance(dt);
	}

	float angle = 2*math::PI * (mDayTime/mDayLength);

	glm::vec2 circlePoint;
	math::pointOnCircle(mDomeScale, angle , circlePoint);

	mSunPos = glm::vec3(0,  circlePoint.y, circlePoint.x);

	mShader->use();
	mShader->setUniformVal("uSunPosition", mSunPos);
}

const glm::vec3& SkyDome::getSunPos() const{
	return mSunPos;
}

void SkyDome::setTimeOfDay(float p){
	mDayTime = p*mDayLength;
	update(0.0f);
}

void SkyDome::render(math::Camera& camera){
	mShader->use();
		
	glm::mat4 view;
	camera.getCameraMatrix(view);

	view[3][0] = 0.0f;
	view[3][1] = 0.0f;
	view[3][2] = 0.0f;

	glDepthMask(false);

	math::Transform domeTransform;
	glm::mat4 model;
		

	domeTransform.setScale(mDomeScale, mDomeScale, mDomeScale);
	domeTransform.setPosition(0, 0, 0);
	domeTransform.getMatrix(model);

	mShader->setModelViewProjMat(model,
		view, camera.getProjectionMatrix());

	glActiveTexture(GL_TEXTURE0);
	mSkyTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE1);
	mGlowTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);


	mDomeMesh->getBatch()->render();

		
	math::Transform sunTrans;
	glm::vec3 sp = mSunPos;
		
	sunTrans.setPosition(sp);
	glm::mat4 m;
	sunTrans.getMatrix(m);


	mPlanetShader.use();

	glActiveTexture(GL_TEXTURE0);
	mPlanetShader.setUniformVal("uPlanetTexture", 0);
	mSunTexture->bind();
	glBlendFunc(GL_ONE, GL_ONE);
	//camera.getMatrix(view);
	mPlanetShader.setUniformVal("uModelMat", glm::translate(sp));
	mPlanetShader.setUniformVal("uViewMat", view);

	glm::vec3 eyePos;
	camera.getTranslation(eyePos);

	mPlanetShader.setUniformVal("uCamPos", eyePos);
	mPlanetShader.setUniformVal("uProjMat", camera.getProjectionMatrix());
	mPlanetBatch.render();

	glDepthMask(true);
}

}; // </wt>