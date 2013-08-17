#ifndef WTE_VEC3VIEW_H
#define WTE_VEC3VIEW_H

#include "ui_VecEditWidget.h"

class VecEditWidget : public QWidget{
Q_OBJECT

public:
	enum Mode{
		eMODE_VEC2,
		eMODE_VEC3,
		eMODE_VEC4
	};

public:
    VecEditWidget(QWidget* parent=NULL);

	void setMode(Mode mode);

	double getX() const;

	double getY() const;

	double getZ() const;

	double getW() const;

	void setValue(const glm::vec2& values);

	void setValue(const glm::vec3& values);

	void setValue(const glm::vec4& values);

	void getValue(glm::vec2& dst) const;

	void getValue(glm::vec3& dst) const;

	void getValue(glm::vec4& dst) const;

	void setX(double x);

	void setY(double y);

	void setZ(double z);

	void setW(double z);

	void setSingleStep(double step);

	void setDecimals(uint32_t num);

	void setValueRange(double min, double max);

private:
	Ui::VecEditWidget ui;

	glm::vec4 mValue;

	Mode mMode;

	void setValue(double x, double y, double z=0.0, double w=0.0);

protected slots:
	void onValueChanged();

signals:
	void valueChanged();

}; // </Vec3View>

#endif // </WTE_VEC3VIEW_H>