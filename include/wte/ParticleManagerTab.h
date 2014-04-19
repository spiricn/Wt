#ifndef WTE_PARTICLEMANAGERTAB_H
#define WTE_PARTICLEMANAGERTAB_H

#include "ui_ParticleManagerTab.h"

#include "wte/ARsrcManagerTab.h"

class ParticleManagerTab : public ARsrcManagerTab{
Q_OBJECT

public:
    ParticleManagerTab(QWidget* parent, wt::AResourceSystem* assets);

private:
	void onTreeItemActivated(RTWItem* item);

	Ui::ParticleManagerTab ui;

}; // </ParticleManagerTab>

#endif // </WTE_PARTICLEMANAGERTAB_H>