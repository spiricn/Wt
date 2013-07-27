#ifndef WTE_COLOREDITWIDGET_H
#define WTE_COLOREDITWIDGET_H

#include "ui_ColorEditWidget.h"

#include <wt/Color.h>

class QColorDialog;

class ColorEditWidget : public QWidget{
Q_OBJECT
public:
    ColorEditWidget(QWidget* parent=NULL);

	wt::Color getColor() const;

	void setColor(const wt::Color& color);

protected slots:

	void onPickColor();

	void onValueChanged();

	void onColorPicked(const QColor&);

signals:
	void valueChanged();

private:
    Ui::ColorEditWidget ui;

	wt::Color mColor;

	QColorDialog* mColorDialog;

}; // </ColorEditWidget>

#endif // </WTE_COLOREDITWIDGET_H>