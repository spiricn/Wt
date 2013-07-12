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

	float getX() const{
		return ui.x->value();
	}

	float getY() const{
		return ui.y->value();
	}

	float getZ() const{
		return ui.z->value();
	}

	void setValue(const glm::vec3& values){
		setX(values.x);
		setY(values.y);
		setZ(values.z);
	}

	glm::vec3 getValue(){
		return glm::vec3(getX(), getY(), getZ());
	}

	void setX(float x){
		ui.x->setValue(x);
	}

	void setY(float y){
		ui.y->setValue(y);
	}

	void setZ(float z){
		ui.z->setValue(z);
	}

protected slots:
	void onValueChanged(double);

signals:
	void onValueChanged();

}; // </Vec3View>

#endif // </WTE_VEC3VIEW_H>