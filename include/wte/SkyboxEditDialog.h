#ifndef WTE_SKYBOXEDITDIALOG_H
#define WTE_SKYBOXEDITDIALOG_H

#include <qdialog.h>

#include <wt/SkyBox.h>

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
	wt::AResourceSystem* mAssets;

public:
	SkyboxEditDialog(QWidget* parent, wt::AResourceSystem* assets, const wt::SkyBox* skybox=NULL) : QDialog(parent), mAssets(assets){
		ui.setupUi(this);

		if(skybox){
			setSideImage(skybox, wt::SkyBox::eSIDE_POS_X);
			setSideImage(skybox, wt::SkyBox::eSIDE_POS_Y);
			setSideImage(skybox, wt::SkyBox::eSIDE_POS_Z);

			setSideImage(skybox, wt::SkyBox::eSIDE_NEG_X);
			setSideImage(skybox, wt::SkyBox::eSIDE_NEG_Y);
			setSideImage(skybox, wt::SkyBox::eSIDE_NEG_Z);


			ui.nameEdit->setText(
				skybox->getName().c_str());
		}
	}

	static EditResult createSkyBox(QWidget* parent, wt::AResourceSystem* assets, wt::SkyBox* skybox=NULL){
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

	void onAutoPick(){
		QList<wt::Image*> images;

		if(!ResourcePickerDialog::pickMultiResources<wt::Image>(this, mAssets->getImageManager(), images, "Pick images")){
			return;
		}

		for(QList<wt::Image*>::Iterator iter=images.begin(); iter!=images.end(); iter++){
			wt::Image* img = *iter;

			wt::SkyBox::Side side;

			if(!strcmp(img->getName().c_str(), "pos_x")){
				side = wt::SkyBox::eSIDE_POS_X;
			}
			else if(!strcmp(img->getName().c_str(), "pos_y")){
				side = wt::SkyBox::eSIDE_POS_Y;
			}
			else if(!strcmp(img->getName().c_str(), "neg_x")){
				side = wt::SkyBox::eSIDE_NEG_X;
			}
			else if(!strcmp(img->getName().c_str(), "neg_y")){
				side = wt::SkyBox::eSIDE_NEG_Y;
			}
			else if(!strcmp(img->getName().c_str(), "pos_z")){
				side = wt::SkyBox::eSIDE_POS_Z;
			}
			else if(!strcmp(img->getName().c_str(), "neg_z")){
				side = wt::SkyBox::eSIDE_NEG_Z;
			}
			else{
				continue;
			}

			setSideImage(side, img);
		}
	}


	void setSideImage(const wt::SkyBox* sky, wt::SkyBox::Side side){
		setSideImage(side, sky->getSideImage(side));
	}

	void setSideImage(wt::SkyBox::Side side, const wt::Image* image){
		if(image==NULL){
			return;
		}

		switch(side){
			case wt::SkyBox::eSIDE_POS_X:
				mResult.posX = image;
				ui.posXEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eSIDE_POS_Y:
				mResult.posY = image;
				ui.posYEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eSIDE_POS_Z:
				mResult.posZ = image;
				ui.posZEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eSIDE_NEG_X:
				mResult.negX= image;
				ui.negXEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eSIDE_NEG_Y:
				mResult.negY= image;
				ui.negYEdit->setText( image->getPath().c_str() );
				break;
			case wt::SkyBox::eSIDE_NEG_Z:
				mResult.negZ= image;
				ui.negZEdit->setText( image->getPath().c_str() );
				break;
		}
	}

	void onPosXClicked(){
		setSideImage(wt::SkyBox::eSIDE_POS_X,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onPosYClicked(){
		setSideImage(wt::SkyBox::eSIDE_POS_Y,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onPosZClicked(){
		setSideImage(wt::SkyBox::eSIDE_POS_Z,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onNegXClicked(){
		setSideImage(wt::SkyBox::eSIDE_NEG_X,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onNegYClicked(){
		setSideImage(wt::SkyBox::eSIDE_NEG_Y,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

	void onNegZClicked(){
		setSideImage(wt::SkyBox::eSIDE_NEG_Z,
			ResourcePickerDialog::pickResource(this, mAssets->getImageManager())
			);
	}

}; // </SkyboxEditDialog>

#endif // </WTE_SKYBOXEDITDIALOG_H>