#include "stdafx.h"

#include "wte/CameraAnimationDialog.h"
#include <wt/TransformableAnimator.h>

#define TD_TRACE_TAG "CameraAnimationDialog"

CameraAnimationDialog::CameraAnimationDialog(QWidget* parent, wt::Scene* scene, wt::ProcessManager* procManager) : QDialog(parent), 
	mScene(scene), mCurrentKeyframe(NULL), mProcManager(procManager), mNodeAnimation(NULL){

    ui.setupUi(this);
}


void CameraAnimationDialog::onKeyframeAdd(){
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);

	
	item->setText(0, QString("Keyframe %1").arg(mKeyframes.size()+1));
	item->setData(0, Qt::UserRole, QVariant(mKeyframes.size()));

	Keyframe* kf = new Keyframe;
	kf->id = mKeyframes.size();
	kf->time = 0.0f;

	kf->widgetItem = item;

	mKeyframes.push_back(kf);

	selectKeyframe(kf);
	setKeyframe();
	ui.time->setValue(1.0f);
}

void CameraAnimationDialog::selectKeyframe(Keyframe* kf){
	mCurrentKeyframe = kf;

	ui.time->setValue(mCurrentKeyframe->time);

	ui.keyframeGroup->setTitle(QString("Keyframe %1").arg(mCurrentKeyframe->id + 1));
}


void CameraAnimationDialog::onKeyframeTimeSet(double val){
	if(!mCurrentKeyframe){
		return;
	}

	mCurrentKeyframe->time = val;
}

void CameraAnimationDialog::setKeyframe(){
	mScene->getCamera().getTranslation(mCurrentKeyframe->pos);
	mScene->getCamera().getRotation(mCurrentKeyframe->rot);
}

void CameraAnimationDialog::onKeyframeSet(){
	if(!mCurrentKeyframe){
		return;
	}

	setKeyframe();
}

void CameraAnimationDialog::onAnimationProgress(wt::TransformableAnimator* animator, float progress){
	ui.horizontalSlider->setValue(100*progress);
}

void CameraAnimationDialog::generateNode(wt::NodeAnimation* node){
	float time = 0.0f;

	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		node->getPositionKeys().push_back(wt::NodeAnimation::PositionKey(
			(*iter)->pos.x,
			(*iter)->pos.y,
			(*iter)->pos.z,
			time
		));

		node->getRotationKeys().push_back(wt::NodeAnimation::RotationKey(
			(*iter)->rot.x,
			(*iter)->rot.y,
			(*iter)->rot.z,
			(*iter)->rot.w,
			time
		));

		time += (*iter)->time;
	}

	node->setDuration(time);
}

void CameraAnimationDialog::onAnimationSave(){
	QString res = QFileDialog::getSaveFileName(this, "Save animation", ".", "*.wta");;
	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	wt::NodeAnimation* node = anim->addNodeAnimation();
	generateNode(node);

	node->setTargetNode("default");

	mScene->getAssets()->getAnimationManager()->getLoader()->save(res.toStdString(), anim);
}

void CameraAnimationDialog::onPlay(){
	if(mNodeAnimation){
		delete mNodeAnimation ;
	}

	mNodeAnimation = new wt::NodeAnimation;

	generateNode(mNodeAnimation);

	LOG("Playing animation %.2f seconds", mNodeAnimation->getDuration());

	wt::TransformableAnimator* player;
	mProcManager->attach( player = new wt::TransformableAnimator(&mScene->getCamera(), mNodeAnimation, false) );
	player->setListener(this);
}

void CameraAnimationDialog::onKeyframeSelected(){
	QTreeWidgetItem* item  = ui.treeWidget->currentItem();

	selectKeyframe( mKeyframes[item->data(0, Qt::UserRole).toInt()] );
}