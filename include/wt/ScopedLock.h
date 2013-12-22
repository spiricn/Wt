#ifndef WT_SCOPEDLOCK_H
#define WT_SCOPEDLOCK_H

#include "wt/Mutex.h"

namespace wt
{

class ScopedLock{
public:
	ScopedLock(Mutex& mutex);

	~ScopedLock();

private:
	ScopedLock(const ScopedLock&);

	Mutex& mMutex;
}; // </ScopedLock>

} // </wt>

#endif // </WT_SCOPEDLOCK_H>
