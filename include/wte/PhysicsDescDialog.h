#ifndef WTE_PHYSICSDESCDIALOG_H
#define WTE_PHYSICSDESCDIALOG_H

#include "ui_PhysicsDescDialog.h"

#include "stdafx.h"

#include <wt/PhysicsActor.h>

class PhysicsDescDialog : public QDialog{
Q_OBJECT;
public:
	struct Result{
		bool accepted;
		wt::PhysicsActor::Desc desc;
	}; // </Result>

public:
	static Result edit(QWidget* parent, wt::ASceneActor* actor);

private:
	PhysicsDescDialog(QWidget* parent, wt::ASceneActor* actor);

	~PhysicsDescDialog();

private slots:

	void onGeometryChanged(int idx);

	void onAccept();

private:
	Result mResult;
	wt::ASceneActor* mActor;

	Ui::PhysicsDescDialog ui;
	
}; // </PhysicsDescDialog>

#endif // </WTE_PHYSICSDESCDIALOG_H>
