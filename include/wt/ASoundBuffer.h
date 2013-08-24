#ifndef WT_ASOUNDBUFFER_H
#define WT_ASOUNDBUFFER_H

#include "wt/AResource.h"

namespace wt
{

class ASoundBuffer : public AResource<ASoundBuffer>{
public:
	ASoundBuffer(AResourceManager<ASoundBuffer>* manager=NULL, ResourceHandle handle=0, const String& name="") : AResource(manager, handle, name){
	}

}; // </ASoundBuffer>

} // </wt>

#endif // </WT_ASOUNDBUFFER_H>