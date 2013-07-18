#ifndef WTE_GODRAYTOOL_H
#define WTE_GODRAYTOOL_H

#include "ui_GodrayTool.h"

#include <wt/Scene.h>
#include <wt/AResourceSystem.h>

class GodrayTool : public QWidget{
Q_OBJECT

public:
    GodrayTool(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets);

protected slots:

	void onParamChanged();

	void onPickTexture();

	void onAssetsLoaded();

	void onReset();

private:
    Ui::GodrayTool ui;
	wt::Scene* mScene;
	wt::AResourceSystem* mAssets;
	bool mInitializing;

	void loadParams();
}; // </GodrayTool>

#endif // </WTE_GODRAYTOOL_H>