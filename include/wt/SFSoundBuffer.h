#ifndef WT_SFSOUNDBUFFER_H
#define WT_SFSOUNDBUFFER_H

#include "wt/ASoundBuffer.h"

namespace wt
{

class SFSoundBuffer : public ASoundBuffer{
public:
	sf::SoundBuffer& getSFSoundBuffer(){
		return mSoundBuffer;
	}
	
	SFSoundBuffer(AResourceManager<ASoundBuffer>* manager=NULL, ResourceHandle handle=0, const String& name="") : ASoundBuffer(manager, handle, name){
	}

private:
	sf::SoundBuffer mSoundBuffer;

}; // </ASoundBuffer>

} // </wt>

#endif // </WT_SFSOUNDBUFFER_H>