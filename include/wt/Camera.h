#ifndef WT_CAMERA_H
#define WT_CAMERA_H

#include <wt/lua/State.h>

#include "wt/Transform.h"
#include "wt/ATransformable.h"

namespace wt{

namespace math{

class Camera : public ATransformable{
public:
	struct PerspectiveParams{
		float verticalFov;
		float nearPlaneDistance;
		float farPlaneDistance;
		float nearPlaneWidth;
		float nearPlaneHeight;

		PerspectiveParams();
	}; // </PerspectiveParams>

	struct OrthoParams{
		float screenWidth;
		float screenHeight;

		OrthoParams();
	}; // </OrthoParams>

	enum ProjectionType{
		ePROJECTION_PERSPECTIVE,
		ePROJECTION_ORTHO
	};

	Camera();

	void setTranslation(const glm::vec3& translation);

	void setRotation(const glm::quat& rotation);

	void setScale(const glm::vec3& scale);

	void getScale(glm::vec3& result) const;

	void getTranslation(glm::vec3& result) const;

	void getRotation(glm::quat& result) const;

	const PerspectiveParams& getPerspectiveParams() const;

	const OrthoParams& getOrthoParams() const;
	
	void setProjectionType(ProjectionType type);

	ProjectionType getProjectionType() const;

	const glm::mat4& getProjectionMatrix() const;

	void getTransformMatrix(glm::mat4& res) const;

	void setPerspectiveParams(const PerspectiveParams& params);

	void setOrthoParams(const OrthoParams& params);

	void getForwardVector(glm::vec3& result) const;

	void getRightVector(glm::vec3& result) const;

	void getUpVector(glm::vec3& result) const;

	void lookAt(const glm::vec3& position);

	void setOrientation(const glm::vec3& fw, const glm::vec3& up);

private:
	void rebuildProjectionMatrix();

	// Projection params
	ProjectionType mProjectionType;
	PerspectiveParams mPerspectiveParams;
	OrthoParams mOrthoParams;
	glm::mat4 mProjectionMatrix;

	// Camera params
	glm::vec3 mTranslation;
	glm::vec3 mForwardVector;
	glm::vec3 mUpVector;
	glm::vec3 mRightVector;

}; // </Camera>

}; // </math>

}; // </wt>

#endif

