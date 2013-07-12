#include "wt/stdafx.h"
#include "wt/Transform.h"

namespace wt{

namespace math{

void Transform::rotate(float x, float y, float z, float dAngle){
	mDirty = true;

	mRotation = mRotation * glm::angleAxis(dAngle, x, y, z);
}

Transform::Transform(const glm::mat4& mat) : mDirty(true){
	setMatrix(mat);
}

float Transform::getYaw() const{
	return 0.0f;
}

void Transform::getPitch() const{
}

void Transform::setFacing(const glm::vec3& fw, const glm::vec3& up){
	mDirty = true;

	glm::mat3 rot;
	glm::vec3 right = glm::normalize( glm::cross(up, fw) );

	rot[0][0] = right.x;
	rot[0][1] = right.y;
	rot[0][2] = right.z;

	rot[1][0] = up.x;
	rot[1][1] = up.y;
	rot[1][2] = up.z;

	rot[2][0] = fw.x;
	rot[2][1] = fw.y;
	rot[2][2] = fw.z;

	mRotation = glm::quat_cast(rot);
}

void Transform::setMatrix(const glm::mat4& mat){
	mDirty = true;

	math::decomposeMatrix(mat,
		mRotation, mPosition, mScale);
}

Transform::Transform() : mScale(glm::vec3(1.0f, 1.0f, 1.0f)),
	mPosition(glm::vec3(0.0f, 0.0f, 0.0f)), mDirty(true){
		setRotation(0, 1, 0, 0);
}

Transform::Transform(const Transform& other) : mDirty(true){
	mPosition = other.mPosition;
	mScale = other.mScale;
	mRotation = other.mRotation;
}

const glm::quat& Transform::getQuat() const{
	return mRotation;
}

void Transform::getMatrix(glm::mat4x4& dst) const{
	if(mDirty){
		glm::mat4x4 translation = glm::translate(glm::mat4x4(1.0f), mPosition.x, mPosition.y, mPosition.z);
		glm::mat4x4 scale = glm::scale(mScale.x, mScale.y, mScale.z);;
		glm::mat4x4 rotation = glm::mat4_cast(mRotation);

		Transform* self = const_cast<Transform*>(this);
		self->mMatrix = translation*(rotation*scale);
		self->mDirty = false;
	}

	dst = mMatrix;
}


void Transform::translate(const glm::vec3& delta){
	mDirty = true;
	mPosition += delta;
}

void Transform::translate(float dx, float dy, float dz){
	mDirty = true;
	mPosition += glm::vec3(dx, dy, dz);
}

void Transform::setPosition(const glm::vec3& pos){
	mDirty = true;
	setPosition(pos.x, pos.y, pos.z);
}

void Transform::getRotation(glm::vec4& res) const{
	math::quatToAxisAngle(mRotation, res);
}

void Transform::setPosition(float x, float y, float z){
	mDirty = true;
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void Transform::setQuat(float x, float y, float z, float w){
	mDirty = true;
	mRotation.x = x;
	mRotation.y = y;
	mRotation.z = z;
	mRotation.w = w;
}

void Transform::setRotation(float x, float y, float z, float angle){
	mDirty = true;
	mRotation = glm::angleAxis(angle, x, y, z);
}

void Transform::setRotation(const glm::quat& rot){
	mDirty = true;
	mRotation = rot;
}

void Transform::setScale(const glm::vec3& scale){
	mDirty = true;
	mScale = scale;
}

void Transform::setScale(float sx, float sy, float sz){
	mDirty = true;
	mScale = glm::vec3(sx, sy, sz);
}

void Transform::moveForward(float distance){
	mDirty = true;

	glm::vec3 forward = getForwardVec();

	mPosition += forward*distance;
}

void Transform::strife(float distance){
	mDirty = true;

	glm::vec3 right = getRightVec();

	mPosition += right*distance;
}

void Transform::setYaw(float deg){
	mDirty = true;

	mRotation = mRotation*glm::angleAxis(deg, 0.0f, 1.0f, 0.0f);
}

void Transform::setPitch(float deg){
	mDirty = true;

	mRotation = mRotation*glm::angleAxis(deg, 1.0f, 0.0f, 0.0f);
}

void Transform::getFacing(glm::vec3& forward, glm::vec3& up) const{
#if 0
	// TODO not tested, but probably a lot faster
	forward.x = 2.0f * (mRotation.x * mRotation.z + mRotation.w * mRotation.y);
	forward.y =  2.0f * (mRotation.y * mRotation.x - mRotation.w * mRotation.x); // mistake ? should be: 2 * (y * z - w * x),
	forward.z = 1.0f - 2.0f * (mRotation.x * mRotation.x + mRotation.y * mRotation.y);

	up.x = 2.0f * (mRotation.x * mRotation.y - mRotation.w * mRotation.z), 
	up.y = 1.0f - 2.0f * (mRotation.x * mRotation.x + mRotation.z * mRotation.z),
	up.z = 2.0f * (mRotation.y * mRotation.z + mRotation.w * mRotation.x);
#else
	forward = getForwardVec();
	up = getUpVec();
#endif
}

glm::vec3 Transform::getForwardVec() const{
	return glm::normalize( mRotation*glm::vec3(0.0f, .0f, -1.0f) );
}

glm::vec3 Transform::getRightVec() const{
	return glm::normalize( mRotation*glm::vec3(1.0f, .0f, .0f) );
}

glm::vec3 Transform::getUpVec() const{
	return glm::normalize( mRotation*glm::vec3(.0f, 1.0f, .0f) );
}

const glm::vec3& Transform::getPosition() const{
	return mPosition;
}

const glm::vec3& Transform::getScale() const{
	return mScale;
}

void Transform::postConcat(const Transform& other){
	mDirty = true;

	*this = *this * other;
}


void Transform::preConcat(const Transform& other){
	mDirty = true;

	*this = other * *this;
}

Transform operator*(const Transform& a, const Transform& b){
	glm::mat4 m1, m2;
	a.getMatrix(m1);
	b.getMatrix(m2);

	return Transform(m1*m2);
}

Transform operator*(const Transform& transform, const glm::mat4& mat){
	glm::mat4 m;
	transform.getMatrix(m);

	return Transform(m*mat);
}

Transform operator*(const glm::mat4& mat, const Transform& transform){
	glm::mat4 m;
	transform.getMatrix(m);

	return Transform(mat*m);
}

}; // </math>

}; // </wt>
