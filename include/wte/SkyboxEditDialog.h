#ifndef WTE_SKYBOXEDITDIALOG_H
#define WTE_SKYBOXEDITDIALOG_H

#include <qdialog.h>

#include <wt/SkyBox.h>
#include <wt/ImageManager.h>
#include <wt/Assets.h>

#include "ui_SkyboxEditDialog.h"

#include "wte/ResourcePickerDialog.h"

class SkyboxEditDialog : public QDialog{
Q_OBJECT
public:
	struct EditResult{
		const wt::Image *posX, *posY, *posZ, *negX, *negY, *negZ;
		QString name;
		bool ok;
		EditResult(){
			posX=posY=posZ=negX=negY=negZ=NULL;
			name="";
			ok=false;
		}
	};
private:
	Ui::SkyboxEditDialog ui;
	EditResult mResult;
	wt::Assets* mAssets;

public:
	SkyboxEditDialog(QWidget* parent, wt::Assets* assets, const wt::SkyBox* skybox=NULL) : QDialog(parent), mAssets(assets){
		ui.setupUi(this);

		if(skybox){
			setSideImage(skybox, wt::SkyBox::ePOS_X);
			setSideImage(skybox, wt::SkyBox::ePOS_Y);
			setSideImage(skybox, wt::SkyBox::ePOS_Z);

			setSideImage(skybox, wt::SkyBox::eNEG_X);
			setSideImage(skybox, wt::SkyBox::eNEG_Y);
			setSideImage(skybox, wt::SkyBox::eNEG_Z);


			ui.nameEdit->setText(
				skybox->getName().c_str());
		}
	}

	static EditResult createSkyBox(QWidget* parent, wt::Assets* assets, wt::SkyBox* skybox=NULL){
		SkyboxEditDialog dlg(parent, assets, skybox);
		dlg.exec();
		return dlg.mResult;
	}

protected slots:
	void onSaveClicked(){
		mResult.ok=true;
		mResult.name=ui.nameEdit->text();
		close();
	}

	void setSideImage(const wt::SkyBox* sky, wt::SkyBox::Side side){
		setSideImage(side, (const wt::Image*)sky->getSideImage(side).getPtr());
	}

	void setSideImage(wt::SkyBox::Side side, const wt::Image* image){
		if(image==NULL){
			return;
		}

		switch(side){
			case wt::SkyBox::ePOS_X:
				mResult.posX = image;
				ui.posXEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::ePOS_Y:
				mResult.posY = image;
				ui.posYEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::ePOS_Z:
				mResult.posZ = image;
				ui.posZEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eNEG_X:
				mResult.negX= image;
				ui.negXEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eNEG_Y:
				mResult.negY= image;
				ui.negYEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eNEG_Z:
				mResult.negZ= image;
				ui.negZEdit->setText( image->getPath().c_str() );
				break;
		}
	}

	void onPosXClicked(){
		setSideImage(wt::SkyBox::ePOS_X,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onPosYClicked(){
		setSideImage(wt::SkyBox::ePOS_Y,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onPosZClicked(){
		setSideImage(wt::SkyBox::ePOS_Z,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onNegXClicked(){
		setSideImage(wt::SkyBox::eNEG_X,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onNegYClicked(){
		setSideImage(wt::SkyBox::eNEG_Y,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onNegZClicked(){
		setSideImage(wt::SkyBox::eNEG_Z,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

}; // </SkyboxEditDialog>

#endif // </WTE_SKYBOXEDITDIALOG_H>