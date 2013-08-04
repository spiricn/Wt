#ifndef WT_ARESOURCE_H
#define WT_ARESOURCE_H


#include "wt/stdafx.h"


#include "wt/Utils.h"
#include "wt/ASerializable.h"

template<typename T>
class AResourceGroup;

template<typename T>
class AResourceManager;

namespace wt{

typedef unsigned long ResourceHandle;

template<class T>
class AResource;

template<class T>
class ResourceLink{
private:
	String mPath;
	AResource<T>* mPtr;
public:
	ResourceLink(AResource<T>* ptr, const String& path) : mPtr(ptr),
		mPath(path){
	}

	ResourceLink() : mPtr(NULL), mPath(""){
	}


	ResourceLink(const ResourceLink& other){
		mPath = other.mPath;
		mPtr = other.mPtr;
	}

	AResource<T>* getPtr(){
		return mPtr;
	}

	const AResource<T>* getPtr() const{
		return mPtr;
	}

	const String& getPath() const{
		return mPath;
	}
}; // </ResourceLink>

class AResourceSystem;

template<class T>
class AResource : public lua::ASerializable{
public:
	enum ResourceState{
		eUNLOADED,
		eLOADED,
		eREADY
	}; // </ResourceState>

protected:
	String mName;
	String mUri;
	ResourceHandle mHandle;
	AResourceGroup<T>* mGroup;
	ResourceState mState;
	AResourceManager<T>* mManager;


private:
	AResource(const AResource& other){}

public:
	const String& getUri() const{
		return mUri;
	}

	AResourceManager<T>* getManager() const{
		return mManager;
	}

	void setResourceState(ResourceState state){
		mState = state;
	}

	bool isResourceLoaded() const{
		return mState == eLOADED || mState == eREADY;
	}

	bool isResourceReady() const{
		return mState == eREADY;
	}

	virtual ~AResource(){
	}

	String getPath() const{
		std::stringstream ss;
		ss << mGroup->getAbsPath() << "/" << mName;
		return ss.str();
	}

	void setUri(const String& uri){
		mUri = uri;
	}

	ResourceLink<T> getLink(){
		return ResourceLink<T>(this, getPath());
	}

	AResource() : mName(""), mHandle(0), mGroup(NULL), mUri(""), mState(eUNLOADED), mManager(NULL){
	}

	AResource(AResourceManager<T>* manager, ResourceHandle handle,
			const String& name) : mName(name), mHandle(handle), mState(eUNLOADED), mManager(manager){
	}

	ResourceState getResourceState() const{
		return mState;
	}

	void setGroup(AResourceGroup<T>* group){
		mGroup = group;
	}

	AResourceGroup<T>* getGroup(){
		return mGroup;
	}

	ResourceHandle getHandle() const{
		return mHandle;
	}

	const String& getName() const{
		return mName;
	}

	virtual void destroy(){
	}

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst){
		dst.Set("type", "RESOURCE");
		if(!mUri.empty()){
			dst.Set("uri", mUri.c_str());
		}
	}

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
		if(table.Get("uri").IsString()){
			mUri = table.Get("uri").ToString();
		}
	}

}; // </AResource>


}; // </wt>

#endif
