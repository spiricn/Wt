#include "stdafx.h"

#include <qcolordialog.h>

#include "wte/ColorEditWidget.h"

#define TD_TRACE_TAG "ColorEditWidget"

ColorEditWidget::ColorEditWidget(QWidget* parent) : QWidget(parent){
    ui.setupUi(this);

	ui.color->setMode(VecEditWidget::eMODE_VEC4);
}

wt::Color ColorEditWidget::getColor() const{
	return mColor;
}

void ColorEditWidget::setColor(wt::Color& color){
	if(mColor != color){
		mColor = color;

		ui.color->blockSignals(true);
		
		ui.color->setValue(glm::vec4(color.mRed, color.mGreen, color.mBlue, color.mAlpha));

		ui.color->blockSignals(false);

		emit valueChanged();
	}
}

void ColorEditWidget::onPickColor(){
	QColorDialog dlg(QColor(mColor.mRed*255, 
		mColor.mGreen*255, 
		mColor.mBlue*255, 
		mColor.mAlpha*255), this);

	if(dlg.exec() != QColorDialog::Accepted){
		return;
	}

	QColor clr = dlg.selectedColor();

	setColor( wt::Color(clr.red()/255.0f, clr.green()/255.0f, clr.blue()/255.0f, clr.alpha()/255.0f) );
}

void ColorEditWidget::onValueChanged(){
	glm::vec4 color;
	ui.color->getValue(color);

	if(mColor.asVec4() != color){
		setColor( wt::Color(color) );
	}
}
