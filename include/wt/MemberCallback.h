#ifndef WT_MEMBERCALLBACK_H
#define WT_MEMBERCALLBACK_H

#include "wt/Exception.h"
#include "wt/ACallback.h"

namespace wt
{

template<class T>
class MemberCallback : public ACallback{
public:
	typedef void (T::*MemberCallbackFnc)();

	MemberCallback(T* instance, MemberCallbackFnc callback) : mInstance(instance), mCallback(callback){
	}

	void handleCallback(){
		WT_ASSERT(mInstance != NULL, "Invalid instance pointer");

		WT_ASSERT(mCallback != NULL, "Invalid callback pointer");

		(mInstance->*(mCallback))();
	}

private:
	T* mInstance;
	MemberCallbackFnc mCallback;

}; // </MemberCallback>

} // </wt>

#endif // </WT_MEMBERCALLBACK_H>