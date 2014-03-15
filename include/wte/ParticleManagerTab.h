#ifndef WTE_PARTICLEMANAGERTAB_H
#define WTE_PARTICLEMANAGERTAB_H

#include "ui_ParticleManagerTab.h"

#include "wte/ARsrcManagerTab.h"

#include <wt/Assets.h>

class ParticleManagerTab : public ARsrcManagerTab{
Q_OBJECT

public:
    ParticleManagerTab(QWidget* parent, wt::Assets* assets);

private:
	void onTreeItemActivated(RTWItem* item);

	Ui::ParticleManagerTab ui;

}; // </ParticleManagerTab>

#endif // </WTE_PARTICLEMANAGERTAB_H>