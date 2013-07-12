#ifndef WTE_SOUNDMANAGERTAB_H
#define WTE_SOUNDMANAGERTAB_H

#include <wt/SFSound.h>

#include "ui_SoundManagerTab.h"

#include "wte/ARsrcManagerTab.h"

class SoundManagerTab : public ARsrcManagerTab{
Q_OBJECT

private:
    Ui::SoundManagerTab ui;

public:
    SoundManagerTab(QWidget* parent, wt::Assets* assets);

	void destroyAll(){
		ARsrcManagerTab::destroyAll<wt::ASoundBuffer>();
	}

	void refreshAll(){
		ui.treeWidget->buildTree<wt::ASoundBuffer>(NULL, getAssets()->getSoundManager());
	}

protected:

	void onCtxMenuCreateResource(RTWItem* item){
		createResource<wt::ASoundBuffer>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		createResourceGroup<wt::ASoundBuffer>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		deleteItem<wt::ASoundBuffer>(item);
	}

	void onCtxMenuSetUri(RTWItem*);

protected slots:


}; // </SoundManagerTab>

#endif // </WTE_SOUNDMANAGERTAB_H>