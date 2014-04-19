#ifndef WTE_ANIMATIONMANAGERTAB_H
#define WTE_ANIMATIONMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include <Wt/Animation.h>

#include "ui_AnimationManager.h"

#include "wte/ARsrcManagerTab.h"


class AnimationManagerTab : public ARsrcManagerTab{
Q_OBJECT;
public:
	AnimationManagerTab(QWidget* parent, wt::AResourceSystem* assets);

	void onCtxMenuSetUri(RTWItem*);

private:
	Ui::AnimationManager ui;

}; // </AnimationManagerTab>

#endif // </WTE_ANIMATIONMANAGERTAB_H>