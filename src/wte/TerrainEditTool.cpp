#include "stdafx.h"

#include "wte/TerrainEditTool.h"
#include "wte/HeightmapCreateDialog.h"

const char* TerrainEditTool::TAG = "TerrainEditTool";

TerrainEditTool::TerrainEditTool(SceneView* sceneView, QWidget* parent, AToolManager* toolManager, wt::Scene* scene, wt::Assets* assets) 
	: QDialog(parent), mSceneView(sceneView), ATool(toolManager), mScene(scene), mAssets(assets){
	ui.setupUi(this);
	setWindowFlags(Qt::Tool);

	connect(mSceneView, SIGNAL(onMouseDown(QMouseEvent*)),
		 this, SLOT(onMouseDown(QMouseEvent*)));

	connect(parent, SIGNAL(onTerrainCreated()),
		this, SLOT(onSceneInitialized()));

	connect(mSceneView, SIGNAL(onMouseDrag(float,float,Qt::MouseButton)),
		this, SLOT(onMouseDrag(float,float,Qt::MouseButton)));

	mPhysics = mScene->getPhysics();
}

void TerrainEditTool::onSaveTexture(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+QString("/assets/images/terrain/terrain.tga"));
	if(!path.isEmpty()){
		mScene->getTerrain()->getMapTexture()->dump(path.toStdString());
		LOGI(TAG, "Terrain texture saved to \"%s\"", path.toStdString().c_str());
	}
}

void TerrainEditTool::onSaveHeightmap(){
	QString path = QFileDialog::getSaveFileName(this,
		"Save", QDir::current().path()+QString("/assets/terrain/terrain"));
	if(!path.isEmpty()){
		std::ofstream out(path.toStdString().c_str(), std::ios::binary);

		out.write((const char*)mScene->getTerrain()->getHeightmap().getData(),
			mScene->getTerrain()->getHeightmap().getSize());

		out.close();

		LOGI(TAG, "Terrain heightmap saved to \"%s\"", path.toStdString().c_str());
	}
}

void TerrainEditTool::onResetTexture(){
	LOGI(TAG, "Resetting texture...");
	mFrameBuffer.bind(wt::Gl::FrameBuffer::DRAW);

	static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, bfrMap);

	wt::Texture2D* tex = mScene->getTerrain()->getMapTexture();

	glClearColor(ui.materialIndex->currentIndex()==0, 
		ui.materialIndex->currentIndex()==1, 
		ui.materialIndex->currentIndex()==2, 
		ui.pressure->value()/100.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	tex->bind();

	glGenerateMipmap(GL_TEXTURE_2D);

}

