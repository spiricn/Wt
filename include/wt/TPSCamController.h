#ifndef WT_TPSCAMCONTROLER_H
#define WT_TPSCAMCONTROLER_H


#include "wt/stdafx.h"

#include "wt/Camera.h"
#include "wt/AGameInput.h"
#include "wt/CameraControler.h"

namespace wt
{

namespace math
{

class TPSCameraControler : public CameraControler{
public:
	TPSCameraControler(Camera* camera=NULL, const ATransformable* target=NULL);

	void setCamera(math::Camera* camera);

	void setTarget(const ATransformable* target);

	void zoomIn(float dt);

	void handle(float dt, AGameInput* input);

	void zoomOut(float dt);

	void lookHorizontal(float dx);

	void lookVertical(float dy);

private:
	void updateCam();

private:
	Camera* mCamera;
	float mMouseSensitivity;
	const ATransformable* mTarget;
	float mTheta, mRho, mDistance;
}; // </TPSCamController>

}; // </wt>

}; // </wt>

#endif // </WT_TPSCAMCONTROLER_H>
