#include "stdafx.h"

#include "wte/VecEditWidget.h"

#define TD_TRACE_TAG "Vec3View"

VecEditWidget::VecEditWidget(QWidget* parent) : QWidget(parent), mValue(0.0f), mMode(eMODE_VEC3){
    ui.setupUi(this);

	setMode(mMode);
}

void VecEditWidget::onValueChanged(){
	// Check if there actually was any change
	
	double x = ui.x->value();
	double y = ui.y->value();
	double z = ui.z->value();
	double w = ui.w->value();

	bool valChanged = false;

	if(mMode == eMODE_VEC2){
		valChanged = mValue.x != x || mValue.y != y;
	}
	else if(mMode == eMODE_VEC3){
		valChanged = mValue.x != x || mValue.y != y || mValue.z != z;
	}
	else{
		valChanged = mValue.x != x || mValue.y != y || mValue.z != z || mValue.w != w;
	}

	if(valChanged){
		setValue(x, y, z, w);
	}
}

void VecEditWidget::setValue(double x, double y, double z, double w){
	// We need to block signals temporarily
	// otherwise 3 signals will be emitted for each spin box
	ui.x->blockSignals(true);
	ui.y->blockSignals(true);
	ui.z->blockSignals(true);
	ui.w->blockSignals(true);

	bool valChanged = false;

	ui.x->setValue(x);
	ui.y->setValue(y);

	if(mMode == eMODE_VEC2){
		valChanged = mValue.x != x || mValue.y != y;
	}
	else if(mMode == eMODE_VEC3){
		ui.z->setValue(z);
		valChanged = mValue.x != x || mValue.y != y || mValue.z != z;
	}
	else{
		ui.z->setValue(z);
		ui.w->setValue(w);
		valChanged = mValue.x != x || mValue.y != y || mValue.z != z || mValue.w != w;
	}

	ui.x->blockSignals(false);
	ui.y->blockSignals(false);
	ui.z->blockSignals(false);
	ui.w->blockSignals(false);

	// Call value changed manually to emit any signals if necessary
	if(valChanged){
		mValue = glm::vec4(x, y, z, w);
		emit valueChanged();
	}
}

void VecEditWidget::setMode(Mode mode){
	ui.z->setVisible( mode == eMODE_VEC3 || mode == eMODE_VEC4);
	ui.w->setVisible( mode == eMODE_VEC4 );

	mMode = mode;
}

void VecEditWidget::getValue(glm::vec2& dst) const{
	dst = mValue.swizzle(glm::X, glm::Y);
}

void VecEditWidget::getValue(glm::vec3& dst) const{
	dst = mValue.swizzle(glm::X, glm::Y, glm::Z);
}

void VecEditWidget::getValue(glm::vec4& dst) const{
	dst = mValue;
}

void VecEditWidget::setX(double x){
	setValue(x, mValue.y, mValue.z, mValue.w);
}

void VecEditWidget::setY(double y){
	setValue(mValue.x, y, mValue.z, mValue.w);
}

void VecEditWidget::setZ(double z){
	setValue(mValue.x, mValue.y, z, mValue.w);
}

void VecEditWidget::setW(double w){
	setValue(mValue.x, mValue.y, mValue.z, w);
}

void VecEditWidget::setValue(const glm::vec2& value){
	setValue(value.x,  value.y);
}

void VecEditWidget::setValue(const glm::vec3& value){
	setValue(value.x,  value.y, value.z);
}

void VecEditWidget::setValue(const glm::vec4& value){
	setValue(value.x,  value.y, value.z, value.w);
}

double VecEditWidget::getX() const{
	return ui.x->value();
}

double VecEditWidget::getY() const{
	return ui.y->value();
}

double VecEditWidget::getZ() const{
	return ui.z->value();
}

double VecEditWidget::getW() const{
	return ui.w->value();
}