void TerrainEditTool::onResetHeightmap(){
	wt::Buffer<wt::Int16> bfr;

	
	wt::Uint32 nc = mScene->getTerrain()->getNumCols();
	wt::Uint32 nr = mScene->getTerrain()->getNumRows();

	bfr.create(nc*nr);

	bfr.clearMem();

	mScene->getTerrain()->editChunk(
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
	if(mScene->getTerrain()){

		// TODO creating it every time is a bad idea

		mFrameBuffer.create();

		mFrameBuffer.addAttachment(GL_COLOR_ATTACHMENT0, mScene->getTerrain()->getMapTexture());

		WT_ASSERT(mFrameBuffer.isComplete(), "Incomplete framebuffer");
	}
}

void TerrainEditTool::editTerrainChunk(wt::Terrain& terrain, wt::Uint32 startRow, wt::Uint32 startCol,
	wt::Uint32 numRows, wt::Uint32 numCols, float pressure, BrushMode mode){

	wt::Buffer<wt::Int16> samples;
	samples.create(numRows*numCols);

	wt::TerrainChunk::HeightMap& heightmap = terrain.getHeightmap();

	wt::Uint32 totalRows=terrain.getNumRows();
	wt::Uint32 totalCols=terrain.getNumCols();

	// max distance from center
	float maxDistance = glm::length( glm::vec2(numRows/2.0, numCols/2.0) );
	glm::vec2 center(startRow + numRows/2.0f, startCol + numCols/2.0f);

	for(wt::Uint32 row=startRow; row<startRow+numRows; row++){
		for(wt::Uint32 col=startCol; col<startCol+numCols; col++){
			wt::Int16 finalSample=0;

			switch(mode){
			case eELEVATE:
			case eLOWER:{
					// normalized distance from center (0 being the closest, and 1 the farthest)
					float factor = glm::length(center - glm::vec2(row, col)) / maxDistance;

					float quantFactor = terrain.getHeightScale(); // minimum delta
		
					wt::Int16 currentHeight = heightmap[row*totalCols + col];

					float maxHeightDelta = 1.0;
					wt::Int16 maxQuantDelta = maxHeightDelta/quantFactor;
					float delta = glm::floor( (1.0f - factor) * ( pressure * (maxQuantDelta-1.0f) + 1.0f ) ) * (mode == eELEVATE ? 1.0 : -1.0 );

					finalSample = currentHeight + delta;
				}
				break;

			case eSMOOTHEN:{
				int kernelSize= 2;
				int n=0;
				float sum = 0.0f;

				for(wt::Uint32 i=row-kernelSize/2; i<row+kernelSize/2; i++){
					for(wt::Uint32 j=col-kernelSize/2; j<col+kernelSize/2; j++){
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
	wt::RaycastHitEvent res;

	
	bool paintMode = ui.tabWidget->currentIndex() == 1;

	
	if(mPhysics->pick(mScene->getCamera(), mSceneView->getRenderer().getFrustum(),
		glm::vec2(x, y), glm::vec2(mSceneView->width(), mSceneView->height()), res)){

			if(paintMode){
			glm::vec2 uv = glm::vec2(res.mImpact.x, res.mImpact.z)/glm::vec2(mScene->getTerrain()->getNumRows()*mScene->getTerrain()->getRowScale(), 
					mScene->getTerrain()->getNumCols()*mScene->getTerrain()->getColScale());


			mFrameBuffer.bind(wt::Gl::FrameBuffer::DRAW);

			static GLenum bfrMap[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, bfrMap);

			wt::Texture2D* tex = mScene->getTerrain()->getMapTexture();

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
			mSceneView->getRenderer().render(
				mAssets->getTextureManager()->getFromPath("$ROOT/brushes/circle_hard"),
				glm::vec2(w, h),
				(w*uv.x)-brushSize/2, (h*uv.y)-brushSize/2, brushSize, brushSize, clr);
		
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			//mScene->getTerrain().getMapTexture()->dump("test.jpg");
			glGenerateMipmap(GL_TEXTURE_2D);

			mFrameBuffer.unbind(wt::Gl::FrameBuffer::DRAW);
		}
		else{

			PxShape* shapes[1];
			((PxRigidStatic*)mScene->getTerrain()->getPhysicsActor()->getPxActor())->getShapes(shapes, 1);

			PxHeightField* pxHeightField = shapes[0]->getGeometry().heightField().heightField;


			
			wt::Gl::Batch& batch = mScene->getTerrain()->getBatch();
			
			//wt::TerrainChunk::Vertex* vertices = (wt::TerrainChunk::Vertex*)batch.getVertexBuffer().map(wt::Gl::Buffer::eREAD_WRITE);

			wt::Uint32 idx = mScene->getTerrain()->getTriangleIndex(res.mTriangleIndex);

			wt::Uint32 numRows = mScene->getTerrain()->getNumRows();
			wt::Uint32 numCols = mScene->getTerrain()->getNumCols();

			wt::Int32 row = idx/numRows;
			wt::Int32 col = idx%numCols;


			wt::Int32 d = ui.brushSize->value(); // brush size

			wt::Int32 startRow = (row-d/2)<0?0:row-d/2;
			wt::Int32 startCol = (col-d/2)<0?0:col-d/2;

			wt::Int32 endRow = (row+d/2)>=numRows?numRows:row+d/2;
			wt::Int32 endCol = (col+d/2)>=numCols?numCols:col+d/2;

			
			editTerrainChunk(*mScene->getTerrain(),
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