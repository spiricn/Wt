#ifndef WTE_HEIGHTMAPMANAGERTAB_H
#define WTE_HEIGHTMAPMANAGERTAB_H

#include "ui_HeightmapManagerTab.h"

#include <wte/ARsrcManagerTab.h>

#include "wte/stdafx.h"

class HeightmapManagerTab : public ARsrcManagerTab{
Q_OBJECT;

public:
	HeightmapManagerTab(QWidget* parent, wt::Assets* assets);

private:
	void onCtxMenuSetUri(RTWItem*);

	Ui::HeightmapManagerTab ui;
	
}; // </HeightmapManagerTab>


#endif // </WTE_HEIGHTMAPMANAGERTAB_H>
