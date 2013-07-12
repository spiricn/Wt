#ifndef WT_FRUSTUM_H
#define WT_FRUSTUM_H

#include "wt/Defines.h"

namespace wt{

namespace math{

class Frustum{
private:
	enum ProjectionType {Perspective, Orthographic};

	glm::mat4x4		mOrthoProj, mPerspectiveProj;
	ProjectionType	mProjType;

	float mFov, mNearDist, mFarDist, mNearWidth, mNearHeight;
	float mOrthoWidth, mOrthoHeight;


	void updatePerspectiveMat();

	void updateOrhoMat();
public:
	Frustum();

	float getFov() const;

	float getNearClip() const;

	float getFarClip() const;

	float getAspectRatio() const;

	bool contains(const glm::vec3& point, const glm::mat4& view) const;

	bool contains(const physx::PxBounds3& bounds, const glm::mat4& view) const;

	const glm::mat4x4& getProjMatrix() const;

	void setPerspectiveProj(float w, float h, float fov, float nearDistance, float farDistance);

	void setOrthoProj(float width, float height);
};

}; // </math>

}; // </wt>

#endif
