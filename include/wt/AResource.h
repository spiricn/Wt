#ifndef WT_ARESOURCE_H
#define WT_ARESOURCE_H


#include "wt/stdafx.h"

#include "wt/Utils.h"
#include "wt/ASerializable.h"

namespace wt
{

template<typename T>
class AResourceGroup;

template<typename T>
class AResourceManager;

typedef unsigned long ResourceHandle;

template<class T>
class AResource;

class AResourceSystem;

template<class T>
class AResource : public lua::ASerializable{
public:
	enum ResourceState{
		eSTATE_UNLOADED,
		eSTATE_LOADED,
		eSTATE_READY
	}; // </ResourceState>

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
		return mState == eSTATE_LOADED || mState == eSTATE_READY;
	}

	bool isResourceReady() const{
		return mState == eSTATE_READY;
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

	AResource() : mName(""), mHandle(0), mGroup(NULL), mUri(""), mState(eSTATE_UNLOADED), mManager(NULL){
	}

	AResource(AResourceManager<T>* manager, ResourceHandle handle,
			const String& name) : mName(name), mHandle(handle), mState(eSTATE_UNLOADED), mManager(manager){
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

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
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

private:
	AResource(const AResource& other){
	}

private:
	String mName;
	String mUri;
	ResourceHandle mHandle;
	AResourceGroup<T>* mGroup;
	ResourceState mState;
	AResourceManager<T>* mManager;

}; // </AResource>


} // </wt>

#endif // </WT_ARESOURCE_H>
