#ifndef WTE_GEOMODELMANAGERTAB_H
#define WTE_GEOMODELMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include <Wt/Model.h>

#include "ui_GeoModelManager.h"

#include "wte/ARsrcManagerTab.h"


class ModelManagerTab : public ARsrcManagerTab{
Q_OBJECT;
private:
	Ui::GeoModelManager ui;

public:
	ModelManagerTab(QWidget* parent, wt::Assets* assets);

protected:
	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::Model>();
	}

	void refreshAll(){
		ui.treeWidget->buildTree<wt::Model>(NULL, getAssets()->getModelManager());
	}


	void onCtxMenuCreateResource(RTWItem* item){
		createResource<wt::Model>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		createResourceGroup<wt::Model>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		deleteItem<wt::Model>(item);
	}

	void onCtxMenuSetUri(RTWItem*);

	void onTreeItemActivated(RTWItem* item);

}; // </ModelManagerTab>

#endif // </WTE_GEOMODELMANAGERTAB_H>