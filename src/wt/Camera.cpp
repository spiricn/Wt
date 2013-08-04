#include "wt/stdafx.h"
#include "wt/math.h"
#include "wt/Camera.h"

#define TD_TRACE_TAG "Camera"

namespace wt{

namespace math{

const glm::vec3 Camera::FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 Camera::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Camera::RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);

float Camera::getYaw() const{
	return math::vectorAngle(mForward);
}

const float Camera::getPitch() const{
	// TODO
	return 0;
}

void Camera::generateMetaTable(){
	expose("getPitch", &Camera::getPitch);
	expose("getYaw", &Camera::getYaw);
	expose("getPosition", &Camera::getPosition);
	expose("setPosition", &Camera::setPosition);
	expose("lookAt", &Camera::lookAt);
	expose("getForwardVec", &Camera::getForwardVec);
	expose("getUpVec", &Camera::getUpVec);
	expose("getRightVec", &Camera::getRightVec);
	expose("move", &Camera::move);
	expose("orbit", &Camera::orbit);
}

void Camera::setRotation(const glm::quat& quat){
	mUpdateMatrix=true;

	glm::mat3 rot = glm::mat3_cast(quat);

	mForward = FORWARD*rot;
	mUp = UP*rot;
	mRight = RIGHT*rot;

	mRight.x = rot[0][0];
	mRight.y = rot[0][1];
	mRight.z = rot[0][2];

	mUp.x = rot[1][0];
	mUp.y = rot[1][1];
	mUp.z = rot[1][2];

	mForward.x = rot[2][0];
	mForward.y = rot[2][1];
	mForward.z = rot[2][2];
}

void Camera::getRotation(glm::mat4& rot){
	buildMatrix(rot, true);
}

void Camera::getRotation(glm::quat& res){
	glm::mat4 rot;
	getRotation(rot);

	res = glm::quat_cast(rot);
}

void Camera::buildCameraMatrix(glm::mat4x4& dst){
	// Load identity
	dst = glm::mat4x4();

	glm::vec3 x, z;
			
	// Make rotation matrix
	// Z vector is reversed
	z.x = -mForward.x;
	z.y = -mForward.y;
	z.z = -mForward.z;

	// X vector = Y cross Z 
	x = glm::cross(mUp, z);

	// Matrix has no translation information and is
	// transposed.... (rows instead of columns)
	dst[0][0] = x.x;
	dst[1][0] = x.y;
	dst[2][0] = x.z;
	dst[3][0] = 0.0;
	dst[0][1] = mUp.x;
	dst[1][1] = mUp.y;
	dst[2][1] = mUp.z;
	dst[3][1] = 0.0;
	dst[0][2] = z.x;
	dst[1][2] = z.y;
	dst[2][2] = z.z;
	dst[3][2] = 0.0;
	dst[0][3] = 0.0;
	dst[1][3] = 0.0;
	dst[2][3] = 0.0;
	dst[3][3] = 1.0;

	if(!mMatrixRotOnly){
		glm::mat4x4 translation;
		translation = glm::translate(glm::mat4x4(1.0f), -mOrigin.x, -mOrigin.y, -mOrigin.z);
		
		dst = dst*translation;
	}
}

void Camera::buildMatrix(glm::mat4x4& dst, bool bRotationOnly){
	// X Column
	dst[0][0] = mRight.x;
	dst[0][1] = mRight.y;
	dst[0][2] = mRight.z;

    // Y Column
	dst[1][0] = mUp.x;
	dst[1][1] = mUp.y;
	dst[1][2] = mUp.z;
                                    
    // Z Column
	dst[2][0] = mForward.x;
	dst[2][1] = mForward.y;
	dst[2][2] = mForward.z;

	if(bRotationOnly == true){
		dst[3][0] = 0.0f;
		dst[3][1] = 0.0f;
		dst[3][2] = 0.0f;
	}
	else{
		dst[3][0] = mOrigin.x;
		dst[3][1] = mOrigin.y;
		dst[3][2] = mOrigin.z;
	}

	dst[3][3] = 1.0f;
}

void Camera::localToWorld(const glm::vec3& local, glm::vec3& dst, bool rotOnly){
	glm::mat4x4 mat;
	buildMatrix(mat, rotOnly);
	float* rotMat = glm::value_ptr(mat);

	// Do the rotation (inline it, and remove 4th column...)
	dst.x = rotMat[0] * local.x + rotMat[4] * local.y + rotMat[8] *  local.z;
	dst.y = rotMat[1] * local.x + rotMat[5] * local.y + rotMat[9] *  local.z;
	dst.z = rotMat[2] * local.x + rotMat[6] * local.y + rotMat[10] * local.z;

	// Translate the point
	if(!rotOnly){
		dst.x += mOrigin.x;
		dst.y += mOrigin.y;
		dst.z += mOrigin.z;
    }
}

void Camera::rotateLocal(float fAngle, float x, float y, float z){
	glm::vec3 worldVec, localVec(x,y,z);

	localToWorld(localVec, worldVec, true);

	rotateWorld(fAngle, worldVec.x, worldVec.y, worldVec.z);

	mUpdateMatrix=true;
}

void Camera::rotateWorld(float fAngle, float x, float y, float z){
	// Create rotation matrix
	glm::mat4x4 mat = glm::rotate(fAngle, x, y, z);
	float* rotMat = glm::value_ptr(mat);


	//float* rotMat = mat.mData;
	glm::vec3 newVect;

	//// Transform the up axis (inlined 3x3 rotation)
	newVect.x = rotMat[0] * mUp.x + rotMat[4] * mUp.y + rotMat[8] *  mUp.z;	
	newVect.y = rotMat[1] * mUp.x + rotMat[5] * mUp.y + rotMat[9] *  mUp.z;	
	newVect.z = rotMat[2] * mUp.x + rotMat[6] * mUp.y + rotMat[10] * mUp.z;	
	mUp = newVect;

	// Transform the forward axis
	newVect[0] = rotMat[0] * mForward.x + rotMat[4] * mForward.y + rotMat[8] *  mForward.z;	
	newVect[1] = rotMat[1] * mForward.x + rotMat[5] * mForward.y + rotMat[9] *  mForward.z;	
	newVect[2] = rotMat[2] * mForward.x + rotMat[6] * mForward.y + rotMat[10] * mForward.z;	
	mForward = newVect;

	// Recalculate right axis
	mRight = glm::cross(mUp, mForward);

	// Camera matrix needs to be updated
	mUpdateMatrix=true;
}


void Camera::getMatrix(glm::mat4x4& dst, bool rotOnly){
	if(mUpdateMatrix || rotOnly!=mMatrixRotOnly){
		mMatrixRotOnly=rotOnly;
		mUpdateMatrix=false;
		buildCameraMatrix(mViewMatrix);
	}

	dst = mViewMatrix;
}

const glm::vec3& Camera::getForwardVec() const{
	return mForward;
}

const glm::vec3& Camera::getUpVec() const{
	return mUp;
}

const glm::vec3& Camera::getRightVec() const{
	return mRight;
}

const glm::vec3& Camera::getPosition() const{
	return mOrigin;
}

void Camera::setPosition(const glm::vec3& origin){
	mUpdateMatrix=true;
	mOrigin=origin;
}

void Camera::yaw(float deg){
	rotateWorld(math::degToRad(deg), 0, 1, 0);
}

void Camera::pitch(float deg){
	rotateLocal(math::degToRad(deg), 1,0,0);
}

void Camera::move(const glm::vec3& vec){
	mOrigin = mOrigin+(mForward*vec.z);
	mOrigin = mOrigin+(mUp*vec.y);
	mOrigin = mOrigin+(mRight*vec.x);

	mUpdateMatrix=true;
}

void Camera::orbit(float radius, float theta, float rho, const glm::vec3& pos){
	pointOnSphere(radius, theta, rho, pos, mOrigin);
	lookAt(pos);

	mUpdateMatrix=true;
}

void Camera::lookAt(const glm::vec3& point){
	mForward = point-mOrigin;
	mForward = glm::normalize(mForward);

	mRight = glm::normalize( glm::vec3(mForward.z, 0, -mForward.x) );

	mUp = glm::normalize( glm::cross(mForward, mRight) );

	mUpdateMatrix=true;
}

}; // </math>

}; // </wt>
