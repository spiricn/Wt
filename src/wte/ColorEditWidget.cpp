#include "stdafx.h"

#include <qcolordialog.h>

#include "wte/ColorEditWidget.h"

#define TD_TRACE_TAG "ColorEditWidget"

ColorEditWidget::ColorEditWidget(QWidget* parent) : QWidget(parent), mColorDialog(NULL){
    ui.setupUi(this);

	ui.color->setMode(VecEditWidget::eMODE_VEC4);

	mColorDialog = new QColorDialog(QColor(mColor.mRed*255, 
			mColor.mGreen*255, 
			mColor.mBlue*255, 
			mColor.mAlpha*255), this);

	mColorDialog->setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog | QColorDialog::NoButtons);
	mColorDialog->setModal(false);

	connect(mColorDialog, SIGNAL(currentColorChanged(const QColor&)),
		this, SLOT(onColorPicked(const QColor&)));
}

wt::Color ColorEditWidget::getColor() const{
	return mColor;
}

void ColorEditWidget::onColorPicked(const QColor& color){
	setColor( wt::Color(color.red()/255.0, 
		color.green()/255.0, 
		color.blue()/255.0, 
		color.alpha()/255.0) );
}


void ColorEditWidget::setColor(const wt::Color& color){
	if(mColor != color){
		mColor = color;

		ui.color->blockSignals(true);
		
		ui.color->setValue(glm::vec4(color.mRed, color.mGreen, color.mBlue, color.mAlpha));

		mColorDialog->setCurrentColor( QColor(color.mRed*255, color.mGreen*255, color.mBlue*255, color.mAlpha*255));

		ui.color->blockSignals(false);

		emit valueChanged();
	}
}

void ColorEditWidget::onPickColor(){
	mColorDialog->show();
}

void ColorEditWidget::onValueChanged(){
	glm::vec4 color;
	ui.color->getValue(color);

	if(mColor.asVec4() != color){
		setColor( wt::Color(color) );
	}
}
