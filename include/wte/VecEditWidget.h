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
	}; // </Mode>

	enum Field{
		eFIELD_X = 1 << 0,
		eFIELD_Y = 1 << 1,
		eFIELD_Z = 1 << 2,
		eFIELD_W = 1 << 3,
		eFIELD_ALL = eFIELD_X|eFIELD_Y|eFIELD_Z|eFIELD_W
	}; // </Field>

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

	void setSingleStep(double step, Field field=eFIELD_ALL);

	void setDecimals(uint32_t num, Field field=eFIELD_ALL);

	void setValueRange(double min, double max, Field field=eFIELD_ALL);

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

inline VecEditWidget::Field operator|(const VecEditWidget::Field& a, const VecEditWidget::Field& b){
	return static_cast<VecEditWidget::Field>( static_cast<int>(a) | static_cast<int>(b) );
}

#endif // </WTE_VEC3VIEW_H>