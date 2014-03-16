#include "stdafx.h"

#include <qerrormessage.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

#include <wt/AssimpModelLoader.h>
#include <Wt/ModelLoader.h>
#include <Wt/AnimationLoader.h>
#include <Wt/Texture.h>
#include <wt/Model.h>
#include <wt/Assets.h>
#include <wt/Utils.h>

#include "wte/ModelImporterTab.h"
#include "wte/ResourcePickerDialog.h"
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "ModelImporterTab"

#define WT_MODEL_EXTENSION "wtm"

#define IMPORTER_SETTINGS_TABLE_NAME "importer_settings"

ModelImporterTab::ModelImporterTab(QWidget* parent, wt::AResourceSystem* assets) : QWidget(parent), mAssets(assets){
	ui.setupUi(this);

	setAcceptDrops(true);

	connect(&WTE_CTX, SIGNAL(workspaceLoaded()),
		this, SLOT(onWorkspaceLoaded()));

	connect(&WTE_CTX, SIGNAL(saveRequest()),
		this, SLOT(onSaveRequest()));
}

QString generateFileName(const QString& directory, const QString& originalName, const QString& extension){
	int32_t counter = 0;
	QString newName = originalName;
	QString newPath;

	do{
		newName = wt::utils::print("%s_%d.%s",
			originalName.toStdString().c_str(),
			counter++,
			extension.toStdString().c_str()).c_str();

		newPath = directory + "/" + newName + extension;

	}while( QFile::exists(newPath) );

	return newName;
}

void ModelImporterTab::onWorkspaceLoaded(){
	ui.rootDir->setText( WTE_CTX.getAssets()->getFileSystem()->getRoot().c_str() );

	wt::LuaObject settings = WTE_CTX.getSettingsTable();

	WT_ASSERT(settings.IsTable(), "Invalid settings table");

	wt::LuaObject importerSettings = settings.Get(IMPORTER_SETTINGS_TABLE_NAME);

	wt::String imageDir;
	wt::String modelDir;

	if(!importerSettings.IsTable() || !wt::lua::luaConv(importerSettings.Get("image_dir"), imageDir)){
		// No value set, set the default one
		imageDir = "image";
	}

	if(!importerSettings.IsTable() || !wt::lua::luaConv(importerSettings.Get("model_dir"), modelDir)){
		// No value set, set the default one
		modelDir = "model";
	}

	ui.modelDir->setText(modelDir.c_str());
	ui.imageDir->setText(imageDir.c_str());

	// Save new values to settings table
	onSaveRequest();
}

void ModelImporterTab::onSaveRequest(){
	wt::LuaObject settings = WTE_CTX.getSettingsTable();

	WT_ASSERT(settings.IsTable(), "Invalid settings table");

	wt::LuaObject importerSettings = settings.Get(IMPORTER_SETTINGS_TABLE_NAME);

	if(!importerSettings.IsTable()){
		// Importer settings table doesn't exist, create it
		importerSettings = WTE_CTX.getLuaState()->newTable();
		settings.Set(IMPORTER_SETTINGS_TABLE_NAME, importerSettings);
	}

	importerSettings.Set("image_dir", ui.imageDir->text().toStdString().c_str());

	importerSettings.Set("model_dir", ui.modelDir->text().toStdString().c_str());
}

