#include "wt/stdafx.h"
#include "wt/math.h"
#include "wt/Camera.h"
#include "wt/Plane.h"
#define TD_TRACE_TAG "Camera"

namespace wt
{


void extractFrustum(float frustum[6][4], const glm::mat4& view, const glm::mat4& projMat){
	const float* modl = glm::value_ptr(view);
	const float* proj= glm::value_ptr(projMat);
	float clip[16];
	float t;

	  /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	 /* Extract the numbers for the RIGHT plane */
	   frustum[0][0] = clip[ 3] - clip[ 0];
	   frustum[0][1] = clip[ 7] - clip[ 4];
	   frustum[0][2] = clip[11] - clip[ 8];
	   frustum[0][3] = clip[15] - clip[12];
	 /* Normalize the result */
	   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2]    * frustum[0][2] );
	   frustum[0][0] /= t;
	   frustum[0][1] /= t;
	   frustum[0][2] /= t;
	   frustum[0][3] /= t;
	 /* Extract the numbers for the LEFT plane */
	   frustum[1][0] = clip[ 3] + clip[ 0];
	   frustum[1][1] = clip[ 7] + clip[ 4];
	   frustum[1][2] = clip[11] + clip[ 8];
	   frustum[1][3] = clip[15] + clip[12];
	 /* Normalize the result */
	   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2]    * frustum[1][2] );
	   frustum[1][0] /= t;
	   frustum[1][1] /= t;
	   frustum[1][2] /= t;
	   frustum[1][3] /= t;
	 /* Extract the BOTTOM plane */
	   frustum[2][0] = clip[ 3] + clip[ 1];
	   frustum[2][1] = clip[ 7] + clip[ 5];
	   frustum[2][2] = clip[11] + clip[ 9];
	   frustum[2][3] = clip[15] + clip[13];
	 /* Normalize the result */
	   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2]    * frustum[2][2] );
	   frustum[2][0] /= t;
	   frustum[2][1] /= t;
	   frustum[2][2] /= t;
	   frustum[2][3] /= t;
	 /* Extract the TOP plane */
	   frustum[3][0] = clip[ 3] - clip[ 1];
	   frustum[3][1] = clip[ 7] - clip[ 5];
	   frustum[3][2] = clip[11] - clip[ 9];
	   frustum[3][3] = clip[15] - clip[13];
	 /* Normalize the result */
	   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2]    * frustum[3][2] );
	   frustum[3][0] /= t;
	   frustum[3][1] /= t;
	   frustum[3][2] /= t;
	   frustum[3][3] /= t;
	 /* Extract the FAR plane */
	   frustum[4][0] = clip[ 3] - clip[ 2];
	   frustum[4][1] = clip[ 7] - clip[ 6];
	   frustum[4][2] = clip[11] - clip[10];
	   frustum[4][3] = clip[15] - clip[14];
	 /* Normalize the result */
	   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2]    * frustum[4][2] );
	   frustum[4][0] /= t;
	   frustum[4][1] /= t;
	   frustum[4][2] /= t;
	   frustum[4][3] /= t;
	 /* Extract the NEAR plane */
	   frustum[5][0] = clip[ 3] + clip[ 2];
	   frustum[5][1] = clip[ 7] + clip[ 6];
	   frustum[5][2] = clip[11] + clip[10];
	   frustum[5][3] = clip[15] + clip[14];
	 /* Normalize the result */
	   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2]    * frustum[5][2] );
	   frustum[5][0] /= t;
	   frustum[5][1] /= t;
	   frustum[5][2] /= t;
	   frustum[5][3] /= t;
}

