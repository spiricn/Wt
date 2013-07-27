#ifndef WTE_COLORDISPLAYWIDGET_H
#define WTE_COLORDISPLAYWIDGET_H

#include <wt/Color.h>

class ColorDisplayWidget : public QWidget{
Q_OBJECT
public:
    ColorDisplayWidget(QWidget* parent=NULL);

public:
	void setColor(const wt::Color& color);

protected:
	void paintEvent(QPaintEvent* e);

	void mouseReleaseEvent(QMouseEvent* event);

protected slots:

signals:
	void clicked();

private:
	wt::Color mColor;
}; // </ColorDisplayWidget>

#endif // </WTE_COLORDISPLAYWIDGET_H>