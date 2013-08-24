#ifndef WT_ASOUNDSTREAM_H
#define WT_ASOUNDSTREAM_H

#include "wt/ASound.h"

namespace wt
{

class ASoundStream : public ASound{
public:
	virtual ~ASoundStream(){
	}

	virtual void setSource(const String& src){
		mSource = src;
	}

	const String& getSource(){
		return mSource;
	}

	ASoundBuffer* getSoundBuffer(){
		return NULL;
	}

private:
	String mSource;

}; // </ASoundStream>

} // </wt>

#endif // </WT_ASOUNDSTREAM_H>