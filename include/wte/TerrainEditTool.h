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

public:
	TerrainEditTool(SceneView* scene, QWidget*, AToolManager*);

	void onToolLostFocus();

	void onToolGainFocus();

	void setTarget(wt::Terrain* terrain);

	void onSceneLoaded();

	void onSceneUnloaded();

protected slots:
	void onCreateNewTerrain();

	void onMouseDown(QMouseEvent*);

	void onMouseUp(QMouseEvent*);

	void onSaveTexture();

	void onSaveHeightmap();

	void onResetTexture();

	void onResetHeightmap();

	void onSceneInitialized();

	void onMouseDrag(float,float,Qt::MouseButton);

	void onBrushActivated();

	void onUpdate(float dt);

private:
	enum BrushMode{
		eELEVATE,
		eLOWER,
		eSMOOTHEN
	}; // </BrushMode>

	Ui::TerrainEdit ui;
	SceneView* mSceneView;
	wt::gl::FrameBuffer mFrameBuffer;
	wt::Terrain* mTerrain;
	wt::Texture2D* mBrushTexture;


	enum EditMode{
		eMODE_SCULPT,
		eMODE_PAINT
	};
	struct EditContext{
		float x;
		float y;
		bool editing;
		EditMode mode;

		EditContext() : x(0.0f), y(0.0f), editing(false), mode(eMODE_SCULPT){
		}
	} mEditContext;

private:
	void editTerrainChunk(wt::Terrain& terrain, uint32_t startRow, uint32_t startCol, uint32_t numRows, uint32_t numCols, float pressure, BrushMode mode);

	void editAt(float x, float y);

}; // </TerrainEditTool>

#endif // </WTE_TERRAINEDITTOOL_H>