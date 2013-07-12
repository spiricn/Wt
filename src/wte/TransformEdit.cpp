#include "stdafx.h"

#include <wt/Math.h>

#include "wte/TransformEdit.h"

#define TD_TRACE_TAG "TransformEdit"

TransformEdit::TransformEdit(QWidget* parent) : QWidget(parent){
    ui.setupUi(this);
}

void TransformEdit::onAngleSliderChanged(int value){
	float angle = (value/100.0f)*360.0f;
	if(angle != mRotAngle){
		mRotAngle = angle;
		ui.rotAngle->setValue( angle);

		emit rotationChanged();
	}
}


void TransformEdit::onAngleSpinboxChanged(double value){
	if(value != mRotAngle){
		mRotAngle = value;

		ui.angleSlider->setValue( value/360.0f * 100 );

		emit rotationChanged();
	}
}

void TransformEdit::onRotAxisChanged(){
	if(mRotAxis != ui.rotAxis->getValue()){
		mRotAxis = ui.rotAxis->getValue();
		emit rotationChanged();
	}
}

void TransformEdit::onScaleChanged(){
	if(mScale != ui.scle->getValue()){
		mScale = ui.scle->getValue();
		emit scaleChanged();
	}
}

void TransformEdit::onTranslationChanged(){
	if(mTranslation != ui.translation->getValue()){
		mTranslation = ui.translation->getValue();
		emit translationChanged();
	}
	
}

glm::vec3 TransformEdit::getScale() const{
	return ui.scle->getValue();
}

float TransformEdit::getRotAngle() const{
	return ui.rotAngle->value();
}

glm::vec3 TransformEdit::getRotAxis() const{
	return ui.rotAxis->getValue();
}

glm::vec3 TransformEdit::getTranslation() const{
	return ui.translation->getValue();
}

void TransformEdit::setScale(const glm::vec3& scale){
	ui.scle->setValue(scale);
}

glm::quat TransformEdit::getRotation() const{
	return glm::angleAxis(getRotAngle(), getRotAxis());
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
	ui.rotAngle->setValue(angle);
	ui.angleSlider->setValue( (angle/360.0f) * 100.0f);

	ui.rotAxis->setValue(axis);
}
