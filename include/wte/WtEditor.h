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
	WtEditor(QWidget *parent = 0, Qt::WFlags flags = 0);

	~WtEditor();

	 static void logCallback(void* opaque, const tdchar* tag, enum TdTraceLevel level, const tdchar* message);

private:
	MainGLWidget* mMainGLWidget;

	wt::Assets mAssets;
	WorldEditTab* mWorldEdit;
	QSettings* mSettings;

	wt::Renderer mRenderer;
	wt::Scene mScene;

	Ui::WtEditorClass ui;

	QString mWorkspaceFilePath;
	QString mScenePath;

	typedef std::vector<ARsrcManagerTab*> TabList;

	TabList mTabs;

	void addTab(ARsrcManagerTab* tab, const QString& name);

	void unloadLevel();

	void loadLevel(const QString&);

	void loadScene(const QString&);

protected slots:
	void onBtnSaveClick();

	void onNew();

	void onOpen();

	void onLoadScene();

	void onSaveScene();

	void onAssetsLoaded();
	
	void onOpenGLContextCreated();

	void onCreateTerrain();

	void onClearScene();

	void onSetSkybox();

	void onScreenshot();

	void onReload();

	void onSaveSceneAs();
};

#endif // WTEDITOR_H
