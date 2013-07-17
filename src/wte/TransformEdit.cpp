#include "stdafx.h"

#include <wt/Math.h>

#include "wte/TransformEdit.h"

#define TD_TRACE_TAG "TransformEdit"

TransformEdit::TransformEdit(QWidget* parent) : QWidget(parent){
    ui.setupUi(this);

	ui.rotationAngle->setMinValue(0.0);
	ui.rotationAngle->setMaxValue(360.0);
	ui.rotationAngle->setValue(0.0);
}

void TransformEdit::onRotAngleChanged(){
	double val = ui.rotationAngle->getValue();
	if(val != mRotAngle){
		mRotAngle = val;
		emit rotationChanged();
	}
}

void TransformEdit::onRotAxisChanged(){
	glm::vec3 axis;
	ui.rotAxis->getValue(axis);

	if(mRotAxis != axis){
		mRotAxis = axis;
		emit rotationChanged();
	}
}

void TransformEdit::onScaleChanged(){
	glm::vec3 scale;
	ui.scle->getValue(scale);

	if(mScale != scale){
		mScale = scale;
		emit scaleChanged();
	}
}

void TransformEdit::onTranslationChanged(){
	glm::vec3 translation;
	ui.translation->getValue(translation);

	if(mTranslation != translation){
		mTranslation = translation;
		emit translationChanged();
	}
}

glm::vec3 TransformEdit::getScale() const{
	glm::vec3 scale;
	ui.scle->getValue(scale);

	// Prevent invalid scale(i.e. 0 values)
	scale.x = scale.x == 0.0f ? 0.0001f : scale.x;
	scale.y = scale.y == 0.0f ? 0.0001f : scale.y;
	scale.z = scale.z == 0.0f ? 0.0001f : scale.z;

	return scale;
}

float TransformEdit::getRotAngle() const{
	//return ui.rotAngle->value();
	return mRotAngle;
}

glm::vec3 TransformEdit::getRotAxis() const{
	glm::vec3 res;
	ui.rotAxis->getValue(res);
	return res;
}

glm::vec3 TransformEdit::getTranslation() const{
	glm::vec3 res;
	ui.translation->getValue(res);
	return res;
}

void TransformEdit::setScale(const glm::vec3& scale){
	ui.scle->setValue(scale);
}

glm::quat TransformEdit::getRotation() const{
	return glm::angleAxis(getRotAngle(),
		// Normalize the axis otherwise the quaternion is going to end up invalid
		glm::normalize(getRotAxis())
	);
}

void TransformEdit::setRotation(const glm::quat& rot){

	glm::vec4 res;
	wt::math::quatToAxisAngle(rot, res);

	setRotation(res.swizzle(glm::X, glm::Y, glm::Z), res.w);
}

void TransformEdit::setTranslation(const glm::vec3& translation){
	ui.translation->setValue(translation);
}

void TransformEdit::setRotation(const glm::vec3& axis, float angle){
	ui.rotationAngle->setValue(angle);

	ui.rotAxis->setValue(axis);
}
