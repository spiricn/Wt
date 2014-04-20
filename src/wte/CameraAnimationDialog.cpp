#include "stdafx.h"

#include "wte/CameraAnimationDialog.h"
#include <wt/TransformableAnimator.h>
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "CameraAnimationDialog"

CameraAnimationDialog::CameraAnimationDialog(QWidget* parent, wt::Scene* scene, wt::ProcessManager* procManager) : QDialog(parent), 
	mScene(scene), mCurrentKeyframe(NULL), mProcManager(procManager), mNodeAnimation(NULL), mAnimationPlayer(NULL){

    ui.setupUi(this);

	mNodeAnimation = new wt::NodeAnimation;

	mAnimationPlayer = new wt::TransformableAnimator(&mScene->getCamera(), mNodeAnimation, false, false);
	mAnimationPlayer->pause();

	mProcManager->attach( mAnimationPlayer );

	mAnimationPlayer->setListener(this);
}

float CameraAnimationDialog::getKeyframeAbsTime(Keyframe* kf){
	float absTime = 0.0f;

	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		absTime += (*iter)->time;

		if(*iter == kf){
			break;
		}
	}

	return absTime;
}

CameraAnimationDialog::Keyframe* CameraAnimationDialog::addKeyframe(const glm::vec3& pos, const glm::quat& rot, float relTime){
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
	
	item->setText(0, QString("Keyframe %1").arg(mKeyframes.size()+1));

	Keyframe* kf = new Keyframe;

	kf->id = 0;
	do{
		if(findById(kf->id) == NULL){
			break;
		}
		++kf->id;
	}while(1);

	item->setData(0, Qt::UserRole, QVariant(kf->id));

	kf->time = relTime;

	kf->posKey = mNodeAnimation->addPositionKey();
	kf->posKey->value = pos;

	kf->rotKey = mNodeAnimation->addRotationKey();
	kf->rotKey->value = rot;
	

	kf->widgetItem = item;

	mKeyframes.push_back(kf);

	kf->posKey->time = kf->rotKey->time = getKeyframeAbsTime(kf);

	return kf;
}

void CameraAnimationDialog::onAnimationSeek(int pos){
	if(mAnimationPlayer){
		mAnimationPlayer->setPosition( ((float)pos/100.0f) * mAnimationPlayer->getNodeAnimation()->getDuration() );
	}
}

void CameraAnimationDialog::onClear(){
	clear();
}

void CameraAnimationDialog::onKeyframeAdd(){
	glm::vec3 pos;
	glm::quat rot;
	Keyframe* kf = addKeyframe(pos, rot, mKeyframes.empty() ? 0.0f : 1.0f);

	selectKeyframe(kf);
	setKeyframe();
}

void CameraAnimationDialog::selectKeyframe(Keyframe* newKeyframe){
	Keyframe* prevKeyframe = mCurrentKeyframe;

	mCurrentKeyframe = newKeyframe;

	if(prevKeyframe){
		prevKeyframe->widgetItem->setBackgroundColor(0, QColor(255, 255, 255));
	}
	
	if(newKeyframe){
		newKeyframe->widgetItem->setBackgroundColor(0, QColor(0, 255, 0));
		
		ui.time->setValue(newKeyframe->time);
		ui.keyframeGroup->setTitle(QString("Keyframe %1").arg(newKeyframe->id + 1));
		ui.treeWidget->scrollToItem(newKeyframe->widgetItem, QAbstractItemView::EnsureVisible);
	}
	else{
		ui.time->setValue(0.0f);
		ui.keyframeGroup->setTitle("");
	}
}

void CameraAnimationDialog::onKeyframeTimeSet(double val){
	if(!mCurrentKeyframe){
		return;
	}

	if(mCurrentKeyframe->time == val){
		return;
	}

	mCurrentKeyframe->time = val;
	refreshTimes();
}

void CameraAnimationDialog::setKeyframe(){
	mScene->getCamera().getTranslation(mCurrentKeyframe->posKey->value);
	mScene->getCamera().getRotation(mCurrentKeyframe->rotKey->value);
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

	// TODO
	wt::NodeAnimation::PositionKey* key = animator->getNodeAnimation()->getPosKeyAt(progress*animator->getNodeAnimation()->getDuration());

	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		if((*iter)->posKey == key){
			selectKeyframe(*iter);
			break;
		}
	}
}

// Brute force approach to updating NodeAnimation's keyframe absolute time from our relatives
void CameraAnimationDialog::refreshTimes(){
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		const float absTime = getKeyframeAbsTime(*iter);
		(*iter)->posKey->time = absTime;
		(*iter)->rotKey->time = absTime;
	}
}

