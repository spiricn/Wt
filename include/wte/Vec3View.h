#ifndef WTE_VEC3VIEW_H
#define WTE_VEC3VIEW_H

#include "ui_Vec3View.h"

class Vec3View : public QWidget{
Q_OBJECT

private:
    Ui::Vec3View ui;

	glm::vec3 mValue;

public:
    Vec3View(QWidget* parent=NULL);

	float getX() const;

	float getY() const;

	float getZ() const;

	void setValue(const glm::vec3& values);

	glm::vec3 getValue();

	void setX(float x);

	void setY(float y);

	void setZ(float z);

protected slots:
	void onValueChanged();

signals:
	void valueChanged();

}; // </Vec3View>

#endif // </WTE_VEC3VIEW_H>