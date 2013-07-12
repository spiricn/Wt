#include "stdafx.h"

#include <qerrormessage.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

#include <wt/Log.h>
#include <wt/AssimpModelLoader.h>
#include <Wt/ModelLoader.h>
#include <Wt/AnimationLoader.h>
#include <Wt/Texture.h>
#include <wt/Model.h>
#include <wt/Assets.h>

#include "wte/ModelImporterTab.h"
#include "wte/ResourcePickerDialog.h"

const char* ModelImporterTab::TAG = "ModelImporterTab";

void ModelImporterTab::import(const QString& srcModel){
// TODO rethink this whole concept, this isn't going to work ...
#if 1
	// base name of the newly created assets
	bool ok;
	wt::String name = QInputDialog::getText(this, "Input",
		"Model name: ", QLineEdit::Normal, "", &ok).toStdString();

	if(!ok){
		return;
	}

	// location where the assets are going to be copied
	QString dstDir = "assets/imported";

	// Directory of the model being imported
	QString srcDir =  QFileInfo(srcModel).dir().absolutePath();

	// Create new geo model / skinned model and model skin
	wt::Model* model = mAssets->getModelManager()->create(name);
	
	wt::Animation animation;
	wt::AssimpModelLoader::TextureMap texMap;

	// Load it using Assimp model loader
	try{
		wt::AssimpModelLoader::getSingleton().load(srcModel.toStdString(), *model, &animation, &texMap);
	}catch(wt::Exception& e){
		QMessageBox::critical(this,
			QString("Error converting model"),
			e.getDescription().c_str()
			);
		return;
	}

	// Check if model with this name already exists in workspace
	QString dstModel = dstDir + "/" + model->getName().c_str() + ".wtm";
	if(QFile::exists(dstModel)){
		QMessageBox::critical(this,
			QString("Error importing model"), QString("File \"") + dstModel + "\" already exists");
		return;
	}

	// Save the converted model to workspace
	wt::ModelLoader::getSingleton().save(dstModel.toStdString(), model);
	model->setUri(dstModel.toStdString());

	// Texture group where the new skin textures are going to be created in
	wt::TextureGroup* texGroup = mAssets->getTextureManager()->getGroupFromPath("$ROOT/models");
	wt::Model::GeometrySkin* skin = model->createSkin("default");

	for(wt::AssimpModelLoader::TextureMap::iterator i=texMap.begin(); i!=texMap.end(); i++){

		// Full source texture file name
		QString srcTexPath = srcDir + "/" + i->second.c_str();

		// Source texture name (file name without extension)
		QString srcTexName = QFileInfo(srcTexPath).completeBaseName();

		// Source texture extension
		QString extension = QFileInfo(srcTexPath).suffix();

		// Full path to where the new texture is going to be imported
		QString dstTexturePath;

		// Resource name of the texture we're importing
		QString dstTexName;

		// Path where the new texture is going to be imported ot
		QString dstTexPath;

		wt::Texture2D* texture = texGroup->find(srcTexName.toStdString(), false);

		bool textureExists = false;
		
		// check to see if this texture has already been created
		if(texture && !sameFile(srcTexPath, texture->getUri().c_str())){
			// File with this name already exists but its different from
			// the one we're importing, so we generate a new file name for it
			QString newTextureName;

			int cnt = 0;
			while(true){
				char tmp[128];
				++cnt;
				sprintf(tmp, "%s_%d", srcTexName.toStdString().c_str(), cnt);
				newTextureName = tmp;

				wt::Texture2D* newTexture = texGroup->find(newTextureName.toStdString(), false);

				if(newTexture){
					if(sameFile(srcTexPath, newTexture->getUri().c_str())){
						// We can use this texture
						textureExists = true;
						texture = newTexture;
						break;
					}
				}
				else{
					// Texture does not exist, we have to import it

					dstTexName = newTextureName;
					dstTexPath = dstDir + "/" + newTextureName + "." + extension;
					if(QFile::exists(dstTexPath)){
						// TODO handle this (just generate a new name)
						LOGE(TAG, "TODO handle this");
						return;
					}
					break;
				}
			}
		}
		else if(texture == NULL){
			// Texture does not exist, we have to import it
			dstTexName = srcTexName;

			dstTexPath = dstDir + "/" + srcTexName + "." + extension;
			if(QFile::exists(dstTexPath)){
				// TODO handle this (just generate a new name)
				LOGE(TAG, "TODO handle this");
				return;
			}
		}
		else{
			// Texture exists we can use thsi one
			textureExists = true;
		}

		if(!textureExists){
			// import the texture fil	e to workspace
			QFile::copy(srcTexPath, dstTexPath);

			LOGI(TAG, "Importing \"%s\"", srcTexPath.toStdString().c_str());

			// create texture
			texture = texGroup->create(dstTexName.toStdString());
			texture->setUri(dstTexPath.toStdString());
		}
		else{
			LOGI(TAG, "Texture already exists");
		}

		wt::Model::GeometrySkin::Mesh* mesh = skin->findMeshByName(i->first);
		mesh->texture = texture;
	}

	QMessageBox::information(this, "Successs", "Model successfuly imported");
#else
		QMessageBox::critical(this, "Fail", "TODO: implement this");
#endif
}

