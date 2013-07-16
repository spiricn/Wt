#include "stdafx.h"

#include "wte/Vec3View.h"

#define TD_TRACE_TAG "Vec3View"

Vec3View::Vec3View(QWidget* parent) : QWidget(parent), mValue(0.0f){
    ui.setupUi(this);
}

float Vec3View::getX() const{
	return ui.x->value();
}

float Vec3View::getY() const{
	return ui.y->value();
}

float Vec3View::getZ() const{
	return ui.z->value();
}

void Vec3View::setValue(const glm::vec3& values){
	// We need to block signals temporarily
	// otherwise 3 signals will be emitted for each spin box
	ui.x->blockSignals(true);
	ui.y->blockSignals(true);
	ui.z->blockSignals(true);

	setX(values.x);
	setY(values.y);
	setZ(values.z);

	ui.x->blockSignals(false);
	ui.y->blockSignals(false);
	ui.z->blockSignals(false);

	// Call value changed manually to emit any signals if necessary
	onValueChanged();
}

glm::vec3 Vec3View::getValue(){
	return glm::vec3(getX(), getY(), getZ());
}

void Vec3View::setX(float x){
	ui.x->setValue(x);
}

void Vec3View::setY(float y){
	ui.y->setValue(y);
}

void Vec3View::setZ(float z){
	ui.z->setValue(z);
}

void Vec3View::onValueChanged(){
	// Check if there actually was any change
	if(getValue() != mValue){
		// If it was, emit the signal
		mValue = getValue();
		emit valueChanged();
	}
}