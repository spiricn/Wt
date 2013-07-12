#ifndef WT_ANODE_H
#define WT_ANODE_H


#include "wt/stdafx.h"

#include "wt/Exception.h"

namespace wt{

template<typename T> /** Curiously recurring template pattern  */
class ANode{
public:
	typedef std::vector<T*> NodeList;
	typedef typename NodeList::iterator Iterator;
	typedef typename NodeList::const_iterator ConstIterator;

protected:
	T* mParent;
	NodeList mChildren;
	String mName;

	T* addChild(const String& name, T* node){
		if(containsChild(name)){
			WT_THROW("ANode already contains a child named \"%s\"", name.c_str());
		}
		mChildren.push_back(node);
		return node;
	}

public:
	ANode(T* parent, const String& name) : mParent(parent), mName(name){
	}

	virtual ~ANode(){
	}

	bool containsChild(const String& name) const{
		for(ConstIterator i=getBeg(); i!=getEnd(); i++){
			if((*i)->getName().compare(name)==0){
				return true;
			}
		}
		return false;
	}

	const String& getName() const{
		return mName;
	}

	Iterator getBeg(){
		return mChildren.begin();
	}

	Iterator getEnd(){
		return mChildren.end();
	}

	ConstIterator getBeg() const{
		return mChildren.cbegin();
	}

	ConstIterator getEnd() const{
		return mChildren.cend();
	}

	T* getParent(){
		return mParent;
	}

	const T* getParent() const{
		return (const T*)mParent;
	}

	T* getChildAt(Int32 idx){
#if WT_CHECKED
		WT_ASSERT(idx < mChildren.size()
			&& idx >= 0, "Invalid node child index %d (must be in range [0, %d)",
			idx, mChildren.size()-1);
#endif
		return mChildren[idx];
	}

	wt::Uint32 getNumChildren(bool recursive) const{
		if(recursive){
			Uint32 count=0;

		
			for(ConstIterator i=getBeg(); i!=getEnd(); i++){
				count += (*i)->getNumChildren(true) + 1;
			}
			return count;
		}
		else{
			return mChildren.size();
		}
	}

	bool isLeaf() const{
		return mChildren.empty();
	}

	virtual T* findChildByName(const String& name, bool recursive=true){
		if(mName.compare(name)==0){
			return (T*)this;
		}

		for(NodeList::iterator i=mChildren.begin(); i!=mChildren.end(); i++){
			T* res = (*i)->findChildByName(name, recursive);
			if(res != NULL){
				return res;
			}
		}

		return NULL;
	}

	virtual void destroy(){
		for(NodeList::iterator i=mChildren.begin(); i!=mChildren.end(); i++){
			(*i)->destroy();
			delete *i;
		}
		
		mChildren.clear();
	}

}; // </ANode>


class SpatialNode : public ANode<SpatialNode>{
protected:
	glm::mat4 mTransform;

public:
	SpatialNode(SpatialNode* parent, const String& name) : ANode(parent, name){
	}

	void setTransform(const glm::mat4& transform){
		mTransform = transform;
	}

	const glm::mat4 getTransform() const{
		return mTransform;
	}

	void getGlobalTransform(glm::mat4& dst){
		dst = mTransform;
		
		if(mParent==NULL){
			return;
		}

		SpatialNode* node = mParent;
		do{
			dst = node->mTransform * dst;
			node = node->mParent;
		}while(node != NULL);
	}

	SpatialNode* addChild(const String& name){
		return ANode::addChild(name, new SpatialNode(this, name));
	}
	
}; // </SpatialNode>

}; // </wt>

#endif
