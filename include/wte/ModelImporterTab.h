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
#include "wte/ATool.h"

class ModelImporterTab : public QWidget, public ATool{
public:
	Q_OBJECT

public:
	ModelImporterTab(QWidget* parent, AToolManager* manager);

private slots:
	void onBatchConvert();

	void onWorkspaceLoaded();

	void onSaveRequest();

	void onPickImageDir();

	void onPickModelDir();

private:
	struct ImportData{
		wt::ModelGroup* modelGroup;
		QString sourcePath;
		QString rootDir;
		QString modelDir;
		QString imageDir;
		QString modelSourceDirectory;
		wt::Model* model;
		wt::AssimpModelLoader::TextureMap textureMap;
		QString modelName;
		wt::ModelSkin* skin;
		wt::AResourceGroup<wt::Texture2D>* textureGroup;
	}; // </ImportData>

private:
	void dropEvent(QDropEvent* evt);

	void dragEnterEvent(QDragEnterEvent* evt);

	void dragMoveEvent(QDragMoveEvent* evt);

	void import(const QString& path);

	void importModel(ImportData& data);

	void importTexture(ImportData& data, const QString& meshName, const QString& textureName);

	bool importDuplicateTexture(const QString& sourceTexturePath, wt::Texture2D* duplicate);

private:
	Ui::ModelImporter ui;

}; // </ModelImporterTab>

#endif // </WTE_MODELIMPORTERTAB_H>
