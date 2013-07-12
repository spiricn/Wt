#include "wt/stdafx.h"
#include "wt/SkyDome.h"
#include "wt/Frustum.h"

namespace wt{


SkyDome::SkyDome() : mDayTime(0.0f), mDayLength(30.0), mDomeScale(500.0f){

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
	mShader.create();


	mPlanetShader.createFromFiles("shaders/planet.vp", "shaders/planet.fp");
	mPlanetShader.link();

	float vertices[3] = {0.0, 0.0, 0.0};
	Uint32 indices[1] = {0};

	mPlanetBatch.create(vertices, 1, 3*sizeof(float),
		indices, 1, sizeof(Uint32), GL_POINTS);

	mPlanetBatch.setVertexAttribute(0, 3, GL_FLOAT, 0);

	mPlanetShader.bindAttribLocation(0, "inPosition");
}

void SkyDome::update(float dt){
	mDayTime += dt;
	if(mDayTime > mDayLength){
		mDayTime = 0.0f;
	}
	else if(mDayTime < 0.0f){
		mDayTime = mDayLength;
	}

	float angle = 2*math::PI * (mDayTime/mDayLength);

	glm::vec2 circlePoint;
	math::pointOnCircle(mDomeScale, angle , circlePoint);

	mSunPos = glm::vec3(0,  circlePoint.y, circlePoint.x);

	mShader.use();
	mShader.setUniformVal("uSunPosition", mSunPos);
}

const glm::vec3& SkyDome::getSunPos() const{
	return mSunPos;
}

void SkyDome::setTimeOfDay(float p){
	mDayTime = p*mDayLength;
	update(0.0f);
}

void SkyDome::render(math::Camera& camera, const math::Frustum& frustum){
	mShader.use();
		
	glm::mat4 view;
	camera.getMatrix(view, false);
	glDepthMask(false);

	math::Transform domeTransform;
	glm::mat4 model;
		

	domeTransform.setScale(mDomeScale, mDomeScale, mDomeScale);
	domeTransform.setPosition(0, 0, 0);
	domeTransform.getMatrix(model);

	mShader.setModelViewProjMat(model,
		view, frustum.getProjMatrix());

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
	mPlanetShader.setUniformVal("uCamPos", camera.getPosition());
	mPlanetShader.setUniformVal("uProjMat", frustum.getProjMatrix());
	mPlanetBatch.render();

	glDepthMask(true);
}

}; // </wt>