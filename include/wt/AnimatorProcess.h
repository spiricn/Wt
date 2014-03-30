#ifndef WT_ANIMATORPROCESS_H
#define WT_ANIMATORPROCESS_H

#include "wt/AProcess.h"
#include "wt/ModelledActor.h"

namespace wt
{

class AnimatorProcess : public AProcess{
public:
	AnimatorProcess(ModelledActor* actor, const String& animation="", float speed=0.0f, bool blend=true, float blendSpeed=0.1f);

	AnimatorProcess* addAnimation(const String& animation, float speed, bool blend=true, float blendSpeed=0.1f, bool loop=false, float loopDuration=0.0f);

	void onProcStart();

	void onProcUpdate(float dt);

private:
	struct AnimationEntry{
		String name;
		float speed;
		bool blend;
		float blendSpeed;
		float loopDuration;
		bool loop;
	};

	typedef std::vector<AnimationEntry> EntryList;

private:
	EntryList mEntries;
	ModelledActor* mActor;
	float mBlendSpeed;
	bool mFirstAnimation;

}; // </AnimatorProcess>

} // </wt>

#endif // </WT_ANIMATORPROCESS_H>
