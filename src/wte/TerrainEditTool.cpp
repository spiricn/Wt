#include "stdafx.h"

#include <qmessagebox.h>

#include "wte/TerrainEditTool.h"
#include "wte/HeightmapCreateDialog.h"
#include "wte/TerrainEditDialog.h"
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "TerrainEditTool"

TerrainEditTool::TerrainEditTool(SceneView* sceneView, QWidget* parent, AToolManager* toolManager) 
	: QDialog(parent), mSceneView(sceneView), ATool(toolManager), mTerrain(NULL), mBrushTexture(NULL){
	ui.setupUi(this);
	setWindowFlags(Qt::Tool);

	connect(mSceneView, SIGNAL(onMouseDown(QMouseEvent*)),
		 this, SLOT(onMouseDown(QMouseEvent*)));

	connect(mSceneView, SIGNAL(onMouseDrag(float,float,Qt::MouseButton)),
		this, SLOT(onMouseDrag(float,float,Qt::MouseButton)));

	connect(mSceneView, SIGNAL(onMouseUp(QMouseEvent*)),
		 this, SLOT(onMouseUp(QMouseEvent*)));
}

void TerrainEditTool::onToolLostFocus(){
	ATool::onToolLostFocus();
	ui.buttonBrush->setText("Activate brush");
}

void TerrainEditTool::onToolGainFocus(){
	ATool::onToolGainFocus();
	ui.buttonBrush->setText("Deactivate Brush");
}

void TerrainEditTool::setTarget(wt::Terrain* terrain){
	mTerrain = terrain;

	if(mFrameBuffer.isCreated()){
		mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0, (GLuint)0, GL_TEXTURE_2D);
		mFrameBuffer.destroy();
	}

	mFrameBuffer.create();

	mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0, mTerrain->getMapTexture());

	WT_ASSERT(mFrameBuffer.isComplete(), "Incomplete framebuffer");

	mTerrain->getMapTexture()->generateMipmap();

	mFrameBuffer.unbind(wt::gl::FrameBuffer::eMODE_DRAW);
}

void TerrainEditTool::onSceneLoaded(){
	if(mBrushTexture == NULL){
		// First scene loaded, create a brush texture
		mBrushTexture = new wt::Texture2D;

		// TODO acquire this texture some other way
		WTE_CTX.getAssets()->getTextureManager()->getLoader()->load("assets/images/brushes/circle_hard.png", mBrushTexture);
	}

	// TODO upon terrain loading scene target texture probably needs to be reloaded
	wt::Terrain* terrain = NULL;
	// Find terrain
	for(wt::Scene::ActorMap::iterator iter=WTE_CTX.getScene()->getActorMap().begin(); iter!=WTE_CTX.getScene()->getActorMap().end(); iter++){
		if(iter->second->getActorType() == wt::ASceneActor::eTYPE_TERRAIN){
			terrain = static_cast<wt::Terrain*>(iter->second);
			break;
		}
	}

	if(terrain){
		setTarget(terrain);
	}
	else{
		LOGW("No terrain found in scene");
	}
}

void TerrainEditTool::onSceneUnloaded(){
	// TODO
	mTerrain = NULL;
}


void TerrainEditTool::onSaveTexture(){
	if(mTerrain == NULL){
		TRACEE("Target not set");
		return;
	}

	QString path = mTerrain->getMapTexture()->getUri().c_str();

	if(path.isEmpty()){
		path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+"/terrain.tga");
	}

	if(QMessageBox::question(this, "Confirmation", "Save terrain texture to this location ?\n\n" + path, QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes){
		return;
	}

	wt::StreamPtr stream = WTE_CTX.getAssets()->getFileSystem()->open(path.toStdString(), wt::AIOStream::eMODE_WRITE);

	mTerrain->getMapTexture()->dump( *stream.get() );

	LOGI("Terrain texture saved to \"%s\"", path.toStdString().c_str());
}

void TerrainEditTool::onSaveHeightmap(){
	if(mTerrain == NULL){
		TRACEE("Target not set");
		return;
	}

	QString path = mTerrain->getHeightmap()->getUri().c_str();

	if(path.isEmpty()){
		path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+QString("terrain.wtt"));
	}

	if(QMessageBox::question(this, "Confirmation", "Save terrain heightmap to this location ?\n\n" + path, QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes){
		return;
	}

	wt::StreamPtr stream = WTE_CTX.getAssets()->getFileSystem()->open(
		mTerrain->getHeightmap()->getUri(), wt::AIOStream::eMODE_WRITE);

	WTE_CTX.getAssets()->getHeightmapManager()->getLoader()->save(
		stream, mTerrain->getHeightmap());

	LOGI("Terrain heightmap saved to \"%s\"", path.toStdString().c_str());
}

