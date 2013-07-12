#ifndef WTE_IMAGEMANAGERTAB_H
#define WTE_IMAGEMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include "ui_ImageManager.h"

#include "wte/ARsrcManagerTab.h"

class ImageManagerTab : public ARsrcManagerTab{
Q_OBJECT;

private:
	Ui::ImageManager ui;

public:
	ImageManagerTab(QWidget* parent, wt::Assets* assets);

	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::Image>();
	}

	void refreshAll(){
		ui.treeWidget->buildTree<wt::Image>(NULL, getAssets()->getImageManager());
	}

protected:
	void onCtxMenuCreateResource(RTWItem* item){
		createResource<wt::Image>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		createResourceGroup<wt::Image>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		deleteItem<wt::Image>(item);
	}

	void onCtxMenuSetUri(RTWItem*);

}; // </ImageManagerTab>

#endif // </WTE_IMAGEMANAGERTAB_H>