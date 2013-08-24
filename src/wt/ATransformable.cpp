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
	glm::quat rotation = glm::angleAxis(angle, axis.x, axis.y, axis.z);

	rotate(rotation);
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

void ATransformable::getForwardVector(glm::vec3& result){
	glm::quat rotation;
	getRotation(rotation);

	result = glm::normalize( rotation*glm::vec3(0.0f, 0.0f, -1.0f) );
}

void ATransformable::getRightVector(glm::vec3& result){
	glm::quat rotation;
	getRotation(rotation);

	result = glm::normalize( rotation*glm::vec3(1.0f, 0.0f, 0.0f) );
}

void ATransformable::getUpVector(glm::vec3& result){
	glm::quat rotation;
	getRotation(rotation);

	result = glm::normalize( rotation*glm::vec3(0.0f, 1.0f, 0.0f) );
}

void ATransformable::setPitch(float angle){
	setRotation( glm::angleAxis(angle, 1.0f, 0.0f, 0.0f) );
}

void ATransformable::setYaw(float angle){
	setRotation( glm::angleAxis(angle, 0.0f, 1.0f, 0.0f) );
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

void ATransformable::getCameraMatrix(glm::vec3& dst) const{
	// TODO

	WT_THROW("Not implemented");
}


} // </wt>