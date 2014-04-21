#include "stdafx.h"

#include "wte/TransformableAnimationDialog.h"
#include <wt/TransformableAnimator.h>
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "TransformableAnimationDialog"

TransformableAnimationDialog::TransformableAnimationDialog(QWidget* parent, wt::ATransformable* tf, wt::ProcessManager* procManager) : QDialog(parent), 
	mTarget(tf), mCurrentKeyframe(NULL), mProcManager(procManager), mNodeAnimation(NULL), mAnimationPlayer(NULL){

    ui.setupUi(this);

	mNodeAnimation = new wt::NodeAnimation;

	mAnimationPlayer = new wt::TransformableAnimator(mTarget, mNodeAnimation, false, false);
	mAnimationPlayer->pause();

	mProcManager->attach( mAnimationPlayer );

	mAnimationPlayer->setListener(this);

	mAnimationPlayer->setAnimationAttribs(0xFFFFFFFF);
}

float TransformableAnimationDialog::getKeyframeAbsTime(Keyframe* kf){
	float absTime = 0.0f;

	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		absTime += (*iter)->time;

		if(*iter == kf){
			break;
		}
	}

	return absTime;
}

TransformableAnimationDialog::Keyframe* TransformableAnimationDialog::addKeyframe(const glm::vec3& pos, const glm::quat& rot, float relTime){
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

void TransformableAnimationDialog::onAnimationSeek(int pos){
	if(mAnimationPlayer){
		mAnimationPlayer->setPosition( ((float)pos/100.0f) * mAnimationPlayer->getNodeAnimation()->getDuration() );
	}
}

void TransformableAnimationDialog::onClear(){
	clear();
}

void TransformableAnimationDialog::onKeyframeAdd(){
	glm::vec3 pos;
	glm::quat rot;
	Keyframe* kf = addKeyframe(pos, rot, mKeyframes.empty() ? 0.0f : 1.0f);

	selectKeyframe(kf);
	setKeyframe();
}

void TransformableAnimationDialog::selectKeyframe(Keyframe* newKeyframe){
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

void TransformableAnimationDialog::onKeyframeTimeSet(double val){
	if(!mCurrentKeyframe){
		return;
	}

	if(mCurrentKeyframe->time == val){
		return;
	}

	mCurrentKeyframe->time = val;
	refreshTimes();
}

void TransformableAnimationDialog::setKeyframe(){
	mTarget->getTranslation(mCurrentKeyframe->posKey->value);
	mTarget->getRotation(mCurrentKeyframe->rotKey->value);
}

void TransformableAnimationDialog::onKeyframeSet(){
	if(!mCurrentKeyframe){
		return;
	}

	setKeyframe();
}

void TransformableAnimationDialog::onAnimationProgress(wt::TransformableAnimator* animator, float progress){
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
void TransformableAnimationDialog::refreshTimes(){
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		const float absTime = getKeyframeAbsTime(*iter);
		(*iter)->posKey->time = absTime;
		(*iter)->rotKey->time = absTime;
	}
}

void TransformableAnimationDialog::onKeyframeDelete(){
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

void TransformableAnimationDialog::onKeyframeGoTo(){
	if(!mCurrentKeyframe){
		return;
	}

	mTarget->setTranslation(mCurrentKeyframe->posKey->value);
	mTarget->setRotation(mCurrentKeyframe->rotKey->value);
}

void TransformableAnimationDialog::onAnimationStateChanged(wt::TransformableAnimator*, wt::TransformableAnimator::State state){
}

void TransformableAnimationDialog::onPlay(){
	if(mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_PLAYING){
		mAnimationPlayer->pause();
	}
	else if(mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_PAUSED 
		|| mAnimationPlayer->getState() == wt::TransformableAnimator::eSTATE_FINISHED){
		LOGD("Continue");
		mAnimationPlayer->play();
	}
}

void TransformableAnimationDialog::onStop(){
	mAnimationPlayer->stop();
}

TransformableAnimationDialog::Keyframe* TransformableAnimationDialog::findById(int32_t id){
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		if((*iter)->id == id){
			return *iter;
		}
	}

	return NULL;
}


void TransformableAnimationDialog::onKeyframeSelected(){
	QTreeWidgetItem* item  = ui.treeWidget->currentItem();

	selectKeyframe( findById( item->data(0, Qt::UserRole).toInt() ) );
}


void TransformableAnimationDialog::onAnimationSave(){
	QString res = QFileDialog::getSaveFileName(this, "Save animation", ".", "*.wta");;
	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	wt::NodeAnimation* node = anim->addNodeAnimation();

	wt::NodeAnimation::clone(mNodeAnimation, node);
	
	node->setName("default");

	wt::StreamPtr stream = new wt::FileIOStream(res.toStdString(), wt::AIOStream::eMODE_WRITE);

	WTE_CTX.getAssets()->getAnimationManager()->getLoader()->save(stream, anim);

	anim->setDuration(node->getDuration());

	delete anim;

	LOGI("Animation saved to \"%s\"", res.toStdString().c_str());
}

void TransformableAnimationDialog::clear(){
	for(KeyframeSet::iterator iter=mKeyframes.begin(); iter!=mKeyframes.end(); iter++){
		delete *iter;
	}

	mNodeAnimation->clear();

	ui.treeWidget->clear();

	mKeyframes.clear();

	mCurrentKeyframe = NULL;
	selectKeyframe(NULL);
}

void TransformableAnimationDialog::onAnimationLoad(){
	QString res = QFileDialog::getOpenFileName(this, "Load animation", ".", "*.wta");

	if(!res.size()){
		return;
	}

	wt::Animation* anim = new wt::Animation;

	wt::StreamPtr stream = new wt::FileIOStream(res.toStdString(), wt::AIOStream::eMODE_READ);

	WTE_CTX.getAssets()->getAnimationManager()->getLoader()->load(stream, anim);

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