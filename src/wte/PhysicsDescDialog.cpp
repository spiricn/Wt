#include "stdafx.h"
#include "wte/PhysicsDescDialog.h"

#include <wt/ModelledActor.h>
#include <wt/PhysicsActor.h>
#include <wt/ASceneActor.h>

#define TD_TRACE_TAG "PhysicsDescDialog"

PhysicsDescDialog::PhysicsDescDialog(QWidget* parent, wt::ASceneActor* actor) : QDialog(parent), mActor(actor) {
	ui.setupUi(this);

	mResult.accepted = false;

	ui.boxExtents->setValue(glm::vec3(1.0f, 1.0f, 1.0f));

	if(mActor && mActor->getPhysicsActor()){
		const wt::PhysicsActor::Desc& desc = mActor->getPhysicsActor()->getDesc();

		ui.isDynamic->setChecked( desc.type == wt::PhysicsActor::eTYPE_DYNAMIC );

		if(desc.geometryType == wt::PhysicsActor::eGEOMETRY_BOX){
			ui.geometry->setCurrentIndex(0);

			ui.boxExtents->setValue(glm::vec3(desc.geometryDesc.boxGeometry.hx,
				desc.geometryDesc.boxGeometry.hy,
				desc.geometryDesc.boxGeometry.hz));
		}
		else if(desc.geometryType == wt::PhysicsActor::eGEOMETRY_SPHERE){
			ui.geometry->setCurrentIndex(1);

			ui.sphereRadius->setValue(desc.geometryDesc.sphereGeometry.radius);
		}
		else if(desc.geometryType == wt::PhysicsActor::eGEOMETRY_MESH){
			ui.geometry->setCurrentIndex(2);
		}
		else{
			WT_THROW("Unsupported geometry type");
		}
	}
}

PhysicsDescDialog::~PhysicsDescDialog(){
}

void PhysicsDescDialog::onGeometryChanged(int idx){
	ui.stackedWidget->setCurrentIndex(idx);
}


PhysicsDescDialog::Result PhysicsDescDialog::edit(QWidget* parent, wt::ASceneActor* actor){
	PhysicsDescDialog dlg(parent, actor);

	dlg.exec();

	return dlg.mResult;
}

void PhysicsDescDialog::onAccept(){
	mResult.accepted = true;

	wt::PhysicsActor::Desc& pd = mResult.desc;
	pd.controlMode = wt::PhysicsActor::eCTRL_MODE_PHYSICS;
	pd.type = ui.isDynamic->isChecked() ? wt::PhysicsActor::eTYPE_DYNAMIC : ui.isStatic->isChecked() ? wt::PhysicsActor::eTYPE_STATIC : wt::PhysicsActor::eTYPE_REGION;

	int geometry = ui.geometry->currentIndex();

	// Box geometry
	if(geometry == 0){
		pd.geometryType = wt::PhysicsActor::eGEOMETRY_BOX;
			
		pd.geometryDesc.boxGeometry.hx = ui.boxExtents->getX();
		pd.geometryDesc.boxGeometry.hy = ui.boxExtents->getY();
		pd.geometryDesc.boxGeometry.hz = ui.boxExtents->getZ();
	}
	// Sphere geometry
	else if(geometry == 1){
		pd.geometryType = wt::PhysicsActor::eGEOMETRY_SPHERE;

		pd.geometryDesc.sphereGeometry.radius = ui.sphereRadius->value();
	}
	// Mesh geometry
	else if(geometry == 2){
		WT_ASSERT(mActor && mActor->getActorType() == wt::ASceneActor::eTYPE_MODELLED, "Actor not set");

		pd.geometryType = wt::PhysicsActor::eGEOMETRY_MESH;
		pd.geometryDesc.meshGeometry.model = dynamic_cast<wt::ModelledActor*>(mActor)->getModel();
	}
	else{
		pd.geometryType = wt::PhysicsActor::eGEOMETRY_INVALID;
	}

	close();
}