#include "stdafx.h"

#include "wte/MeshEditDialog.h"
#include "wte/MaterialEditDialog.h"

#include "wte/ResourcePickerDialog.h"

#define TD_TRACE_TAG "MeshEditDialog"

MeshEditDialog::MeshEditDialog(QWidget* parent, wt::AResourceSystem* assets, wt::ModelSkin::Mesh* mesh) : QDialog(parent), mAccepted(true), mMesh(mesh), mAssets(assets){
    ui.setupUi(this);

	if(mMesh->texture){
		ui.texture->setText(mMesh->texture->getPath().c_str());
	}

	if(mMesh->normalMap){
		ui.normalMap->setText(mMesh->normalMap->getPath().c_str());
	}
}

bool MeshEditDialog::edit(QWidget* widget, wt::AResourceSystem* assets, wt::ModelSkin::Mesh* mesh){
	MeshEditDialog* dlg = new MeshEditDialog(widget, assets, mesh);
	dlg->mAssets = assets;
	dlg->mMesh = mesh;

	dlg->exec();

	return dlg->mAccepted;
}

void MeshEditDialog::onEditTexture(){
	wt::Texture2D* texture = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager(), "Texture");

	if(texture){
		mMesh->texture = texture;
		ui.texture->setText(texture->getPath().c_str());
	}
}

void MeshEditDialog::accept(){
	mAccepted = true;

	mMesh->normalMap = mAssets->getTextureManager()->getFromPath(ui.normalMap->text().toStdString());
	mMesh->texture = mAssets->getTextureManager()->getFromPath(ui.texture->text().toStdString());

	QDialog::accept();
}

void MeshEditDialog::onEditMaterial(){
	MaterialEditDialog::edit(this, &mMesh->material);
}

void MeshEditDialog::onEditNormalMap(){
	wt::Texture2D* normalMap = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager(), "Bump map");

	if(normalMap){
		mMesh->normalMap = normalMap;
		ui.normalMap->setText(normalMap->getPath().c_str());
	}
}
