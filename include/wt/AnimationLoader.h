#ifndef WT_ANIMATIONLOADER_H
#define WT_ANIMATIONLOADER_H


#include "wt/stdafx.h"

#include "wt/AResourceLoader.h"
#include "wt/Animation.h"

namespace wt{

class AnimationLoader : public Singleton<AnimationLoader>, public AResourceLoader<Animation>{
private:
	static const char* TAG;
	static const char* FORMAT_ID;

public:
	void load(const String& path, Animation* ani);

	void save(const String& path, Animation* ani);

	void postProcess(Animation* ani, const glm::mat4& transform);

}; // </AnimationLoader

}; // </wt>

#endif
