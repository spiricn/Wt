#ifndef WTE_TERRAINEDITTOOL_H
#define WTE_TERRAINEDITTOOL_H

#include <qdialog.h>
#include <wt/FrameBuffer.h>

#include "ui_TerrainEditor.h"

#include "wte/ResourcePickerDialog.h"
#include "wte/SceneView.h"
#include "wte/ATool.h"

class TerrainEditTool : public QDialog, public ATool{
Q_OBJECT;

private:
	static const char* TAG;

	enum BrushMode{
		eELEVATE,
		eLOWER,
		eSMOOTHEN
	};

	Ui::TerrainEdit ui;
	SceneView* mSceneView;
	wt::Scene* mScene;
	wt::Physics* mPhysics;
	wt::Gl::FrameBuffer mFrameBuffer;
	wt::Assets* mAssets;

public:
	TerrainEditTool(SceneView* scene, QWidget*, AToolManager*, wt::Scene*, wt::Assets*);

	void onToolLostFocus(){
		ATool::onToolLostFocus();
		ui.buttonBrush->setText("Activate brush");
	}

	void onToolGainFocus(){
		ATool::onToolGainFocus();
		ui.buttonBrush->setText("Deactivate Brush");
	}

private:
	void editTerrainChunk(wt::Terrain& terrain, wt::Uint32 startRow, wt::Uint32 startCol, wt::Uint32 numRows, wt::Uint32 numCols, float pressure, BrushMode mode);

	void editAt(float x, float y);

protected slots:
	void onMouseDown(QMouseEvent*);

	void onSaveTexture();

	void onSaveHeightmap();

	void onResetTexture();

	void onResetHeightmap();

	void onSceneInitialized();

	void onMouseDrag(float,float,Qt::MouseButton);

	void onBrushActivated();

}; // </TerrainEditTool>

#endif // </WTE_TERRAINEDITTOOL_H>