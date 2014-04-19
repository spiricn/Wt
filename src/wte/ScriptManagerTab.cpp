#include "stdafx.h"
#include "wte/ScriptManagerTab.h"

#define TD_TRACE_TAG "ScriptManagerTab"

ScriptManagerTab::ScriptManagerTab(QWidget* parent, wt::AResourceSystem* assets) : ARsrcManagerTab(parent, assets, assets->getScriptManager(), true){
	ui.setupUi(this);

	initialize<wt::ScriptResource>(ui.treeWidget);

	mScripterDlg = new Scripter;
}


void ScriptManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open script"), "", tr("Lua script files(*.lua)"));

	static_cast<wt::ScriptResource*>(item->userData)->setUri( WTE_CTX.getAssets()->getRelativeURI( path.toStdString() ) );
}


void ScriptManagerTab::onTreeItemActivated(RTWItem* item){
	mScripterDlg->open( static_cast<wt::ScriptResource*>(item->userData) );
	mScripterDlg->show();
}