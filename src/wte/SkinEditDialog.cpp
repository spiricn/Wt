#include "stdafx.h"

#include <qmenu.h>

#include "wte/SkinEditDialog.h"

//
//void SkinEditDialog::addTexture(const QString& name, wt::Texture2D* tex){
//	QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
//
//	item->setText(0, name);
//	item->setText(1, tex->getPath().c_str());
//
//	mResult.map.push_back(
//		EditResult::Material(name, tex));
//
//	ui.treeWidget->addTopLevelItem(item);
//}
//
//SkinEditDialog::SkinEditDialog(QWidget* parent, wt::ModelSkin* skin, wt::TextureGroup* texGroup) : QDialog(parent){
//	ui.setupUi(this);
//
//	if(texGroup){
//		for(wt::TextureGroup::ResourceMap::iterator i=texGroup->getResources().begin();
//			i!=texGroup->getResources().end(); i++){
//				addTexture(
//					i->second->getName().c_str(), i->second);
//		}
//	}
//	else if(skin){
//		for(wt::ModelSkin::MaterialMap::iterator i=skin->getMaterialMap().begin();
//			i!=skin->getMaterialMap().end(); i++){
//				if(i->second.mTexture){
//					addTexture(
//						i->first.c_str(), i->second.mTexture);
//				}
//				else{
//					LOGW("Null pointer texture in skin %s", skin->getName().c_str());
//				}
//		}
//	}
//}
//
//SkinEditDialog::EditResult SkinEditDialog::editSkin(QWidget* parent, wt::ModelSkin* skin,
//	wt::TextureGroup* texGroup){
//
//	SkinEditDialog dlg(parent, skin, texGroup);
//
//	dlg.exec();
//
//	return dlg.mResult;
//}
//
//void SkinEditDialog::onSave(){
//	mResult.ok = true;
//	close();
//}
//
//void SkinEditDialog::onAdd(){
//	bool ok;
//	QString name = QInputDialog::getText(this, "Input",
//		"Geometry name: ", QLineEdit::Normal, "", &ok);
//	if(!ok){
//		return;
//	}
//
//	ResourcePickerDialog picker(this);
//	wt::Texture2D* texture = picker.pickResource<wt::Texture2D>(this, &wt::TextureManager::getSingleton());
//
//	if(texture){
//		addTexture(name, texture);
//	}
//}
//
//void SkinEditDialog::onContextMenuRequest(QPoint){
//	QMenu menu(this);
//	QAction* action;
//	
//	/*action = new QAction(QIcon(), "Set texture", this);
//	connect(action, SIGNAL(triggered()),
//		this, onSetTexture
//
//	menu.addAction();
//	menu.addAction("Set normal map");*/
//
//	menu.exec();
//}