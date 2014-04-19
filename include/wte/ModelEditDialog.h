#ifndef WTE_DLGSKINNEDMODELEDIT_H
#define WTE_DLGSKINNEDMODELEDIT_H

#include <qdialog.h>

#include <wt/SkyBox.h>
#include <Wt/Model.h>
#include <Wt/Animation.h>
#include <wt/Assets.h>

#include "ui_ModelEditDialog.h"

#include "wte/ResourcePickerDialog.h"
#include "wte/SkinEditDialog.h"
#include "wte/WtEditorContext.h"

class ModelEditDialog : public QDialog{
Q_OBJECT
public:
	struct EditResult{
		QString name;

		
		typedef QPair<QString, wt::Animation*> Animation;
		QVector<Animation> animations;

		bool ok;

		EditResult() : name(""), ok(false){
		}
	}; // </EditResult>

	static EditResult edit(QWidget* parent, wt::Assets* assets, wt::Model* model=NULL);

protected slots:
	void onSave();

	void onSkinActivated(void* item);

	void onAddAnimation();

	void onAddFromGroup();

	void onDeleteSkin();

	void onAddSkin();

private:
	ModelEditDialog(QWidget* parent, wt::Assets* assets, wt::Model* model);

	void addAnimation(const QString& name, wt::Animation* ani);

private:
	Ui::ModelEditDialog ui;
	EditResult mResult;
	wt::Model* mModel;
	wt::Assets* mAssets;

}; // </ModelEditDialog>

#endif // </WTE_DLGSKINNEDMODELEDIT_H>