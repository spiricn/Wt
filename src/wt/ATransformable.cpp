#include "wt/stdafx.h"

#include "wt/ATransformable.h"
#include "wt/Math.h"

namespace wt
{

ATransformable::~ATransformable(){
}

void ATransformable::translate(const glm::vec3& offset){
	glm::vec3 translation;
	getTranslation(translation);

	setTranslation(translation + offset);
}

void ATransformable::rotate(const glm::vec3& axis, float angle){
#if 0
	glm::mat4 offset = glm::rotate(angle, axis);

	glm::quat quat;
	getRotation(quat);

	glm::mat4 rotation = glm::mat4_cast(quat);

	glm::mat4 newRot = offset*rotation;

	setRotation( glm::quat_cast(newRot) );
#else
	rotate( glm::angleAxis(angle, axis) );
#endif
}

void ATransformable::rotate(const glm::quat& offset){
	glm::quat rotation;
	getRotation(rotation);

	setRotation(rotation*offset);
}

void ATransformable::scale(const glm::vec3& offset){
	glm::vec3 scale;
	getScale(scale);

	setScale(scale + offset);
}

void ATransformable::getForwardVector(glm::vec3& result) const{
	glm::quat rotation;
	getRotation(rotation);

	result = glm::normalize( rotation*kFORWARD_VEC );
}

void ATransformable::getRightVector(glm::vec3& result) const{
	glm::quat rotation;
	getRotation(rotation);

	result = glm::normalize( rotation*kRIGHT_VEC );
}

void ATransformable::getUpVector(glm::vec3& result) const{
	glm::quat rotation;
	getRotation(rotation);

	result = glm::normalize( rotation*kUP_VEC );
}

void ATransformable::setPitch(float angle){
	glm::vec3 right;
	getRightVector(right);

	setRotation( glm::angleAxis(angle, right) );
}

void ATransformable::setYaw(float angle){
	setRotation( glm::angleAxis(angle, kUP_VEC) );
}

void ATransformable::moveForward(float distance){
	glm::vec3 forward;
	getForwardVector(forward);

	translate(forward*distance);
}

void ATransformable::moveUp(float distance){
	glm::vec3 up;
	getUpVector(up);

	translate(up*distance);
}

void ATransformable::strife(float distance){
	glm::vec3 right;
	getRightVector(right);

	translate(right*distance);
}

float ATransformable::getPitch() const{
	// TODO
	WT_THROW("Not implemented");

	return 0.0f;
}

float ATransformable::getYaw() const{
	// TODO
	WT_THROW("Not implemented");

	return 0.0f;
}

void ATransformable::getRotation(glm::vec3& axis, float& angle) const{
	glm::quat rotation;
	getRotation(rotation);

	glm::vec4 axisAngle;
	math::quatToAxisAngle(rotation, axisAngle);

	axis = axisAngle.swizzle(glm::X, glm::Y, glm::Z);
	angle = axisAngle.w;
}

void ATransformable::getTransformMatrix(glm::mat4& res) const{
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;

	getTranslation(translation);
	getScale(scale);
	getRotation(rotation);

	res = glm::translate(translation) * (glm::mat4_cast(rotation) * glm::scale(scale));
}

void ATransformable::setTransformMatrix(const glm::mat4& mat){
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;

	math::decomposeMatrix(mat, rotation, translation, scale);

	setTranslation(translation);
	setScale(scale);
	setRotation(rotation);
}

void ATransformable::setRotation(const glm::vec3& axis, float angle){
	setRotation( glm::angleAxis(angle, axis) );
}

void ATransformable::decomposeTransform(glm::vec3& pos, glm::vec3& scale, glm::quat& rot) const{
	getTranslation(pos);
	getScale(scale);
	getRotation(rot);
}

void ATransformable::getCameraMatrix(glm::mat4& dst) const{
#if 0
	glm::vec3 up, right, forward, translation;

	getUpVector(up);
	getForwardVector(forward);
	getRightVector(right);
	getTranslation(translation);

	glm::vec3 z = -forward;
	glm::vec3 x =glm::cross(up, z);

	glm::mat4 rotMat(1.0f);
	rotMat[0][0] = x.x;
	rotMat[0][1] = up.x;
	rotMat[0][2] = z.x;

	rotMat[1][0] = x.y;
	rotMat[1][1] = up.y;
	rotMat[1][2] = z.y;

	rotMat[2][0] = x.z;
	rotMat[2][1] = up.z;
	rotMat[2][2] = z.z;

	glm::mat4 transMat(1.0f);
	transMat[3][0] = -translation.x;
	transMat[3][1] = -translation.y;
	transMat[3][2] = -translation.z;

	dst = rotMat*transMat;
#else

	getTransformMatrix(dst);
	dst = glm::inverse(dst);

#endif
}

void ATransformable::lookAt(const glm::vec3& position){
	// TODO there's probably a better way of doing this
	glm::vec3 translation;
	getTranslation(translation);

	glm::vec3 forward = glm::normalize( position-translation );

	glm::vec3 right = glm::normalize( glm::vec3(forward.z, 0, -forward.x) );

	glm::vec3 up = glm::normalize( glm::cross(forward, right) );


	glm::mat4 rotMat(1.0f);

	// X Column
	rotMat[0][0] = right.x;
	rotMat[0][1] = right.y;
	rotMat[0][2] = right.z;

    // Y Column
	rotMat[1][0] = up.x;
	rotMat[1][1] = up.y;
	rotMat[1][2] = up.z;
                                    
    // Z Column
	rotMat[2][0] = forward.x;
	rotMat[2][1] = forward.y;
	rotMat[2][2] = forward.z;

	glm::quat rot = glm::quat_cast(rotMat);

	setRotation(rot);
}

void ATransformable::orbit(float radius, float theta, float rho, const glm::vec3& pos){
	WT_THROW("Not implemented");
}

void ATransformable::yaw(float angle){
	glm::vec3 right;

	rotate(kUP_VEC, angle);
}

void ATransformable::pitch(float angle){
	glm::vec3 right;

	getRightVector(right);

	rotate(right, angle);
}

const glm::vec3 ATransformable::kFORWARD_VEC = glm::vec3(0.0f, 0.0f, -1.0f);

const glm::vec3 ATransformable::kUP_VEC = glm::vec3(0.0f, 1.0f, 0.0f);

const glm::vec3 ATransformable::kRIGHT_VEC = glm::vec3(1.0f, 0.0f, 0.0f);

} // </wt>