void TerrainEditTool::onResetTexture(){
	if(mTerrain == NULL){
		TRACEE("Target not set");
		return;
	}

	if(QMessageBox::question(this, "Confirmation", "Reset texture ?", QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes){
		return;
	}

	LOGI("Resetting texture...");

	mFrameBuffer.bind(wt::gl::FrameBuffer::eMODE_DRAW);

	static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, bfrMap);

	wt::Texture2D* tex = mTerrain->getMapTexture();

	glClearColor(ui.materialIndex->currentIndex()==0, 
		ui.materialIndex->currentIndex()==1, 
		ui.materialIndex->currentIndex()==2, 
		ui.pressure->value()/100.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	tex->generateMipmap();
}

void TerrainEditTool::onResetHeightmap(){
	if(!mTerrain){
		TRACEE("Terrain object not set");
		return;
	}

	if(QMessageBox::question(this, "Confirmation", "Reset heightmap ?", QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes){
		return;
	}

	wt::Buffer<int16_t> bfr;

	
	uint32_t nc = mTerrain->getHeightmap()->getNumColumns();
	uint32_t nr = mTerrain->getHeightmap()->getNumRows();

	bfr.create(nc*nr);

	bfr.clearMem();

	mTerrain->editChunk(
		bfr, 0, 0, nr, nc);
}

void TerrainEditTool::onMouseDrag(float x, float y, Qt::MouseButton btn){
	if(isToolFocused() && btn == Qt::LeftButton){
		editAt(x, y);
	}
}

void TerrainEditTool::onBrushActivated(){
	if(isToolFocused()){
		getToolManager()->giveupFocus(this);
	}
	else{
		getToolManager()->requestFocus(this);
	}
}

void TerrainEditTool::onSceneInitialized(){
}

void TerrainEditTool::editTerrainChunk(wt::Terrain& terrain, uint32_t startRow, uint32_t startCol,
	uint32_t numRows, uint32_t numCols, float pressure, BrushMode mode){

	if(mTerrain == NULL){
		TRACEE("Target not set");
		return;
	}

	WT_ASSERT(pressure >= 0.0f && pressure <= 1.0f, "Invalid pressure value %f", pressure);

	// New heightmap samples
	wt::Buffer<int16_t> samples;
	samples.create(numRows*numCols);

	// Old heightmap
	wt::Heightmap& heightmap = *terrain.getHeightmap();

	const uint32_t totalRows = heightmap.getNumRows();

	const uint32_t totalCols = heightmap.getNumColumns();

	// Max distance from center
	const float maxDistance = glm::length( glm::vec2(numRows/2.0, numCols/2.0) );

	const glm::vec2 center(startRow + numRows/2.0f, startCol + numCols/2.0f);


//#define DEBUG_DUMP

#ifdef DEBUG_DUMP
	FILE* f1 = fopen("factor.txt", "wb");
	FILE* f2 = fopen("before.txt", "wb");
	FILE* f3 = fopen("after.txt", "wb");
#endif

	for(uint32_t row=startRow; row<startRow+numRows; row++){
		for(uint32_t col=startCol; col<startCol+numCols; col++){
			// Current height at the given point
			const int16_t currentHeight = heightmap.getSamples()[row*totalCols + col];

			// Resulting height
			int16_t finalSample=0;

			// Normalized distance from center (1 being the closest, and 0 the farthest)
			const float factor = 1.0f - ( glm::length(center - glm::vec2(row, col)) / maxDistance );

			// Minimum height delta
			const float quantFactor = heightmap.getHeightScale();

			switch(mode){
			case eELEVATE:
			case eLOWER:{
					int16_t minDelta = 1;
					int16_t maxDelta = 20;

					int16_t delta =  factor * ( minDelta + pressure*(maxDelta-minDelta) );

					delta *= mode == eELEVATE ? 1 : -1;

					finalSample = currentHeight + delta;
				}
				break;

			case eSMOOTHEN:{
				static const uint32_t kernelSize= 4;
				uint32_t numSamples = 0;
				int16_t sum = 0;

				for(int32_t i=row-kernelSize/2; i<row+kernelSize/2; i++){
					for(int32_t j=col-kernelSize/2; j<col+kernelSize/2; j++){
						if(i < 0 || j < 0 || i >=heightmap.getNumRows() || j >= heightmap.getNumColumns()){
							continue;
						}

						sum += heightmap.getSamples()[i*totalCols + j];
						++numSamples;
					}
				}

				if(numSamples == 0){
					finalSample = currentHeight;
				}
				else{
					int16_t avgHeight = static_cast<int16_t>( glm::floor( static_cast<float>(sum) / numSamples ) );

					finalSample = static_cast<int16_t>( glm::floor( glm::mix(static_cast<float>(currentHeight), static_cast<float>(avgHeight), factor) ) );
				}
	
				break;
				}
			}

			samples[(row-startRow)*numCols + (col-startCol)] = finalSample;

#ifdef DEBUG_DUMP
			fprintf(f1, "%.2f ", factor);
			fprintf(f2, "%d ", currentHeight);
			fprintf(f3, "%d ", finalSample);
#endif
		}

#ifdef DEBUG_DUMP
		fprintf(f1, "\n");
		fprintf(f2, "\n");
		fprintf(f3, "\n");
#endif
	}

#ifdef DEBUG_DUMP
	fclose(f1);
	fclose(f2);
	fclose(f3);
#endif

	terrain.editChunk(samples, startRow, startCol, numRows, numCols);
}


void TerrainEditTool::editAt(float x, float y){
	if(mTerrain == NULL){
		TRACEE("Target not set");
		return;
	}

	wt::RaycastHitEvent res;
	
	bool paintMode = ui.tabWidget->currentIndex() == 1;

	if(WTE_CTX.getScene()->getPhysics()->pick(WTE_CTX.getScene()->getCamera(),
		glm::vec2(x, y), glm::vec2(mSceneView->width(), mSceneView->height()), res)){
			if(paintMode){
				glm::vec2 uv = glm::vec2(res.mImpact.x, res.mImpact.z)/glm::vec2(mTerrain->getHeightmap()->getNumRows()*mTerrain->getHeightmap()->getRowScale(), 
					mTerrain->getHeightmap()->getNumColumns()*mTerrain->getHeightmap()->getColumnScale());


				mFrameBuffer.bind(wt::gl::FrameBuffer::eMODE_DRAW);

				static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
				glDrawBuffers(1, bfrMap);

				wt::Texture2D* tex = mTerrain->getMapTexture();

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);

				float brushSize = 3 + (ui.brushSize->value()/100.0f)*50.f;

				float width = tex->getWidth();

				float height = tex->getHeigth();

				wt::Color clr(
					ui.materialIndex->currentIndex()==0, 
					ui.materialIndex->currentIndex()==1,
					ui.materialIndex->currentIndex()==2,
					ui.pressure->value()/100.0f
				);

				mSceneView->getRenderer()->render(
					mBrushTexture,
					glm::vec2(width, height),
					// X
					(width * uv.x) - brushSize/2,
					// Y
					(height*uv.y) - brushSize/2,
					brushSize,
					brushSize,
					clr);
		
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);

				mFrameBuffer.unbind(wt::gl::FrameBuffer::eMODE_DRAW);

				tex->generateMipmap();
		}
		else{
			// Index of the triangle the ray hit
			const uint32_t idx = mTerrain->getTriangleIndex(res.mTriangleIndex);

			// Total number of rows/columns
			const int32_t numRows = mTerrain->getHeightmap()->getNumRows();
			const int32_t numCols = mTerrain->getHeightmap()->getNumColumns();

			// The row/column the ray hit
			const int32_t row = idx / numRows;
			const int32_t col = idx % numCols;

			int32_t d = ui.brushSize->value(); // brush size

			// Start row/column of the chunk being edited
			int32_t startRow = glm::max(row - d/2, 0);
			int32_t startCol = glm::max(col - d/2, 0);

			// End row/column of the chunk being edited
			int32_t endRow = glm::min(row+d/2, numRows);
			int32_t endCol = glm::min(col+d/2, numCols);
			
			// Edit the chunk
			editTerrainChunk(
				*mTerrain,
				startRow, startCol,
				endRow-startRow, endCol-startCol,
				(ui.pressure->value()/100.0f),
				(BrushMode)ui.comboBox->currentIndex() // TODO fix this
			); 
		}
	}
}

void TerrainEditTool::onMouseDown(QMouseEvent* evt){
	if(isToolFocused()){
		editAt(evt->x(), evt->y());
	}
}


void TerrainEditTool::onCreateNewTerrain(){
	// TODO delete old terrain?

	wt::Terrain::Desc desc;
	
	if(!TerrainEditDialog::editTerrain(this, WTE_CTX.getAssets(), desc)){
		return;
	}

	wt::Terrain* terrain = WTE_CTX.getScene()->createTerrain();

	try{
		terrain->create(desc);

		wt::PhysicsActor::Desc desc;
		terrain->getPhysicsDesc(desc);

		WTE_CTX.getScene()->getPhysics()->createActor(terrain, desc);

		setTarget(terrain);
	}catch(wt::Exception& e){
		QMessageBox::critical(this, "Error", QString("Error creating terrain\n") + e.getDescription().c_str());
	}
}

void TerrainEditTool::onMouseUp(QMouseEvent*){
}