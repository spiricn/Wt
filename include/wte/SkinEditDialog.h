#ifndef WTE_DLGSKINEDIT_H
#define WTE_DLGSKINEDIT_H

#include <qdialog.h>

#include "ui_SkinEditDialog.h"

#include <wt/Model.h>
#include <wt/Assets.h>

#include "wte/ResourcePickerDialog.h"

class SkinEditDialog : public QDialog{
Q_OBJECT

public:
	struct EditResult{
		bool ok;

		EditResult() : ok(false){
		}
	};

private:
	Ui::SkinEditDialog ui;
	EditResult mResult;
	wt::ModelSkin* mSkin;
	wt::Assets* mAssets;

public:
	SkinEditDialog(QWidget* parent, wt::Assets* assets, wt::ModelSkin* skin) : QDialog(parent), mSkin(skin), mAssets(assets){
		ui.setupUi(this);

		int id=0;
		for(wt::ModelSkin::MeshList::iterator i=skin->getMeshListBeg(); i!=skin->getMeshListEnd(); i++){
			wt::ModelSkin::Mesh* mesh = *i;

			QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);

			item->setText(0, mesh->geometry->getName().c_str());

			if(mesh->texture != NULL){
				item->setText(1, mesh->texture->getPath().c_str());
			}

			if(mesh->normalMap != NULL){
				item->setText(2, mesh->normalMap->getPath().c_str());
			}

			ui.treeWidget->addTopLevelItem(item);

			item->setData(0, Qt::UserRole, QVariant(id++));
		}
	}

	static EditResult edit(QWidget* parent, wt::Assets* assets, wt::ModelSkin* skin){
		SkinEditDialog dlg(parent, assets, skin);

		dlg.exec();

		return dlg.mResult;
	}

protected slots:
	void onItemActivated(QTreeWidgetItem* item, int){
		
		wt::ModelSkin::Mesh* mesh = *(mSkin->getMeshListBeg() + item->data(0, Qt::UserRole).toInt() );


		wt::Texture2D* texture = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager(), "Texture");

		if(texture){
			mesh->texture = texture;
			item->setText(1, texture->getPath().c_str());
		}

		wt::Texture2D* normalMap = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager(), "Bump map");

		if(normalMap){
			mesh->normalMap = normalMap;
			item->setText(2, normalMap->getPath().c_str());
		}

	}

}; // </SkinEditDialog>

#endif // </WTE_DLGSKINEDIT_H>