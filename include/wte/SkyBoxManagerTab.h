#ifndef WTE_SKYMANAGERTAB_H
#define WTE_SKYMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include <wt/SkyBox.h>

#include "ui_SkyBoxManager.h"

#include "ARsrcManagerTab.h"

class SkyBoxManagerTab : public ARsrcManagerTab{
	Q_OBJECT;
private:
	Ui::SkyBoxManager ui;

public:
	SkyBoxManagerTab(QWidget* parent, wt::Assets* assets);

	void refreshAll(){
		ui.treeWidget->buildTree<wt::SkyBox>(NULL, getAssets()->getSkyBoxManager());
	}

	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::SkyBox>();
	}

protected:
	


	void onCtxMenuCreateResource(RTWItem* item){
		createResource<wt::SkyBox>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		createResourceGroup<wt::SkyBox>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		deleteItem<wt::SkyBox>(item);
	}

	void onTreeItemActivated(RTWItem* item);

}; // </SkyBoxManagerTab>

#endif // </WTE_SKYMANAGERTAB_H>