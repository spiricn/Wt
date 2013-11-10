#ifndef WT_WAITPROCESS_H
#define WT_WAITPROCESS_H

#include "wt/AProcess.h"

namespace wt
{

class WaitProcess : public AProcess{
public:
	WaitProcess(float time);

	void onProcUpdate(float dt);

private:
	float mCurrentTime;
	float mDuration;

}; // </WaitProcess>

} // </wt>

#endif // </WT_WAITPROCESS_H>
