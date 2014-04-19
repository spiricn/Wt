#ifndef WTE_SOUNDMANAGERTAB_H
#define WTE_SOUNDMANAGERTAB_H

#include <wt/SFSound.h>

#include "ui_SoundManagerTab.h"

#include "wte/ARsrcManagerTab.h"

class SoundManagerTab : public ARsrcManagerTab{
Q_OBJECT

public:
    SoundManagerTab(QWidget* parent, wt::AResourceSystem* assets);

private:
	void onCtxMenuSetUri(RTWItem*);

	Ui::SoundManagerTab ui;
}; // </SoundManagerTab>

#endif // </WTE_SOUNDMANAGERTAB_H>