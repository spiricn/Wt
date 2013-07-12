#include "stdafx.h"

#include "wte/Vec3View.h"

#define TD_TRACE_TAG "Vec3View"

Vec3View::Vec3View(QWidget* parent) : QWidget(parent), mValue(0.0f){
    ui.setupUi(this);
}

void Vec3View::onValueChanged(double){
	if(getValue() != mValue){
		mValue = getValue();
		emit onValueChanged();
	}
	else{
		LOGI("Same value, skipping...");
	}
}