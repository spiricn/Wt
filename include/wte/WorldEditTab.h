#ifndef WTE_WORLDEDITTAB_H
#define WTE_WORLDEDITTAB_H

#include "stdafx.h"

#include <wt/EventManager.h>
#include <wt/ProcessManager.h>

#include <qdockwidget.h>

#include "ui_WorldEdit.h"

#include "wte/ATool.h"


class TerrainEditTool;
class LightEditTool;
class ActorEditTool;
class FogTool;
class GodrayTool;

namespace wt{
	class Assets;
}; // </wt>

class WtEditor;

class WorldEditTab : public QMainWindow, public AToolManager{
	Q_OBJECT;

friend class WtEditor;

protected:
	Ui::WorldEdit ui;

private:
	// TODO There's probably a generalized way of doing this (don't save references to all tools, instead use their interfaces via the ToolList
	TerrainEditTool* mTerrainEditTool;
	LightEditTool* mLightTool;
	ActorEditTool* mActorEditTool;
	FogTool* mFogTool;
	GodrayTool* mGodrayTool;

	wt::AResourceSystem* mAssets;
	wt::Scene* mScene;
	wt::EventManager* mEventManager;
	wt::ProcessManager mProcManager;

	// Main loop timer
	QTimer mTimer;

	typedef QVector<ATool*> ToolList;
	ToolList mTools;

public:
	WorldEditTab(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets, wt::EventManager* evtManager);

	~WorldEditTab();

	void requestFocus(ATool* tool);

	void giveupFocus(ATool* tool);

	void onAssetsLoaded();

	void onBeforeAssetsUnload();

	void onBeforeSceneUnload();

	void onAfterSceneUnload();

	void onAfterAssetsUnload();

	void onSceneLoaded();

private:
	void createToolbar();

	QDockWidget* mPrevDockWidget;
	template<class T>
	void addTool(QString title, T* obj){
		QWidget* widget = dynamic_cast<QWidget*>(obj);
		ATool* tool = dynamic_cast<ATool*>(obj);

		QDockWidget* dock = new QDockWidget(this);

		dock->setFeatures(0x00);
		dock->setWidget(widget);
		dock->setVisible(true);

		dock->setWindowTitle(title);

		addDockWidget(Qt::LeftDockWidgetArea, dock);
		if(mPrevDockWidget){
			this->tabifyDockWidget(mPrevDockWidget, dock);
		}
		mPrevDockWidget = dock;
		mTools.push_back(tool);
	}

protected slots:
	void onScreenshot();
	
	void onSetSkybox();

	void onTimeout();

	void onGLContextCreated();

	void onGrabInputChecked(int);

	void onSetTransClicked();

	void onCameraNewAnimation();

	void onShowTerrainTool(bool);

	void onClearSkybox();

	void onMouseDown(QMouseEvent*);

	void onShowLightEditor(bool);

	void onToggleToolFog();

	void onToggleToolTerrain();

	void onToggleToolLight();

	void onToggleToolActor();

	void onToggleBones();

	void onToggleBoundingBoxes();

	void onToggleGodrayTool();

signals:
	void initialized();

}; // </WorldEditTab>

#endif // </WTE_WORLDEDITTAB_H>