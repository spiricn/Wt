#ifndef WT_ASOUND_H
#define WT_ASOUND_H

#include "wt/AResource.h"

namespace wt{

class ASoundBuffer : public AResource<ASoundBuffer>{
public:
	ASoundBuffer(ResourceHandle handle=0, const String& name="") : AResource(handle, name){
	}
};


class ASound{
public:
	enum Status{
		ePLAYING,
		ePAUSED,
		eSTOPPED
	};

public:

	virtual ~ASound(){
	}

	virtual Status getStatus() const = 0;

	virtual void play() = 0;

	virtual void setLoop(bool state) = 0;

	virtual void stop() = 0;

	/** range [0.0f, 1.0f] */
	virtual void setVolume(float volume) = 0;

	virtual void setPosition(const glm::vec3& position) = 0;

	virtual glm::vec3 getPosition() const = 0;

	virtual bool isPlaying() const{
		return getStatus() == ePLAYING;
	}

	virtual void setRelativeToListener(bool state) = 0;

}; // </ASound>

class ASoundStream : public ASound{
private:
	String mSource;

public:
	virtual ~ASoundStream(){
	}

	virtual void setSource(const String& src){
		mSource = src;
	}

	const String& getSource(){
		return mSource;
	}
}; // </ASoundStream>


}; // </wt>


#endif