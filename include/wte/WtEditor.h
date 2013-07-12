#ifndef WTEDITOR_H
#define WTEDITOR_H

#include "stdafx.h"

#include <QtGui/QMainWindow>
#include <qsettings.h>
#include <wt/ImageManager.h>
#include <wt/DevilImageLoader.h>
#include <Wt/Assets.h>
#include <wt/game/GameLevel.h>

#include "wte/ImageManagerTab.h"
#include "wte/SkyBoxManagerTab.h"
#include "wte/TextureManagerTab.h"
#include "wte/ModelManagerTab.h"
#include "wte/AnimationManagerTab.h"
#include "wte/SoundManagerTab.h"
#include "wte/WorldEditTab.h"
#include "wte/SceneView.h"

#include "ui_WtEditor.h"

class WtEditor : public QMainWindow{
	Q_OBJECT
public:
	WtEditor(QWidget *parent = 0, Qt::WFlags flags = 0);

	~WtEditor();

	 static void logCallback(void* opaque, const tdchar* tag, enum TdTraceLevel level, const tdchar* message);

private:
	
	wt::Assets mAssets;
	WorldEditTab* mWorldEdit;
	wt::GameLevel* mGameLevel;
	QSettings* mSettings;
	wt::EntityManager mEntityManager;

	wt::Scene mScene;

	Ui::WtEditorClass ui;

	QString mWorkspaceFilePath;

	typedef std::vector<ARsrcManagerTab*> TabList;

	TabList mTabs;

	void addTab(ARsrcManagerTab* tab, const QString& name);

	void unloadLevel();

	void loadLevel(const QString&);

protected slots:
	void onBtnSaveClick();

	void onNew();

	void onOpen();

	void onAssetsLoaded();
	
	void onInitialized();

	void onReload();
};

#endif // WTEDITOR_H
