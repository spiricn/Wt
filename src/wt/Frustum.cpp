#include "wt/stdafx.h"
#include "wt/Frustum.h"

namespace wt{

namespace math{

void Frustum::updatePerspectiveMat(){
	mPerspectiveProj = glm::perspective(mFov, getAspectRatio(), mNearDist, mFarDist);
}

void Frustum::updateOrhoMat(){
	mOrthoProj = glm::ortho(-mOrthoWidth/2.0f, mOrthoWidth/2.0f, -mOrthoHeight/2.0f, mOrthoHeight/2.0f, -1.0f, 1.0f);
}


Frustum::Frustum() :	mProjType(Perspective),
			mFov(50.0f), mNearWidth(640), mNearHeight(480), mNearDist(1.0f), mFarDist(10000.0f),
			mOrthoWidth(640), mOrthoHeight(480){
}

const glm::mat4x4& Frustum::getProjMatrix() const{
	return mProjType==Perspective?mPerspectiveProj:mOrthoProj;
}

void Frustum::setPerspectiveProj(float nearWidth, float nearHeight, float fov, float nearDistance, float farDistance){
	mProjType = Perspective;

	mNearWidth = nearWidth;
	mNearHeight = nearHeight;
	mFov = fov;
	mNearDist=nearDistance;
	mFarDist=farDistance;

	updatePerspectiveMat();
}

void Frustum::setOrthoProj(float width, float height){
	mProjType = Orthographic;

	mOrthoWidth = width;
	mOrthoHeight = height;

	updateOrhoMat();
}


float Frustum::getFov() const{
	return mFov;
}

float Frustum::getNearClip() const{
	return mNearDist;
}

float Frustum::getFarClip() const{
	return mFarDist;
}

float Frustum::getAspectRatio() const{
	return mNearWidth/mNearHeight;
}

bool Frustum::contains(const glm::vec3& point, const glm::mat4& view) const{
	WT_UNUSED(point);
	WT_UNUSED(view);

	return true;
}

bool Frustum::contains(const physx::PxBounds3& bounds, const glm::mat4& view) const{
	WT_UNUSED(bounds);
	WT_UNUSED(view);

	return true;
}


}; // </math>

}; // </wt>
