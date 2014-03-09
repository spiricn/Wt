#include "stdafx.h"

#include "wte/TerrainEditDialog.h"
#include "wte/ResourcePickerDialog.h"
#include "wte/HeightmapCreateDialog.h"

#define TD_TRACE_TAG "TerrainEditDialog"

TerrainEditDialog::TerrainEditDialog(QWidget* parent, wt::AResourceSystem* assets) : QDialog(parent), mAssets(assets), mAccepted(false){
    ui.setupUi(this);
}


void TerrainEditDialog::onNewHeightmap(){
	QString res = HeightmapCreateDialog::createHeightmap(this);
	if(res.size()){
		ui.heightmap->setText(res);
	}

}

void TerrainEditDialog::onTex1Pick(){
	wt::Image* res = ResourcePickerDialog::pickResource<wt::Image>(this, mAssets->getImageManager());
	if(res){
		ui.texture1->setText(res->getPath().c_str());
	}
}

void TerrainEditDialog::onTex2Pick(){
	wt::Image* res = ResourcePickerDialog::pickResource<wt::Image>(this, mAssets->getImageManager());
	if(res){
		ui.texture2->setText(res->getPath().c_str());
	}
}

void TerrainEditDialog::onTex3Pick(){
	wt::Image* res = ResourcePickerDialog::pickResource<wt::Image>(this, mAssets->getImageManager());
	if(res){
		ui.texture3->setText(res->getPath().c_str());
	}
}

void TerrainEditDialog::onTexMapPick(){
	wt::Texture2D* res = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager());
	if(res){
		ui.textureMap->setText(res->getPath().c_str());
	}
}

void TerrainEditDialog::onHeightmapPick(){
	wt::Heightmap* res = ResourcePickerDialog::pickResource<wt::Heightmap>(this, mAssets->getHeightmapManager());

	if(res){
		ui.heightmap->setText(res->getPath().c_str());
	}
}


void TerrainEditDialog::onSave(){
	mResult.heightmap = mAssets->getHeightmapManager()->getFromPath( ui.heightmap->text().toStdString() );

	mResult.texture1 =  mAssets->getImageManager()->getFromPath( ui.texture1->text().toStdString() );
	mResult.texture2 =  mAssets->getImageManager()->getFromPath( ui.texture2->text().toStdString() );
	mResult.texture3 =  mAssets->getImageManager()->getFromPath( ui.texture3->text().toStdString() );

	mResult.textureMap =  mAssets->getTextureManager()->getFromPath( ui.textureMap->text().toStdString() );

	mAccepted = true;

	close();
}