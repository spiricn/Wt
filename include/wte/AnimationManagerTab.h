#ifndef WTE_ANIMATIONMANAGERTAB_H
#define WTE_ANIMATIONMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include <Wt/Animation.h>

#include "ui_AnimationManager.h"

#include "wte/ARsrcManagerTab.h"


class AnimationManagerTab : public ARsrcManagerTab{
Q_OBJECT;
private:
	Ui::AnimationManager ui;

public:
	AnimationManagerTab(QWidget* parent, wt::Assets* assets);

	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::Animation>();
	}

	void refreshAll(){
		ui.treeWidget->buildTree<wt::Animation>(NULL, getAssets()->getAnimationManager());
	}


	void onCtxMenuCreateResource(RTWItem* item){
		createResource<wt::Animation>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		createResourceGroup<wt::Animation>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		deleteItem<wt::Animation>(item);
	}

	void onCtxMenuSetUri(RTWItem*);

}; // </AnimationManagerTab>

#endif // </WTE_ANIMATIONMANAGERTAB_H>