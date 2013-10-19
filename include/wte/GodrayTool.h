#ifndef WTE_GODRAYTOOL_H
#define WTE_GODRAYTOOL_H

#include "ui_GodrayTool.h"

#include <wt/Scene.h>
#include <wt/AResourceSystem.h>

#include "wte/ATool.h"

class GodrayTool : public QWidget, public ATool{
Q_OBJECT

public:
    GodrayTool(QWidget* parent, AToolManager* manager, wt::Scene* scene, wt::AResourceSystem* assets);

protected slots:

	void onParamChanged();

	void onPickTexture();

	void onSceneLoaded();

	void onReset();

private:
    Ui::GodrayTool ui;
	wt::Scene* mScene;
	wt::AResourceSystem* mAssets;
	bool mInitializing;

	void loadParams();
}; // </GodrayTool>

#endif // </WTE_GODRAYTOOL_H>