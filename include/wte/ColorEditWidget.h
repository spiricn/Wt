#ifndef WTE_COLOREDITWIDGET_H
#define WTE_COLOREDITWIDGET_H

#include "ui_ColorEditWidget.h"

#include <wt/Color.h>

class ColorEditWidget : public QWidget{
Q_OBJECT
public:
    ColorEditWidget(QWidget* parent=NULL);

	wt::Color getColor() const;

	void setColor(wt::Color& color);

protected slots:

	void onPickColor();

	void onValueChanged();

signals:
	void valueChanged();

private:
    Ui::ColorEditWidget ui;

	wt::Color mColor;

}; // </ColorEditWidget>

#endif // </WTE_COLOREDITWIDGET_H>