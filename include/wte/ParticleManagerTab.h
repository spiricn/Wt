#ifndef WTE_PARTICLEMANAGERTAB_H
#define WTE_PARTICLEMANAGERTAB_H

#include "ui_ParticleManagerTab.h"

#include "wte/ARsrcManagerTab.h"

#include <wt/Assets.h>

class ParticleManagerTab : public ARsrcManagerTab{
Q_OBJECT

private:
    Ui::ParticleManagerTab ui;

public:
    ParticleManagerTab(QWidget* parent, wt::Assets* assets);

	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::ParticleEffectResource>();
	}

	void refreshAll(){
		ui.treeWidget->buildTree<wt::ParticleEffectResource>(NULL, getAssets()->getParticleResourceManager());
	}

private:
	wt::Scene mScene;

protected slots:

}; // </ParticleManagerTab>

#endif // </WTE_PARTICLEMANAGERTAB_H>