#include "stdafx.h"

#include "wte/SlidedSpinboxWidget.h"

#define TD_TRACE_TAG "SlidedSpinbox"

SlidedSpinboxWidget::SlidedSpinboxWidget(QWidget* parent) : QWidget(parent), mValue(0.0){
    ui.setupUi(this);
}

void SlidedSpinboxWidget::onSpinboxChanged(double val){
	if(mValue != val){
		ui.slider->setValue( getPercentage() * 100 );
		mValue = val;

		emit valueChanged();
	}
}

void SlidedSpinboxWidget::setValueRange(double min, double max){
	setMinValue(min);
	setMaxValue(max);
}

double SlidedSpinboxWidget::getRange() const{
	return mMaxValue - mMinValue;
}

void SlidedSpinboxWidget::onSliderChanged(int sliderVal){
	double val = mMinValue + ( (sliderVal/100.0) * getRange() );

	// Epsilon check
	if( glm::abs(val - mValue) > 0.0001){
		mValue = val;
		ui.spinbox->setValue(val);

		emit valueChanged();
	}
}

double SlidedSpinboxWidget::getPercentage() const{
	return (mValue - mMinValue) / getRange();
}

void SlidedSpinboxWidget::setMaxValue(double value){
	// TODO checks
	mMaxValue = value;
	ui.spinbox->setMaximum(value);
	if(mValue > mMaxValue){
		mValue = mMaxValue;
	}
}

void SlidedSpinboxWidget::setMinValue(double value){
	// TODO checks
	mMinValue = value;
	ui.spinbox->setMinimum(value);
	if(mValue < mMinValue){
		mValue = mMinValue;
	}
}
double SlidedSpinboxWidget::getValue() const{
	return mValue;
}

void SlidedSpinboxWidget::setValue(double value){
	if(mValue != value){
		mValue = value;

		ui.spinbox->setValue(value);
		ui.slider->setValue( getPercentage()*100 );
	}
}