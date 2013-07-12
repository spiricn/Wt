#include "stdafx.h"

#include "wte/Vec3View.h"


#include <wt/Log.h>
const char* Vec3View::TAG = "Vec3View";

Vec3View::Vec3View(QWidget* parent) : QWidget(parent), mValue(0.0f){
    ui.setupUi(this);
}

void Vec3View::onValueChanged(double){
	if(getValue() != mValue){
		mValue = getValue();
		emit onValueChanged();
	}
	else{
		LOGI(TAG, "Same value, skipping...");
	}
}