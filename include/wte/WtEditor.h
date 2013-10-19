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

	wt::lua::State mLuaState;
	wt::EventManager mEventManager;
	wt::Scene mScene;
	wt::Renderer mRenderer;


	Ui::WtEditorClass ui;

	QString mAssetsFilePath;
	QString mSceneFilePath;
	
	bool mAssetsLoaded, mSceneLoaded;

	typedef std::vector<ARsrcManagerTab*> TabList;

	TabList mTabs;

	void addTab(ARsrcManagerTab* tab, const QString& name);


	void clearScene();


	void loadScene(const QString&);

	void loadAssets(const QString&);

	void saveScene(const QString&);

	void saveAssets(const QString&);

	void unloadAssets();

	void unloadScene();

protected slots:
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


	void onOpenGLContextCreated();
};

#endif // WTEDITOR_H
