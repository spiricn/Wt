#ifndef WTE_SLIDEDSPINBOX_H
#define WTE_SLIDEDSPINBOX_H

#include "ui_SlidedSpinboxWidget.h"

class SlidedSpinboxWidget : public QWidget{
Q_OBJECT

private:
    Ui::SlidedSpinbox ui;

	double mValue;
	double mMinValue;
	double mMaxValue;

public:
    SlidedSpinboxWidget(QWidget* parent=NULL);

	double getValue() const;

	void setMaxValue(double value);

	void setMinValue(double value);

	void setValueRange(double min, double max);

	void setValue(double value);

	double getPercentage() const;

	double getRange() const;

	void setDecimals(qint32 decimals);

	void setSingleStep(double step);

protected slots:

	void onSpinboxChanged(double);

	void onSliderChanged(int);

signals:
	void valueChanged();

}; // </SlidedSpinbox>

#endif // </WTE_SLIDEDSPINBOX_H>