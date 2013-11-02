#include "wt/stdafx.h"

#include "wt/Animation.h"

namespace wt
{

Animation::Animation(AResourceManager<Animation>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name),
	mDuration(0.0f){
}

void Animation::setDuration(float dur){
	mDuration = dur;
}

float Animation::getDuration() const{
	return mDuration;
}

NodeAnimation* Animation::addNodeAnimation(const String& name){
	NodeAnimation* res = new NodeAnimation(name);
	mNodeAnimations.push_back(res);
	return res;
}

Animation::~Animation(){
	for(size_t i=0; i<mNodeAnimations.size(); i++){
		delete mNodeAnimations[i];
	}
}

Animation::NodeAnimationList& Animation::getNodeAnimationList(){
	return mNodeAnimations;
}

NodeAnimation* Animation::findNodeAnimation(const String& name){
	for(NodeAnimationList::iterator iter=mNodeAnimations.begin(); iter!=mNodeAnimations.end(); iter++){
		if((*iter)->getName().compare(name) == 0){
			return *iter;
		}
	}

	return NULL;
}

} // </wt>