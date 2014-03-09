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
		WTE_CTX.getAssets()->getTextureManager()->getLoader()->load("assets/images/brushes/circle_hard.png", mBrushTexture);

		// TODO do this some other way
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

	QString path = mTerrain->getDesc().heightmapPath.c_str();

	if(path.isEmpty()){
		path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+QString("terrain.wtt"));
	}

	if(QMessageBox::question(this, "Confirmation", "Save terrain heightmap to this location ?\n\n" + path, QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes){
		return;
	}

	std::ofstream out(path.toStdString().c_str(), std::ios::binary);

	out.write((const char*)mTerrain->getHeightmap().getData(),
		mTerrain->getHeightmap().getSize());

	out.close();

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

	
	uint32_t nc = mTerrain->getNumCols();
	uint32_t nr = mTerrain->getNumRows();

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

	wt::Buffer<int16_t> samples;
	samples.create(numRows*numCols);

	wt::TerrainChunk::HeightMap& heightmap = terrain.getHeightmap();

	uint32_t totalRows=terrain.getNumRows();
	uint32_t totalCols=terrain.getNumCols();

	// max distance from center
	float maxDistance = glm::length( glm::vec2(numRows/2.0, numCols/2.0) );
	glm::vec2 center(startRow + numRows/2.0f, startCol + numCols/2.0f);

	enum BrushShape{
		eSHAPE_CIRCLE,
		eSHAPE_SQUARE
	};

	BrushShape shape = static_cast<BrushShape>(ui.brushShape->currentIndex());

	for(uint32_t row=startRow; row<startRow+numRows; row++){
		for(uint32_t col=startCol; col<startCol+numCols; col++){
			// Current height at the given point
			const int16_t currentHeight = heightmap[row*totalCols + col];

			// Resulting height
			int16_t finalSample=0;

			// Normalized distance from center (1 being the closest, and 0 the farthest)
			const float factor = 1.0f - ( glm::length(center - glm::vec2(row, col)) / maxDistance );

			// Minimum height delta
			const float quantFactor = terrain.getHeightScale();

			switch(mode){
			case eELEVATE:
			case eLOWER:{
					int16_t minDelta = 1;
					int16_t maxDelta = 20;

					int16_t delta = shape == eSHAPE_CIRCLE ? factor : 1.0f * ( minDelta + pressure*(maxDelta-minDelta) );

					delta *= mode == eELEVATE ? 1 : -1;

					finalSample = currentHeight + delta;
				}
				break;

			case eSMOOTHEN:{
				int kernelSize= 10;
				int n=0;
				float sum = 0.0f;

				for(uint32_t i=row-kernelSize/2; i<row+kernelSize/2; i++){
					for(uint32_t j=col-kernelSize/2; j<col+kernelSize/2; j++){
						sum += heightmap[i*totalCols + j] * terrain.getHeightScale();
						n++;
					}
				}

	
				finalSample = glm::floor( ((float)sum/n) / terrain.getHeightScale() );
	

				break;
				}
			}

			samples[(row-startRow)*numCols + (col-startCol)] = finalSample;
		}
	}

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
			glm::vec2 uv = glm::vec2(res.mImpact.x, res.mImpact.z)/glm::vec2(mTerrain->getNumRows()*mTerrain->getRowScale(), 
					mTerrain->getNumCols()*mTerrain->getColScale());


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

			float w=tex->getWidth();
			float h=tex->getHeigth();

			wt::Color clr(ui.materialIndex->currentIndex()==0, 
				ui.materialIndex->currentIndex()==1,
				ui.materialIndex->currentIndex()==2,
				ui.pressure->value()/100.0f);

			mSceneView->getRenderer()->render(
				/*mAssets->getTextureManager()->getFromPath("$ROOT/brushes/circle_hard"),*/
				mBrushTexture,
				glm::vec2(w, h),
				(w*uv.x)-brushSize/2, (h*uv.y)-brushSize/2, brushSize, brushSize, clr);
		
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			//mTerrain.getMapTexture()->dump("test.jpg");

			mFrameBuffer.unbind(wt::gl::FrameBuffer::eMODE_DRAW);

			tex->generateMipmap();
		}
		else{

			PxShape* shapes[1];
			((PxRigidStatic*)mTerrain->getPhysicsActor()->getPxActor())->getShapes(shapes, 1);

			PxHeightField* pxHeightField = shapes[0]->getGeometry().heightField().heightField;


			
			wt::gl::Batch& batch = mTerrain->getBatch();
			
			//wt::TerrainChunk::Vertex* vertices = (wt::TerrainChunk::Vertex*)batch.getVertexBuffer().map(wt::gl::Buffer::eREAD_WRITE);

			uint32_t idx = mTerrain->getTriangleIndex(res.mTriangleIndex);

			uint32_t numRows = mTerrain->getNumRows();
			uint32_t numCols = mTerrain->getNumCols();

			int32_t row = idx/numRows;
			int32_t col = idx%numCols;


			int32_t d = ui.brushSize->value(); // brush size

			int32_t startRow = (row-d/2)<0?0:row-d/2;
			int32_t startCol = (col-d/2)<0?0:col-d/2;

			int32_t endRow = (row+d/2)>=numRows?numRows:row+d/2;
			int32_t endCol = (col+d/2)>=numCols?numCols:col+d/2;

			
			editTerrainChunk(*mTerrain,
				startRow, startCol, endRow-startRow, endCol-startCol, (ui.pressure->value()/100.0f),
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
	// TODO move terrain creation to TerrainTool
	wt::TerrainDesc desc;
	
	if(!TerrainEditDialog::editTerrain(this, WTE_CTX.getAssets(), desc)){
		return;
	}

	wt::Terrain* terrain = WTE_CTX.getScene()->createTerrain();
	try{
		terrain->create(desc);

		setTarget(terrain);
	}catch(wt::Exception& e){
		QMessageBox::critical(this, "Error", QString("Error creating terrain\n") + e.getDescription().c_str());
	}
}