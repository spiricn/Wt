#ifndef WTE_MODELIMPORTERTAB_H
#define WTE_MODELIMPORTERTAB_H

#include "stdafx.h"

namespace wt{

class Assets;

}; // </wt>


#include <wt/AResourceSystem.h>

#include "ui_ModelImporterTab.h"

#include "wte/utils.h"
#include <wt/Model.h>
#include <wt/AssimpModelLoader.h>


struct ImportData{
	wt::ModelGroup* modelGroup;
	QString rootDir;
	QString modelDir;
	QString imageDir;
	QString modelSourceDirectory;
	wt::Model* model;
	wt::AssimpModelLoader::TextureMap textureMap;
	QString modelName;
	wt::Model::GeometrySkin* skin;
	wt::TextureGroup* textureGroup;
};

class ModelImporterTab : public QWidget{
	Q_OBJECT;

private:
	wt::AResourceSystem* mAssets;
	Ui::ModelImporter ui;

	void importModel(ImportData& data);

	void importTexture(ImportData& data, const QString& meshName, const QString& textureName);

	bool importDuplicateTexture(const QString& sourceTexturePath, wt::Texture2D* duplicate);

protected:
	void dropEvent(QDropEvent* evt);

	void dragEnterEvent(QDragEnterEvent* evt);

	void dragMoveEvent(QDragMoveEvent* evt);

	void import(const QString& path);

public:
	ModelImporterTab(QWidget* parent, wt::AResourceSystem* assets);

protected slots:
	void onBatchConvert();

}; // </ModelImporterTab>

#endif // </WTE_MODELIMPORTERTAB_H>