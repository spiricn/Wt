#ifndef WT_CAMERACONTROLER_H
#define WT_CAMERACONTROLER_H


#include "wt/AGameInput.h"

namespace wt{

namespace math{

class CameraControler{
public:
	virtual void handle(float dt, AGameInput* input)=0;

	virtual void setCamera(math::Camera* camera)=0;
};

}; // </math>

}; // </wt>

#endif
