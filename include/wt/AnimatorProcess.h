#ifndef WT_ANIMATORPROCESS_H
#define WT_ANIMATORPROCESS_H

#include "wt/AProcess.h"
#include "wt/ModelledActor.h"

namespace wt
{

class AnimatorProcess : public AProcess{
public:
	AnimatorProcess(ModelledActor* actor, const String& animation, float speed, bool blend=true, float blendSpeed=0.1f);

	void onProcStart();

	void onProcUpdate(float dt);

private:
	ModelledActor* mActor;
	String mAnimation;
	float mSpeed;
	bool mBlend;
	float mBlendSpeed;

}; // </AnimatorProcess>

} // </wt>

#endif // </WT_ANIMATORPROCESS_H>
