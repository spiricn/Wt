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

	void destroyAll();

	void refreshAll();

	void onCtxMenuCreateResource(RTWItem* item);

	void onTreeItemActivated(RTWItem* item);

protected slots:

}; // </ParticleManagerTab>

#endif // </WTE_PARTICLEMANAGERTAB_H>