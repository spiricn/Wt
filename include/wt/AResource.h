#ifndef WT_ARESOURCE_H
#define WT_ARESOURCE_H


#include "wt/stdafx.h"

#include "wt/Utils.h"
#include "wt/ASerializable.h"
#include "wt/IResource.h"

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
	AResource();

	AResource(AResourceManager<T>* manager, ResourceHandle handle,
			const String& name);

	virtual ~AResource();

	const String& getUri() const;

	AResourceManager<T>* getManager() const;

	void setResourceState(ResourceState state);

	bool isResourceLoaded() const;

	bool isResourceReady() const;

	String getPath() const;

	void setUri(const String& uri);

	ResourceState getResourceState() const;

	void setGroup(AResourceGroup<T>* group);

	AResourceGroup<T>* getGroup();

	ResourceHandle getHandle() const;

	const String& getName() const;

	virtual void destroy();

	virtual void serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const;

	virtual void deserialize(lua::State* luaState, const LuaPlus::LuaObject& table);

	IResource::ResourceType getResourceType();

	void setResourceType(IResource::ResourceType type){
		mType = type;
	}

private:
	AResource(const AResource& other);

private:
	String mName;
	String mUri;
	ResourceHandle mHandle;
	AResourceGroup<T>* mGroup;
	ResourceState mState;
	AResourceManager<T>* mManager;
	IResource::ResourceType mType;
}; // </AResource>

template<class T>
AResource<T>::AResource(const AResource& other){
}

template<class T>
const String& AResource<T>::getUri() const{
	return mUri;
}
template<class T>
AResourceManager<T>* AResource<T>::getManager() const{
	return mManager;
}

template<class T>
void AResource<T>::setResourceState(ResourceState state){
	mState = state;
}

template<class T>
bool AResource<T>::isResourceLoaded() const{
	return mState == eSTATE_LOADED || mState == eSTATE_READY;
}

template<class T>
bool AResource<T>::isResourceReady() const{
	return mState == eSTATE_READY;
}

template<class T>
AResource<T>::~AResource(){
}

template<class T>
String AResource<T>::getPath() const{
	std::stringstream ss;
	ss << mGroup->getAbsPath() << "/" << mName;
	return ss.str();
}

template<class T>
void AResource<T>::setUri(const String& uri){
	mUri = uri;
}

template<class T>
AResource<T>::AResource() : mName(""), mHandle(0), mGroup(NULL), mUri(""), mState(eSTATE_UNLOADED), mManager(NULL){
}

template<class T>
AResource<T>::AResource(AResourceManager<T>* manager, ResourceHandle handle,
		const String& name) : mName(name), mHandle(handle), mState(eSTATE_UNLOADED), mManager(manager){
}

template<class T>
typename AResource<T>::ResourceState AResource<T>::getResourceState() const{
	return mState;
}

template<class T>
void AResource<T>::setGroup(AResourceGroup<T>* group){
	mGroup = group;
}

template<class T>
AResourceGroup<T>* AResource<T>::getGroup(){
	return mGroup;
}

template<class T>
ResourceHandle AResource<T>::getHandle() const{
	return mHandle;
}

template<class T>
const String& AResource<T>::getName() const{
	return mName;
}

template<class T>
void AResource<T>::destroy(){
}

template<class T>
void AResource<T>::serialize(lua::State* luaState, LuaPlus::LuaObject& dst) const{
	dst.Set("type", "RESOURCE");
	if(!mUri.empty()){
		dst.Set("uri", mUri.c_str());
	}
}

template<class T>
void AResource<T>::deserialize(lua::State* luaState, const LuaPlus::LuaObject& table){
	if(table.Get("uri").IsString()){
		mUri = table.Get("uri").ToString();
	}
}

template<class T>
IResource::ResourceType AResource<T>::getResourceType(){
	return mType;
}

} // </wt>

#endif // </WT_ARESOURCE_H>
