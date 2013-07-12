#ifndef WT_CONDITIONVAR_H
#define WT_CONDITIONVAR_H

#include "wt/Mutex.h"

namespace wt{

class ConditionVar{
public:

	ConditionVar();

	~ConditionVar();

	void wait(Mutex& mutex);

	void wakeOne();

	void wakeAll();

#if defined(WIN32)

private:
	CONDITION_VARIABLE mWin32Handle;

#endif // </WIN32>

}; // </ConditionVar>


}; // </wt>

#endif WT_CONDITIONVAR_H // </WT_CONDITIONVAR_H>