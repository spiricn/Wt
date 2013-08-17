#include "stdafx.h"

#include "wte/ColorDisplayWidget.h"

#define TD_TRACE_TAG "ColorDisplayWidget"

ColorDisplayWidget::ColorDisplayWidget(QWidget* parent) : QWidget(parent), mColor(wt::Color::Red()){
	installEventFilter(this);
}

void ColorDisplayWidget::setColor(const wt::Color& color){
	mColor = color;
	update();
}

void ColorDisplayWidget::mouseReleaseEvent(QMouseEvent* event){
	emit clicked();
}

void ColorDisplayWidget::paintEvent(QPaintEvent* e){
	QRectF rect(0, 0, width(), height());
	QColor color = QColor(mColor.red*255, mColor.green*255, mColor.blue*255, 255);

	QPainter painter(this);

	QBrush brush;
	brush.setStyle(Qt::SolidPattern);

	brush.setColor(color);
	painter.fillRect(rect, brush);

	brush.setColor(QColor(0, 0, 0));
	painter.fillRect(QRectF(0, 0, width(), height()*(15/100.0)), brush);
	
	brush.setColor(QColor(255, 255, 255));	
	painter.fillRect(QRectF(0, 0, width() * mColor.alpha, height()*(15/100.0)), brush);
}