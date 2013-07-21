#include "stdafx.h"

#include "wte/ParticleManagerTab.h"

#define TD_TRACE_TAG "ParticleManagerTab"

ParticleManagerTab::ParticleManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets, assets->getParticleResourceManager(), false),
	mScene(NULL, assets){

    ui.setupUi(this);
	ui.sceneView->setScene(&mScene);

	mScene.setSkyBox( assets->getSkyBoxManager()->find("valley") );
}
