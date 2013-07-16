#ifndef WTE_WORLDEDITTAB_H
#define WTE_WORLDEDITTAB_H

#include "stdafx.h"

#include "ui_WorldEdit.h"

#include "wte/ATool.h"

class TerrainEditTool;
class LightEditTool;
class ActorEditTool;
class FogTool;

namespace wt{

	class Assets;

}; // </wt>



struct World{
	//wt::EventManager* eventManager;
	wt::Scene* scene;	
	wt::Physics* physics;	
};

class WorldEditTab : public QMainWindow, public AToolManager{
	Q_OBJECT;
private:
	Ui::WorldEdit ui;

	wt::ASceneActor* mSelectedActor;
	wt::math::Camera mCamera;
	TerrainEditTool* mTerrainEditTool;
	LightEditTool* mLightTool;
	ActorEditTool* mActorEditTool;
	FogTool* mFogTool;
	wt::AResourceSystem* mAssets;
	wt::Scene* mScene;

	QTimer mTimer;

	typedef QVector<ATool*> ToolList;
	ToolList mTools;

public:
	WorldEditTab(QWidget* parent, wt::Scene* scene, wt::AResourceSystem* assets);

	~WorldEditTab();

	void requestFocus(ATool* tool){
		for(ToolList::Iterator i=mTools.begin(); i!=mTools.end(); i++){
			if(*i != tool){
				(*i)->onToolLostFocus();
			}
		}

		tool->onToolGainFocus();
	}

	void giveupFocus(ATool* tool){
		tool->onToolLostFocus();
	}

private:
	void createToolbar();


// TODO protect these
public slots:
	void onScreenshot();
	
	void onSetSkybox();

	void onClearAll();

	void onCreateTerrain();

protected slots:
	void onTimeout();

	void onGLContextCreated();

	void onGrabInputChecked(int);

	void onSetTransClicked();

	void onTreeItemActivated(void*);

	void onRotSliderMoved(int);
	
	void onShowTerrainTool(bool);

	void onMouseDown(QMouseEvent*);

	void onShowLightEditor(bool);

	void onSave();

	void onToggleToolTerrain();

	void onToggleToolLight();

	void onToggleToolActor();

	void onToggleBones();

	void onToggleBoundingBoxes();
public:

	void loadResources(const QString&);

	void loadScene(const QString&);

	void saveScene(const QString&);

	void unloadLevel();

signals:
	void assetsLoaded();

	void sceneLoaded();

	void onTerrainCreated();

	void initialized();

}; // </WorldEditTab>

#endif // </WTE_WORLDEDITTAB_H>