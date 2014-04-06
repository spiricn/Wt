#ifndef WT_MEMBERCALLBACK_H
#define WT_MEMBERCALLBACK_H

#include "wt/Exception.h"
#include "wt/ACallback.h"

namespace wt
{

template<class T>
class MemberCallback :  public IEventListener{
public:
	typedef void (T::*MemberCallbackFnc)();

public:
	MemberCallback(T* instance, MemberCallbackFnc callback) : mInstance(instance), mCallback(callback){
	}

	bool handleEvent(const EventPtr event){
		(mInstance->*(mCallback))();

		return false;
	}

private:
	T* mInstance;
	MemberCallbackFnc mCallback;

}; // </MemberCallback>

} // </wt>

#endif // </WT_MEMBERCALLBACK_H>