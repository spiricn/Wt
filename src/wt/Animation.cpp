#include "wt/stdafx.h"

#include "wt/Animation.h"

namespace wt
{

Animation::Animation(AResourceManager<Animation>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name),
	mDuration(0.0f){
}

void Animation::setDuration(float dur){
	mDuration = dur;
	for(NodeAnimationList::iterator i=mNodeAnimations.begin(); i!=mNodeAnimations.end(); i++){
		(*i)->setDuration(dur);
	}
}

float Animation::getDuration() const{
	return mDuration;
}

NodeAnimation* Animation::addNodeAnimation(){
	NodeAnimation* res = new NodeAnimation();
	mNodeAnimations.push_back(res);
	res->setDuration(mDuration);
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
		if((*iter)->getTargetNode().compare(name) == 0){
			return *iter;
		}
	}

	return NULL;
}

} // </wt>