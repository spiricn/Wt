#include "stdafx.h"

#include "wte/MaterialEditDialog.h"

#define TD_TRACE_TAG "MaterialEditDialog"

MaterialEditDialog::MaterialEditDialog(QWidget* parent, wt::Material* material) : QDialog(parent), mMaterial(material){
    ui.setupUi(this);

	ui.alphaTested->setChecked( mMaterial->isAlphaTested() );
	ui.blended->setChecked( mMaterial->getBlend() );
	ui.ambientColor->setColor( mMaterial->getAmbient() );
	ui.diffuseColor->setColor( mMaterial->getDiffuse() );
	ui.specularColor->setColor( mMaterial->getSpecular() );
	ui.shininess->setValue( mMaterial->getShininess() );

	// TODO
	ui.cullMode->setCurrentIndex( static_cast<int>(mMaterial->getCullMode()) );
}

void MaterialEditDialog::reject(){
	QDialog::reject();
}

void MaterialEditDialog::accept(){
	mMaterial->setAlphaTest( ui.alphaTested->isChecked() );
	mMaterial->setBlend( ui.blended->isChecked() );
	mMaterial->setAmbient( ui.ambientColor->getColor() );
	mMaterial->setDiffuse( ui.diffuseColor->getColor() );
	mMaterial->setSpecular( ui.specularColor->getColor() );
	mMaterial->setShininess( ui.shininess->value() );

	// TODO
	mMaterial->setCullMode( static_cast<wt::CullMode>(ui.cullMode->currentIndex()) );

	QDialog::accept();
}

void MaterialEditDialog::edit(QWidget* parent, wt::Material* material){
	MaterialEditDialog* dlg = new MaterialEditDialog(parent, material);

	dlg->exec();
}