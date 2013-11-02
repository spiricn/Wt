#include "stdafx.h"

#include "wte/CameraAnimationDialog.h"
#include <wt/TransformableAnimator.h>

#define TD_TRACE_TAG "CameraAnimationDialog"

CameraAnimationDialog::CameraAnimationDialog(QWidget* parent, wt::Scene* scene, wt::ProcessManager* procManager) : QDialog(parent), 
	mScene(scene), mCurrentKeyframe(NULL), mProcManager(procManager), mNodeAnimation(NULL), mAnimationPlayer(NULL){

    ui.setupUi(this);
}

CameraAnimationDialog::Keyframe* CameraAnimationDialog::addKeyframe(const glm::vec3& pos, const glm::quat& rot, float time){
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
	
	item->setText(0, QString("Keyframe %1").arg(mKeyframes.size()+1));
	item->setData(0, Qt::UserRole, QVariant(mKeyframes.size()));

	Keyframe* kf = new Keyframe;
	kf->id = mKeyframes.size();
	kf->time = time;
	kf->pos = pos;
	kf->rot = rot;

	kf->widgetItem = item;

	mKeyframes.push_back(kf);

	return kf;
}

void CameraAnimationDialog::onAnimationSeek(int pos){
	if(mAnimationPlayer){
		mAnimationPlayer->setPosition( ((float)pos/100.0f) * mAnimationPlayer->getNodeAnimation()->getDuration() );
	}
}

void CameraAnimationDialog::onClear(){
	ui.treeWidget->clear();
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		delete *iter;
	}
}

void CameraAnimationDialog::onKeyframeAdd(){
	glm::vec3 pos;
	glm::quat rot;
	Keyframe* kf = addKeyframe(pos, rot, 1.0f);

	selectKeyframe(kf);
	setKeyframe();
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
	ui.labelTime->setText(
		QString("Time: %1 sec").arg(progress*animator->getNodeAnimation()->getDuration())
	);

	wt::NodeAnimation::PositionKey* key = animator->getNodeAnimation()->getPosKeyAt(progress*animator->getNodeAnimation()->getDuration());
}

void CameraAnimationDialog::generateNode(wt::NodeAnimation* node){
	float time = 0.0f;

	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		time += (*iter)->time;

		wt::NodeAnimation::PositionKey* posKey = node->addPositionKey();

		posKey->value.x = (*iter)->pos.x;
		posKey->value.y = (*iter)->pos.y;
		posKey->value.z = (*iter)->pos.z;
		posKey->time = time;

		wt::NodeAnimation::RotationKey* rotKey = node->addRotationKey();

		rotKey->value.x = (*iter)->rot.x;
		rotKey->value.y = (*iter)->rot.y;
		rotKey->value.z = (*iter)->rot.z;
		rotKey->value.w = (*iter)->rot.w;
		rotKey->time = time;
	}
}

void CameraAnimationDialog::onAnimationLoad(){
	QString res = QFileDialog::getOpenFileName(this, "Load animation", ".", "*.wta");

	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	mScene->getAssets()->getAnimationManager()->getLoader()->load(res.toStdString(), anim);

	wt::NodeAnimation* node = *anim->getNodeAnimationList().begin();

	WT_ASSERT(node->getNumPosKeys() == node->getNumRotKeys(), "Invalid camera animation");

	float prevTime = 0.0f;

	wt::NodeAnimation::PosKeyIter posKeyIter = node->getPosKeysBegin();
	wt::NodeAnimation::RotKeyIter rotKeyIter = node->getRotKeysBegin();

	for(uint32_t i=0; i<node->getNumPosKeys(); i++, posKeyIter++, rotKeyIter++){
		WT_ASSERT((*posKeyIter)->time == (*rotKeyIter)->time, "Invalid camera animation");

		addKeyframe((*posKeyIter)->value, (*rotKeyIter)->value, (*posKeyIter)->time-prevTime);

		prevTime = (*posKeyIter)->time;
	}

	delete anim;

	LOGI("Animation load form \"%s\"", res.toStdString().c_str());
}

void CameraAnimationDialog::onKeyframeDelete(){
	if(!mCurrentKeyframe){
		return;
	}

	TRACEW("TODO");
}

void CameraAnimationDialog::onKeyframeGoTo(){
	if(!mCurrentKeyframe){
		return;
	}

	mScene->getCamera().setTranslation(mCurrentKeyframe->pos);
	mScene->getCamera().setRotation(mCurrentKeyframe->rot);
}

void CameraAnimationDialog::onAnimationSave(){
	QString res = QFileDialog::getSaveFileName(this, "Save animation", ".", "*.wta");;
	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	wt::NodeAnimation* node = anim->addNodeAnimation();
	generateNode(node);

	node->setName("default");

	mScene->getAssets()->getAnimationManager()->getLoader()->save(res.toStdString(), anim);

	anim->setDuration(node->getDuration());

	delete anim;

	LOGI("Animation saved to \"%s\"", res.toStdString().c_str());
}

void CameraAnimationDialog::onAnimationStateChanged(wt::TransformableAnimator*, wt::TransformableAnimator::State state){
	if(state == wt::TransformableAnimator::eSTATE_FINISHED){
		mAnimationPlayer = NULL;
	}
}

void CameraAnimationDialog::onPlay(){
	if(mAnimationPlayer && mAnimationPlayer->getState() != wt::TransformableAnimator::eSTATE_FINISHED){
		if(mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_PLAYING){
			LOGD("Pause");
			mAnimationPlayer->pause();
		}
		else if(mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_PAUSED){
			LOGD("Continue");
			mAnimationPlayer->play();
		}
	}
	else{
		LOGD("Play");
		if(mNodeAnimation){
			delete mNodeAnimation;
		}

		mNodeAnimation = new wt::NodeAnimation;

		generateNode(mNodeAnimation);

		LOG("Playing animation %.2f seconds", mNodeAnimation->getDuration());

		mAnimationPlayer = new wt::TransformableAnimator(&mScene->getCamera(), mNodeAnimation, false, false);
		mProcManager->attach( mAnimationPlayer );

		mAnimationPlayer->setListener(this);
	}
}

void CameraAnimationDialog::onStop(){
}

void CameraAnimationDialog::onKeyframeSelected(){
	QTreeWidgetItem* item  = ui.treeWidget->currentItem();

	selectKeyframe( mKeyframes[item->data(0, Qt::UserRole).toInt()] );
}