Camera::Camera() : mTranslation(0.0f, 0.0f, 0.0f), mForwardVector(kFORWARD_VEC), mRightVector(kRIGHT_VEC), mUpVector(kUP_VEC){
	setProjectionType(ePROJECTION_PERSPECTIVE);
	setPerspectiveParams(mPerspectiveParams);
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

bool Camera::contains(const glm::vec3& point){
#if 0
int p;
	for(p=0; p<6; p++){
		if(mFrustum[p][0] * point.x + mFrustum[p][1] * point.y + mFrustum[p][2] * point.z + mFrustum[p][3] <= 0){
			return false;
		}
	}

	return true;
#elif 0
	glm::mat4 viewMat;
	getCameraMatrix(viewMat);

	glm::mat4 mvp = mProjectionMatrix * viewMat;

	glm::vec4 clip = mvp * glm::vec4(point, 1.0f);

	return glm::abs(clip.x) < clip.w && 
		glm::abs(clip.y) < clip.w &&
		0 < clip.z &&
		clip.z < clip.w;
#else
	// Not implemented
	return true;
#endif
}

bool Camera::contains(const physx::PxBounds3& bounds){
#if 1
	// None of the solutions worked for some reason
	return true;
#elif 1
	physx::PxVec3 c = bounds.getCenter();
	physx::PxVec3 e = bounds.getExtents();


	glm::vec3 points[8] = {
		// Left - Down - Back
		glm::vec3(c.x - e.x, c.y - e.y, c.z - e.z),

		// Right - Down - Back
		glm::vec3(c.x + e.x, c.y - e.y, c.z - e.z),

		// Right - Down - Front 
		glm::vec3(c.x + e.x, c.y - e.y, c.z + e.z),

		// Left - Down - Front
		glm::vec3(c.x - e.x, c.y - e.y, c.z + e.z),

		// Left - Up - Back
		glm::vec3(c.x - e.x, c.y + e.y, c.z - e.z),

		// Right - Up - Back
		glm::vec3(c.x + e.x, c.y + e.y, c.z - e.z),

		// Right - Up - Front
		glm::vec3(c.x + e.x, c.y + e.y, c.z + e.z),
		
		// Left - Up - Front
		glm::vec3(c.x - e.x, c.y + e.y, c.z + e.z),
	};

	for(int i=0; i<8; i++){
		if(contains(points[i])){
			return true;
		}
	}

	return false;
#else
	// Taken from http://www.crownandcutlass.com/features/technicaldetails/frustum.html
	glm::mat4 viewMat;
	getCameraMatrix(viewMat);
	extractFrustum(mFrustum, viewMat, mProjectionMatrix);

	physx::PxVec3 center = bounds.getCenter();

	float x = center.x;
	float y = center.y;
	float z = center.y;

	physx::PxVec3 extents = bounds.getExtents();

	float sizeX = extents.x;
	float sizeY = extents.y;
	float sizeZ = extents.z;

	int p;

	for( p = 0; p < 6; p++ )
	{
		if( mFrustum[p][0] * (x - sizeX) + mFrustum[p][1] * (y - sizeY) + mFrustum[p][2] * (z - sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x + sizeX) + mFrustum[p][1] * (y - sizeY) + mFrustum[p][2] * (z - sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x - sizeX) + mFrustum[p][1] * (y + sizeY) + mFrustum[p][2] * (z - sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x + sizeX) + mFrustum[p][1] * (y + sizeY) + mFrustum[p][2] * (z - sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x - sizeX) + mFrustum[p][1] * (y - sizeY) + mFrustum[p][2] * (z + sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x + sizeX) + mFrustum[p][1] * (y - sizeY) + mFrustum[p][2] * (z + sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x - sizeX) + mFrustum[p][1] * (y + sizeY) + mFrustum[p][2] * (z + sizeZ) + mFrustum[p][3] > 0 )
			continue;
		if( mFrustum[p][0] * (x + sizeX) + mFrustum[p][1] * (y + sizeY) + mFrustum[p][2] * (z + sizeZ) + mFrustum[p][3] > 0 )
			continue;
		return false;
	}
	return true;
#endif
}

} // </wt>
