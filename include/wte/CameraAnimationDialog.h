#ifndef WTE_CAMERAANIMATIONDIALOG_H
#define WTE_CAMERAANIMATIONDIALOG_H

#include "ui_CameraAnimationDialog.h"

#include <wt/Scene.h>
#include <wt/NodeAnimation.h>
#include <wt/ProcessManager.h>
#include <wt/TransformableAnimator.h>

class CameraAnimationDialog : public QDialog, public wt::TransformableAnimator::IListener{
Q_OBJECT

public:
    CameraAnimationDialog(QWidget* parent, wt::Scene* scene, wt::ProcessManager* procManager);

	void onAnimationProgress(wt::TransformableAnimator* animator, float progress);

	void onAnimationStateChanged(wt::TransformableAnimator*, wt::TransformableAnimator::State);

private:
	struct Keyframe{
		int32_t id;
		float time;
		glm::vec3 pos;
		glm::quat rot;
		QTreeWidgetItem* widgetItem;
	};

	void selectKeyframe(Keyframe* kf);

	void setKeyframe();

	void generateNode(wt::NodeAnimation* node);

	typedef std::vector<Keyframe*> KeyframeSet;

    Ui::CameraAnimationDialog ui;
	wt::Scene* mScene;
	wt::ProcessManager* mProcManager;
	KeyframeSet mKeyframes;
	Keyframe* mCurrentKeyframe;
	wt::NodeAnimation* mNodeAnimation;
	wt::TransformableAnimator* mAnimationPlayer;

protected slots:
	void onAnimationSave();

	void onKeyframeAdd();

	Keyframe* addKeyframe(const glm::vec3& pos, const glm::quat& rot, float time);

	void onKeyframeTimeSet(double);

	void onKeyframeSet();

	void onKeyframeGoTo();

	void onClear();

	void onAnimationSeek(int);

	void onStop();

	void onAnimationLoad();

	void onKeyframeSelected();

	void onPlay();

}; // </CameraAnimationDialog>

#endif // </WTE_CAMERAANIMATIONDIALOG_H>