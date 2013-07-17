#ifndef WTE_TRANSFORMEDIT_H
#define WTE_TRANSFORMEDIT_H

#include "ui_TransformEdit.h"

class TransformEdit : public QWidget{
Q_OBJECT

private:
    Ui::TransformEdit ui;

	glm::vec3 mTranslation;

	glm::vec3 mRotAxis;

	float mRotAngle;

	glm::vec3 mScale;

public:
    TransformEdit(QWidget* parent=NULL);

	glm::vec3 getScale() const;

	float getRotAngle() const;

	glm::vec3 getRotAxis() const;

	glm::vec3 getTranslation() const;

	void setScale(const glm::vec3& scale);

	void setTranslation(const glm::vec3& translation);

	void setRotation(const glm::vec3& axis, float angle);

	glm::quat getRotation() const;

	void setRotation(const glm::quat& rot);

protected slots:
	void onRotAngleChanged();

	void onRotAxisChanged();

	void onScaleChanged();

	void onTranslationChanged();

signals:
	void translationChanged();

	void rotationChanged();

	void scaleChanged();
}; // </TransformEdit>

#endif // </WTE_TRANSFORMEDIT_H>