#ifndef WTE_TEXTUREMANAGERTAB_H
#define WTE_TEXTUREMANAGERTAB_H

#include <qmenu.h>

#include <Wt/Texture2D.h>

#include "ui_TextureManager.h"

#include "stdafx.h"
#include "wte/ARsrcManagerTab.h"

class TextureManagerTab : public ARsrcManagerTab{
Q_OBJECT;
private:
	Ui::TextureManager ui;

public:
	TextureManagerTab(QWidget* parent, wt::Assets* assets);

protected:

	void refreshAll(){
		ui.treeWidget->buildTree<wt::Texture2D>(NULL, getAssets()->getTextureManager());

		ui.treeWidget->setFilter("");
		setTreeWidget(ui.treeWidget);
	}

	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::Texture2D>();
	}


	void onCtxMenuCreateResource(RTWItem* item){
		createResource<wt::Texture2D>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		createResourceGroup<wt::Texture2D>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		deleteItem<wt::Texture2D>(item);
	}

	void onCtxMenuSetUri(RTWItem*);

	void dropEvent(QDropEvent* evt);

}; // </TextureManagerTab >

#endif // </WTE_TEXTUREMANAGERTAB_H>