#ifndef WT_SFSOUNDLOADER_H
#define WT_SFSOUNDLOADER_H

#include "wt/SFSoundBuffer.h"

namespace wt
{

class SFSoundLoader: public AResourceLoader<ASoundBuffer>, public Singleton<SFSoundLoader>{
public:
	void load(AIOStream* stream, ASoundBuffer* dst){
		SFSoundBuffer* bfr = static_cast<SFSoundBuffer*>(dst);
		//bfr->getSFSoundBuffer().load

		char* data = new char[stream->getSize()];

		stream->read(data, stream->getSize());

		if(!bfr->getSFSoundBuffer().loadFromMemory(data, stream->getSize())){
			WT_THROW("Error loading sound from stream");
		}

		delete[] data;
		//dst->setUri(path);
	}

	void save(AIOStream* stream, ASoundBuffer* dst){
		LOGW("Saving not implemented!");
	}

}; // </SFSoundLoader>


} // </wt>

#endif // </WT_SFSOUNDLOADER_H>