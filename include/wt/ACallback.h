#ifndef WT_ACALLBACK_H
#define WT_ACALLBACK_H

#include "wt/Sp.h"

namespace wt
{

class ACallback{
public:
	virtual ~ACallback(){
	}

	virtual void handleCallback() = 0;
}; // </ACallback>

typedef wt::Sp<ACallback> CallbackPtr;

} // </wt>

#endif // </WT_ACALLBACK_H>