bool ModelImporterTab::sameFile(const QString& path1, const QString& path2){
	QFile file1(path1), file2(path2);

	if(!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly)){
		LOGE(TAG, "Error comparing files (open file failed)");
	}
	
	if(file1.size() != file2.size()){
		LOGW(TAG, "Same file name, different sizes");
		return false;
	}
	else{
		char bfr1[1024];
		char bfr2[1024];

		while(true){
			qint64 read1, read2;
			read1 = file1.read(bfr1, 1024);
			read2 = file2.read(bfr2, 1024);
		
			if(read1 == -1 || read2 == -1 || read1 != read2){
				LOGE(TAG, "Error comparing files (read error)");
				return false;
			}

			if(memcmp(bfr1, bfr2, read1) != 0){
				LOGW(TAG, "Same file name, same sizes, different content");
				return false;
			}

			if(read1 != 1024){
				break;
			}
		}
	}

	return true;
}

ModelImporterTab::ModelImporterTab(QWidget* parent, wt::Assets* assets) : QWidget(parent), mAssets(assets){
	ui.setupUi(this);

	setAcceptDrops(true);
}

void ModelImporterTab::onBatchConvert(){
	QStringList ls = QFileDialog::getOpenFileNames();
	int cnt=0;
	for(QStringList::Iterator i=ls.begin(); i!=ls.end(); i++){
		bool err=false;
		QString file = *i;

		// create new geo model / skinned model and model skin
		wt::Model model;
		wt::Animation animation;

		// load it using Assimp model loader
		try{
			wt::AssimpModelLoader::getSingleton().load(file.toStdString(), model, &animation);
		}catch(wt::Exception& e){
			QMessageBox::critical(this,
				QString("Error converting model \"" + file + "\""),
				e.getDescription().c_str()
				);
			continue;
		}

		Uint32 e = file.toStdString().rfind(".");

		std::string baseName = file.toStdString().substr(0, e);

		std::string convModelName = baseName+".wtm";
		
		try{
			wt::ModelLoader::getSingleton().save(convModelName, &model);
			LOGI(TAG, "Converted model saved to \"%s\"", convModelName.c_str());
		}catch(wt::Exception& e){
			QMessageBox::critical(this,
				QString("Error saving converted model to \"" + QString(convModelName.c_str()) + "\""),
				e.getDescription().c_str()
				);
			err=true;
		}

		std::string convAniName = baseName+".wta";
		try{
			wt::AnimationLoader::getSingleton().save(convAniName, &animation);
			LOGI(TAG, "Converted animation saved to \"%s\"", convAniName.c_str());
		}catch(wt::Exception& e){
			QMessageBox::critical(this,
				QString("Error saving converted animation to \"" + QString(convAniName.c_str()) + "\""),
				e.getDescription().c_str()
				);
			err=true;
		}
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