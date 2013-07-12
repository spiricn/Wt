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
	wt::Model::GeometrySkin* mSkin;
	wt::Assets* mAssets;

public:
	SkinEditDialog(QWidget* parent, wt::Assets* assets, wt::Model::GeometrySkin* skin) : QDialog(parent), mSkin(skin), mAssets(assets){
		ui.setupUi(this);

		int id=0;
		for(wt::Model::GeometrySkin::MeshList::iterator i=skin->getMeshList().begin(); i!=skin->getMeshList().end(); i++){
			QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
			item->setText(0, i->geometry->getName().c_str());
			if(i->texture != NULL){
				item->setText(1, i->texture->getPath().c_str());
			}
			ui.treeWidget->addTopLevelItem(item);

			item->setData(0, Qt::UserRole, QVariant(id++));
		}
	}

	static EditResult edit(QWidget* parent, wt::Assets* assets, wt::Model::GeometrySkin* skin){
		SkinEditDialog dlg(parent, assets, skin);

		dlg.exec();

		return dlg.mResult;
	}

protected slots:
	void onItemActivated(QTreeWidgetItem* item, int){
		
		wt::Model::GeometrySkin::Mesh& mesh = mSkin->getMeshList()[ item->data(0, Qt::UserRole).toInt() ];

		wt::Texture2D* texture = ResourcePickerDialog::pickResource<wt::Texture2D>(this, mAssets->getTextureManager());

		if(texture){
			mesh.texture = texture;
			item->setText(1, texture->getPath().c_str());
		}
	}

}; // </SkinEditDialog>

#endif // </WTE_DLGSKINEDIT_H>