void CameraAnimationDialog::onKeyframeDelete(){
	QList<QTreeWidgetItem*> items = ui.treeWidget->selectedItems();

	for(QList<QTreeWidgetItem*>::Iterator iter=items.begin(); iter!=items.end(); iter++){
		Keyframe* kf = findById( (*iter)->data(0, Qt::UserRole).toInt() );

		delete ui.treeWidget->takeTopLevelItem( ui.treeWidget->indexOfTopLevelItem( *iter ) );

		mKeyframes.erase( std::find(mKeyframes.begin(), mKeyframes.end(), kf) );

		mNodeAnimation->deletePositionKey(kf->posKey);
		mNodeAnimation->deleteRotationKey(kf->rotKey);

		bool currentSelected = kf == mCurrentKeyframe;

		delete kf;

		if(currentSelected){
			mCurrentKeyframe = NULL;
			selectKeyframe(NULL);
		}		
	}

	refreshTimes();
}

void CameraAnimationDialog::onKeyframeGoTo(){
	if(!mCurrentKeyframe){
		return;
	}

	mScene->getCamera().setTranslation(mCurrentKeyframe->posKey->value);
	mScene->getCamera().setRotation(mCurrentKeyframe->rotKey->value);
}

void CameraAnimationDialog::onAnimationStateChanged(wt::TransformableAnimator*, wt::TransformableAnimator::State state){
}

void CameraAnimationDialog::onPlay(){
	if(mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_PLAYING){
		mAnimationPlayer->pause();
	}
	else if(mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_PAUSED 
		|| mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_FINISHED){
		LOGD("Continue");
		mAnimationPlayer->play();
	}
}

void CameraAnimationDialog::onStop(){
	mAnimationPlayer->stop();
}

CameraAnimationDialog::Keyframe* CameraAnimationDialog::findById(int32_t id){
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		if((*iter)->id == id){
			return *iter;
		}
	}

	return NULL;
}


void CameraAnimationDialog::onKeyframeSelected(){
	QTreeWidgetItem* item  = ui.treeWidget->currentItem();

	selectKeyframe( findById( item->data(0, Qt::UserRole).toInt() ) );
}


void CameraAnimationDialog::onAnimationSave(){
	QString res = QFileDialog::getSaveFileName(this, "Save animation", ".", "*.wta");;
	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	wt::NodeAnimation* node = anim->addNodeAnimation();

	wt::NodeAnimation::clone(mNodeAnimation, node);
	
	node->setName("default");

	wt::StreamPtr stream = new wt::FileIOStream(res.toStdString(), wt::AIOStream::eMODE_WRITE);

	mScene->getAssets()->getAnimationManager()->getLoader()->save(stream, anim);

	anim->setDuration(node->getDuration());

	delete anim;

	LOGI("Animation saved to \"%s\"", res.toStdString().c_str());
}

void CameraAnimationDialog::clear(){
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		delete *iter;
	}

	mNodeAnimation->clear();

	ui.treeWidget->clear();

	mKeyframes.clear();

	mCurrentKeyframe = NULL;
	selectKeyframe(NULL);
}

void CameraAnimationDialog::onAnimationLoad(){
	QString res = QFileDialog::getOpenFileName(this, "Load animation", ".", "*.wta");

	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	wt::StreamPtr stream = new wt::FileIOStream(res.toStdString(), wt::AIOStream::eMODE_READ);

	mScene->getAssets()->getAnimationManager()->getLoader()->load(stream, anim);

	wt::NodeAnimation* node = *anim->getNodeAnimationList().begin();

	WT_ASSERT(node->getNumPosKeys() == node->getNumRotKeys(), "Invalid camera animation");

	float prevTime = 0.0f;

	wt::NodeAnimation::PosKeyIter posKeyIter = node->getPosKeysBegin();
	wt::NodeAnimation::RotKeyIter rotKeyIter = node->getRotKeysBegin();

	clear();

	for(uint32_t i=0; i<node->getNumPosKeys(); i++, posKeyIter++, rotKeyIter++){
		WT_ASSERT((*posKeyIter)->time == (*rotKeyIter)->time, "Invalid camera animation");

		addKeyframe((*posKeyIter)->value, (*rotKeyIter)->value, (*posKeyIter)->time-prevTime);

		prevTime = (*posKeyIter)->time;
	}

	delete anim;

	LOGI("Animation load form \"%s\"", res.toStdString().c_str());
}