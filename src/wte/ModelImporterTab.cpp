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

#include "wte/ModelImporterTab.h"
#include "wte/ResourcePickerDialog.h"

#define TD_TRACE_TAG "ModelImporterTab"

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
	// TODO get root dir from Assets
	QString dstDir = "d:/Documents/prog/c++/workspace/Wt/workspace/assets";

	// Directory of the model being imported
	QString srcDir =  QFileInfo(srcModel).dir().absolutePath();

	// Create new geo model / skinned model and model skin
	wt::Model* model = mAssets->getModelManager()->create(name);
	
	wt::Animation animation;
	wt::AssimpModelLoader::TextureMap texMap;

	// Load it using Assimp model loader
	/*try{*/
		wt::AssimpModelLoader::getSingleton().load(srcModel.toStdString(), *model, &animation, &texMap);
	/*}catch(wt::Exception& e){
		QMessageBox::critical(this,
			QString("Error converting model"),
			e.getDescription().c_str()
			);
		return;
	}*/

	// Check if model with this name already exists in workspace
	QString dstModel = dstDir + "/model/" + model->getName().c_str() + ".wtm";
	if(QFile::exists(dstModel)){
		QMessageBox::critical(this,
			QString("Error importing model"), QString("File \"") + dstModel + "\" already exists");
		return;
	}

	// Save the converted model to workspace
	mAssets->getModelManager()->save(dstModel.toStdString(), model);

	
	model->setUri(wt::utils::toRelative(mAssets->getFileSystemRoot(), dstModel.toStdString()));

	// Texture group where the new skin textures are going to be created in
	wt::TextureGroup* texGroup = mAssets->getTextureManager()->getGroupFromPath("$ROOT/model");
	if(!texGroup){
		// Create the group if it doesn't exist
		texGroup = mAssets->getTextureManager()->createGroup("model");
	}

	// Create a skin
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
		//mAssets->getRo
		LOG("1");
		if(texture && !sameFile(srcTexPath, QString(mAssets->getFileSystemRoot().c_str()) + "/" + texture->getUri().c_str())){
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
					LOG("2");
					if(sameFile(srcTexPath, QString(mAssets->getFileSystemRoot().c_str()) + "/" + newTexture->getUri().c_str())){
						// We can use this texture
						textureExists = true;
						texture = newTexture;
						break;
					}
				}
				else{
					// Texture does not exist, we have to import it

					dstTexName = newTextureName;
					dstTexPath = dstDir + "/image/" + newTextureName + "." + extension;
					if(QFile::exists(dstTexPath)){
						// Check if this is the same file as we're trying to import
						LOG("3");
						if(sameFile(dstTexPath, srcTexPath)){
							LOGI("File already exists, skipping copy");
						}
						else{
							// TODO handle this (just generate a new name)
							TRACEE("TODO handle this");
							return;
						}
					}
					break;
				}
			}
		}
		else if(texture == NULL){
			// Texture does not exist, we have to import it
			dstTexName = srcTexName;

			dstTexPath = dstDir + "/image/" + srcTexName + "." + extension;
			if(QFile::exists(dstTexPath)){
				// Check if this is the same file as we're trying to import
				LOG("4");
				if(sameFile(dstTexPath, srcTexPath)){
					LOGI("File already exists, skipping copy");
				}
				else{
					// TODO handle this (just generate a new name)
					TRACEE("TODO handle this");
					return;
				}
			}
		}
		else{
			// Texture exists we can use thsi one
			textureExists = true;
		}

		if(!textureExists){
			// import the texture fil	e to workspace
			QFile::copy(srcTexPath, dstTexPath);

			LOGI("Importing \"%s\"", srcTexPath.toStdString().c_str());

			// create texture
			texture = texGroup->create(dstTexName.toStdString());
			texture->setUri(wt::utils::toRelative(mAssets->getFileSystemRoot(), dstTexPath.toStdString()));
		}
		else{
			LOGI("Texture already exists");
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

	WT_ASSERT(file1.open(QIODevice::ReadOnly), "Error comparing files (open file failed \"%s\")", path1.toStdString().c_str());
		
	WT_ASSERT(file2.open(QIODevice::ReadOnly), "Error comparing files (open file failed \"%s\")", path2.toStdString().c_str());
	
	if(file1.size() != file2.size()){
		LOGW("Same file name, different sizes");
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
				LOGE("Error comparing files (read error)");
				return false;
			}

			if(memcmp(bfr1, bfr2, read1) != 0){
				LOGW("Same file name, same sizes, different content");
				return false;
			}

			if(read1 != 1024){
				break;
			}
		}
	}

	return true;
}

ModelImporterTab::ModelImporterTab(QWidget* parent, wt::AResourceSystem* assets) : QWidget(parent), mAssets(assets){
	ui.setupUi(this);

	setAcceptDrops(true);
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