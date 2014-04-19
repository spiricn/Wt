#ifndef WTE_SKYMANAGERTAB_H
#define WTE_SKYMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include <wt/SkyBox.h>

#include "ui_SkyBoxManager.h"

#include "ARsrcManagerTab.h"

class SkyBoxManagerTab : public ARsrcManagerTab{
	Q_OBJECT;

public:
	SkyBoxManagerTab(QWidget* parent, wt::AResourceSystem* assets);

private:
	void onTreeItemActivated(RTWItem* item);

	Ui::SkyBoxManager ui;

}; // </SkyBoxManagerTab>

#endif // </WTE_SKYMANAGERTAB_H>