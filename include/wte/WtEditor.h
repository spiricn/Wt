#ifndef WTEDITOR_H
#define WTEDITOR_H

#include "stdafx.h"

#include <QtGui/QMainWindow>
#include <qsettings.h>
#include <wt/ImageManager.h>
#include <wt/DevilImageLoader.h>
#include <wt/Renderer.h>
#include <Wt/Assets.h>

#include "wte/ImageManagerTab.h"
#include "wte/SkyBoxManagerTab.h"
#include "wte/TextureManagerTab.h"
#include "wte/ModelManagerTab.h"
#include "wte/AnimationManagerTab.h"
#include "wte/SoundManagerTab.h"
#include "wte/WorldEditTab.h"
#include "wte/SceneView.h"
#include "wte/MainGLWidget.h"

#include "ui_WtEditor.h"

class WtEditor : public QMainWindow{
Q_OBJECT

public:
	WtEditor(QWidget *parent, Qt::WFlags flags, int argc, char* argv[]);

	~WtEditor();

private:
	static void logCallback(void* opaque, const tdchar* tag, enum TdTraceLevel level, const tdchar* message);

	void updateTitle();

	void addTab(ARsrcManagerTab* tab, const QString& name);

private slots:
	void onAssetsNew();

	void onAssetsOpen();

	void onAssetsSave();

	void onAssetsSaveAs();

	void onAssetsClear();

	void onAssetsReload();

	void onSceneNew();

	void onSceneOpen();

	void onSceneSave();

	void onSceneSaveAs();

	void onSceneClear();

	void onSceneReload();

	void onWorkspaceSwitch();

	void onWorkspaceSave();

	void onOpenGLContextCreated();

	void onAssetsUnloaded();

	void onAssetsLoaded();

private:
	QString mCmdArg;
	MainGLWidget* mMainGLWidget;
	WorldEditTab* mWorldEdit;
	QSettings* mSettings;
	Ui::WtEditorClass ui;
}; // </WtEditor>

#endif // WTEDITOR_H

