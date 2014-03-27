#ifndef WTE_SCRIPTMANAGERTAB_H
#define WTE_SCRIPTMANAGERTAB_H

#include "ui_ScriptManagerTab.h"

#include "wte/ARsrcManagerTab.h"
#include "wte/Scripter.h"

#include "stdafx.h"

class ScriptManagerTab : public ARsrcManagerTab{
Q_OBJECT;

public:
	ScriptManagerTab(QWidget* parent, wt::Assets* assets);

private:
	void onCtxMenuSetUri(RTWItem*);

	void onTreeItemActivated(RTWItem* item);

	Ui::ScriptManagerTab ui;

	Scripter* mScripterDlg;
	
}; // </ScriptManagerTab>

#endif // </WTE_SCRIPTMANAGERTAB_H>