void ModelImporterTab::importTexture(ImportData& data, const QString& meshName, const QString& textureName){
	// Full source texture file name
	const QString sourceTexturePath = data.modelSourceDirectory + "/" + textureName;

	// Source texture name (file name without extension)
	QString sourceTextureName = QFileInfo(sourceTexturePath).completeBaseName();

	// Source texture extension
	const QString sourceTextureExtension = QFileInfo(sourceTexturePath).suffix();

	// Full path to where the new texture is going to be imported
	QString destinationTexturePath;

	// Resource name of the texture we're importing
	QString destinationTextureName = textureName;

	wt::Texture2D* resultTexture = NULL;

	wt::Texture2D* duplicateTexture = data.textureGroup->find(destinationTextureName.toStdString(), false);

	bool textureExists = false;

	if(duplicateTexture){
		const QString duplicateTexturePath = mAssets->getFileSystem()->getRoot().c_str() + QString("/") + QString(duplicateTexture->getUri().c_str());
		
		// Compare duplicate texture with the one we're importing
		if(wt::utils::compareFiles(sourceTexturePath.toStdString(), duplicateTexturePath.toStdString())){
			// Same files, skip copying the new one
			resultTexture = duplicateTexture;
			textureExists = true;
		}
		else{
			// Not the same file so we need to generate a new name
			int32_t counter = 0;
			const QString originalName = sourceTextureName;

			while(true){
				destinationTextureName = wt::utils::print("%s_%d",
					originalName.toStdString().c_str(), counter++).c_str();

				wt::Texture2D* texture = data.textureGroup->find(destinationTextureName.toStdString(), false);

				if(texture){
					// Found a texture with this generated name, check if it's the same file
					if(wt::utils::compareFiles(sourceTexturePath.toStdString(),
						(mAssets->getFileSystem()->getRoot().c_str() + QString("/") + texture->getUri().c_str()).toStdString())){
							// Same files we can use this texture instead
							resultTexture = texture;
							textureExists = true;
							break;
					}
				}
				else{
					// Found a free name
					break;
				}
			}
		}
	}

	// Texture with that name does not exist, we need to import it
	if(!textureExists){
		resultTexture = data.textureGroup->create(destinationTextureName.toStdString());

		// Generate texture path
		destinationTexturePath = data.imageDir + "/" + destinationTextureName + "." + sourceTextureExtension;

		bool fileExists = false;

		// Path already taken?
		if(QFile::exists(destinationTexturePath)){
			// Check if it's the same file we're trying to import
			if(wt::utils::compareFiles(destinationTexturePath.toStdString(), sourceTexturePath.toStdString())){
				// Same files, just set the uri to this
				resultTexture->setUri( wt::utils::toRelative(data.rootDir.toStdString(), destinationTexturePath.toStdString()) );
				fileExists = true;
			}
			// It's not the same file so we need to generate a new name for it
			else{
				QString newFileName = generateFileName(data.imageDir, sourceTextureName, sourceTextureExtension);

				destinationTexturePath = data.imageDir + "/" + newFileName + "." + sourceTextureExtension;
			}
		}

		if(!fileExists){
			// Import the texture to the workspace
			if(!QFile::copy(sourceTexturePath, destinationTexturePath)){
				WT_THROW("Copy failed; \"%s\" -> \"%s\"",
					sourceTexturePath.toStdString().c_str(),
					destinationTexturePath.toStdString().c_str()
				);
			}

			resultTexture->setUri( wt::utils::toRelative(data.rootDir.toStdString(), destinationTexturePath.toStdString()) );

			LOGI("Texture imported \"%s\"", sourceTexturePath.toStdString().c_str());
		}
	}

	wt::ModelSkin::Mesh* mesh = data.skin->findMeshByName(meshName.toStdString());
    mesh->texture = resultTexture;
}

void ModelImporterTab::importModel(ImportData& data){
	// Check if model with this name already exists in workspace
	QString destinationModelPath = data.modelDir + "/" + data.modelName + "." WT_MODEL_EXTENSION;

	if(QFile::exists(destinationModelPath)){
		destinationModelPath = data.modelDir + "/" + generateFileName(data.modelDir, data.modelName, WT_MODEL_EXTENSION);
		LOGW("Model file already exists, new name generated");
	}

	// Save the converted model to workspace
	mAssets->getModelManager()->save(destinationModelPath.toStdString(), data.model);
	data.model->setUri(wt::utils::toRelative(mAssets->getFileSystem()->getRoot(), destinationModelPath.toStdString()));

	for(wt::AssimpModelLoader::TextureMap::iterator iter=data.textureMap.begin(); iter!=data.textureMap.end(); iter++){
		importTexture(data, iter->first.c_str(), iter->second.c_str());
	}

	QMessageBox::information(this, "Successs", "Model successfuly imported");
}

