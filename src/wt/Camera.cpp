#include "wt/stdafx.h"
#include "wt/math.h"
#include "wt/Camera.h"

#define TD_TRACE_TAG "Camera"

namespace wt
{

namespace math
{

Camera::Camera() : mTranslation(0.0f, 0.0f, 0.0f), mForwardVector(kFORWARD_VEC), mRightVector(kRIGHT_VEC), mUpVector(kUP_VEC){
	setProjectionType(ePROJECTION_PERSPECTIVE);
	rebuildProjectionMatrix();
}

void Camera::lookAt(const glm::vec3& position){
	mForwardVector = -glm::normalize( position-mTranslation );

	mRightVector = glm::normalize( glm::vec3(mForwardVector.z, 0, -mForwardVector.x) );

	mUpVector = glm::normalize( glm::cross(mForwardVector, mRightVector) );
}

void Camera::setOrientation(const glm::vec3& fw, const glm::vec3& up){
	mForwardVector = glm::normalize(fw);
	mUpVector = glm::normalize(up);
	mRightVector = glm::normalize( glm::cross(mForwardVector, mUpVector) );
}

void Camera::setTranslation(const glm::vec3& translation){
	mTranslation = translation;
}

void Camera::setRotation(const glm::quat& rotation){
	glm::mat3 rot = glm::mat3_cast(rotation);
	
	mForwardVector.x = rot[2][0];
	mForwardVector.y = rot[2][1];
	mForwardVector.z = rot[2][2];
	mForwardVector = glm::normalize(mForwardVector);

	mRightVector.x = rot[0][0];
	mRightVector.y = rot[0][1];
	mRightVector.z = rot[0][2];
	mRightVector = glm::normalize(mRightVector);

#if 0
	mUpVector.x = rot[1][0];
	mUpVector.y = rot[1][1];
	mUpVector.z = rot[1][2];
#else
	mUpVector = glm::cross(mForwardVector, mRightVector);
#endif

	mUpVector = glm::normalize(mUpVector);
}

void Camera::setScale(const glm::vec3&){
}

void Camera::getScale(glm::vec3& result) const{
	result = glm::vec3(1.0f);
}

void Camera::getTranslation(glm::vec3& result) const{
	result = mTranslation;
}

void Camera::getTransformMatrix(glm::mat4& dst) const{
	// X Column
	dst[0][0] = mRightVector.x;
	dst[0][1] = mRightVector.y;
	dst[0][2] = mRightVector.z;

    // Y Column
	dst[1][0] = mUpVector.x;
	dst[1][1] = mUpVector.y;
	dst[1][2] = mUpVector.z;
                                    
    // Z Column
	dst[2][0] = mForwardVector.x;
	dst[2][1] = mForwardVector.y;
	dst[2][2] = mForwardVector.z;


	dst[3][0] = mTranslation.x;
	dst[3][1] = mTranslation.y;
	dst[3][2] = mTranslation.z;


	dst[3][3] = 1.0f;
}

void Camera::getRotation(glm::quat& result) const{
	glm::mat4 tf;
	getTransformMatrix(tf);

	tf[3][0] = 0.0f;
	tf[3][1] = 0.0f;
	tf[3][2] = 0.0f;

	result = glm::quat_cast(tf);
}

void Camera::setProjectionType(ProjectionType type){
	if(mProjectionType != type){
		mProjectionType = type;
		rebuildProjectionMatrix();
	}
}

const glm::mat4& Camera::getProjectionMatrix() const{
	return mProjectionMatrix;
}

Camera::PerspectiveParams::PerspectiveParams() : verticalFov(50.0f), nearPlaneWidth(640.0f), nearPlaneHeight(480.0f), nearPlaneDistance(1.0f), farPlaneDistance(10000.0f){
}

Camera::OrthoParams::OrthoParams() : screenWidth(640.0f), screenHeight(480.0f){
}

Camera::ProjectionType Camera::getProjectionType() const{
	return mProjectionType;
}

const Camera::PerspectiveParams& Camera::getPerspectiveParams() const{
	return mPerspectiveParams;
}

const Camera::OrthoParams& Camera::getOrthoParams() const{
	return mOrthoParams;
}

void Camera::setPerspectiveParams(const PerspectiveParams& params){
	mPerspectiveParams = params;

	if(mProjectionType == ePROJECTION_PERSPECTIVE){
		rebuildProjectionMatrix();
	}
}

void Camera::setOrthoParams(const OrthoParams& params){
	mOrthoParams = params;

	if(mProjectionType == ePROJECTION_ORTHO){
		rebuildProjectionMatrix();
	}
}

void Camera::rebuildProjectionMatrix(){
	if(mProjectionType == ePROJECTION_PERSPECTIVE){
		mProjectionMatrix = glm::perspective(
			mPerspectiveParams.verticalFov, 
			mPerspectiveParams.nearPlaneWidth/mPerspectiveParams.nearPlaneHeight,
			mPerspectiveParams.nearPlaneDistance,
			mPerspectiveParams.farPlaneDistance
		);
	}
	else if(mProjectionType == ePROJECTION_ORTHO){
		mProjectionMatrix = glm::ortho(-mOrthoParams.screenWidth/2.0f, mOrthoParams.screenWidth/2.0f, -mOrthoParams.screenHeight/2.0f, mOrthoParams.screenHeight/2.0f, -1.0f, 1.0f);
	}
}

void Camera::getForwardVector(glm::vec3& result) const{
	result = mForwardVector;
}

void Camera::getRightVector(glm::vec3& result) const{
	result = mRightVector;
}

void Camera::getUpVector(glm::vec3& result) const{
	result = mUpVector;
}


} // </math>

} // </wt>
