#ifndef WTE_GEOMODELMANAGERTAB_H
#define WTE_GEOMODELMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include <Wt/Model.h>

#include "ui_GeoModelManager.h"

#include "wte/ARsrcManagerTab.h"


class ModelManagerTab : public ARsrcManagerTab{
Q_OBJECT;
public:
	ModelManagerTab(QWidget* parent, wt::Assets* assets);

private:
	void onCtxMenuSetUri(RTWItem*);

	void onTreeItemActivated(RTWItem* item);

	Ui::GeoModelManager ui;
}; // </ModelManagerTab>

#endif // </WTE_GEOMODELMANAGERTAB_H>