void ModelImporterTab::import(const QString& srcModel){
	ImportData data;

	// Get model name
	bool ok;
	data.modelName = QInputDialog::getText(this, "Input",
		"Model name: ", QLineEdit::Normal, "", &ok);
	if(!ok){
		return;
	}

	// Get model group
	data.modelGroup = ResourcePickerDialog::pickGroup<wt::Model>(this, mAssets->getModelManager(), "Pick model group");
	if(data.modelGroup == NULL){
		TRACEW("Group not picked, aborting");
		return;
	}

	// Acquire import directories (e.g. model, texture etc..)
	data.rootDir = ui.rootDir->text();

	data.modelDir = data.rootDir + "/" + ui.modelDir->text();

	data.imageDir = data.rootDir + "/" + ui.imageDir->text();

	// Directory of the model being imported
	data.modelSourceDirectory = QFileInfo(srcModel).dir().absolutePath();

	if(data.modelGroup->find(data.modelName.toStdString(), false)){
		WT_THROW("Model with name \"%s\" already exists in group \"%s\"",
			data.modelName.toStdString().c_str(),
			data.modelGroup->getAbsPath().c_str()
		);
	}
	
	// Create new animation
	wt::Animation animation;

	// Create new model
	data.model = data.modelGroup->create(data.modelName.toStdString());

	// Load the animation and the model along with the texture map (used for skinning)
	wt::AssimpModelLoader::getSingleton().load(srcModel.toStdString(), *data.model, &animation, &data.textureMap);

	// Create a skin
	data.skin = data.model->createSkin("default");

	// Texture group where the new skin textures are going to be created in
	// TODO don't hard core this?
	data.textureGroup = mAssets->getTextureManager()->getGroupFromPath("$ROOT/model");
	if(!data.textureGroup){
		// Create the group if it doesn't exist
		data.textureGroup = mAssets->getTextureManager()->createGroup("model");
	}

	importModel(data);
}

void ModelImporterTab::onBatchConvert(){
	QStringList ls = QFileDialog::getOpenFileNames();
	int cnt=0;
	for(QStringList::Iterator i=ls.begin(); i!=ls.end(); i++){
		bool err=false;
		QString file = *i;
		 //d model and model skin
		wt::Model model;
		wt::Animation animation;

		// load it using Assimp model loader
		/*try{*/
			wt::AssimpModelLoader::getSingleton().load(file.toStdString(), model, &animation);
		/*}catch(wt::Exception& e){
			QMessageBox::critical(this,
				QString("Error converting model \"" + file + "\""),
				e.getDescription().c_str()
				);
			continue;
		}*/

		Uint32 e = file.toStdString().rfind(".");

		std::string baseName = file.toStdString().substr(0, e);

		std::string convModelName = baseName+".wtm";
		
		/*try{*/
			mAssets->getModelManager()->save(convModelName, &model);
			LOGI("Converted model saved to \"%s\"", convModelName.c_str());
		/*}catch(wt::Exception& e){
			QMessageBox::critical(this,
				QString("Error saving converted model to \"" + QString(convModelName.c_str()) + "\""),
				e.getDescription().c_str()
				);
			err=true;
		}*/

		std::string convAniName = baseName+".wta";
		/*try{*/
			mAssets->getAnimationManager()->save(convAniName, &animation);
			LOGI("Converted animation saved to \"%s\"", convAniName.c_str());
	/*	}catch(wt::Exception& e){
			QMessageBox::critical(this,
				QString("Error saving converted animation to \"" + QString(convAniName.c_str()) + "\""),
				e.getDescription().c_str()
				);
			err=true;
		}*/
		cnt += err ? 0 : 1;
	}

	char msg[512];
	sprintf(msg, "%d / %d files converted successfuly", cnt, ls.size());
	QMessageBox::information(this, "Successs", msg);
}

void ModelImporterTab::dropEvent(QDropEvent* evt){
    const QMimeData* mimeData = evt->mimeData();
 
    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls()){
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
 
        // extract the local paths of the files
        for (int i = 0; i < urlList.size() && i < 32; ++i){
			import(urlList.at(i).toLocalFile().toStdString().c_str());
			break;
        }
 
        // call a function to open the files
        //openFiles(pathList);
    }
}


void ModelImporterTab::dragEnterEvent(QDragEnterEvent* evt){
	evt->acceptProposedAction();
}

void ModelImporterTab::dragMoveEvent(QDragMoveEvent* evt){
	evt->acceptProposedAction();
}

void ModelImporterTab::onPickImageDir(){
	QString dir = QFileDialog::getExistingDirectory(this, "Browse image directory", ui.rootDir->text());

	ui.imageDir->setText( WTE_CTX.getAssets()->getRelativeURI(dir.toStdString()).c_str() );
}

void ModelImporterTab::onPickModelDir(){
	QString dir = QFileDialog::getExistingDirectory(this, "Browse model directory", ui.rootDir->text());

	ui.modelDir->setText( WTE_CTX.getAssets()->getRelativeURI(dir.toStdString()).c_str() );
}