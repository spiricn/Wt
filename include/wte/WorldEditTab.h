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
public:
	WorldEditTab(QWidget* parent);

	~WorldEditTab();

	void requestFocus(ATool* tool);

	void giveupFocus(ATool* tool);

	void onAssetsLoaded();

	void onBeforeAssetsUnload();

	void onBeforeSceneUnload();

	void onAfterSceneUnload();

	void onAfterAssetsUnload();

private:
	Ui::WorldEdit ui;

	// TODO There's probably a generalized way of doing this (don't save references to all tools, instead use their interfaces via the ToolList
	TerrainEditTool* mTerrainEditTool;
	LightEditTool* mLightTool;
	ActorEditTool* mActorEditTool;
	FogTool* mFogTool;
	GodrayTool* mGodrayTool;

	wt::ProcessManager mProcManager;

	typedef QVector<ATool*> ToolList;
	ToolList mTools;
	QDockWidget* mPrevDockWidget;

	void createToolbar();

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
		
		// Stack widgets
		if(mPrevDockWidget){
			this->tabifyDockWidget(mPrevDockWidget, dock);
		}
		mPrevDockWidget = dock;

		mTools.push_back(tool);
	}

	friend class WtEditor;

protected slots:
	void onScreenshot();
	
	void onSetSkybox();

	void onUpdate(float);

	void onGLContextCreated();

	void onGrabInputChecked(int);

	void onSetTransClicked();

	void onCameraNewAnimation();

	void onShowTerrainTool(bool);

	void onClearSkybox();

	void onMouseDown(QMouseEvent*);

	void onToggleBones();

	void onToggleBoundingBoxes();

	void onSceneLoaded();

	void onExecuteScript();

signals:
	void initialized();

}; // </WorldEditTab>

#endif // </WTE_WORLDEDITTAB_H>