#ifndef WT_ARESOURCEMANAGER_H
#define WT_ARESOURCEMANAGER_H


#include "wt/stdafx.h"

#include "wt/AResourceGroup.h"

#include "wt/Exception.h"
#include "wt/AResourceLoader.h"
#include "wt/AResourceSystem.h"
#include "wt/IResourceManager.h"

namespace wt
{

class AResourceSystem;

template <class T>
class AResourceManager : public AResourceAllocator<T>, public AResourceGroup<T>, public IResourceManager{
public:

	class ResourceIterator{
	public:
		ResourceIterator() : mValid(false){
		}

		bool next(){
			WT_ASSERT(mValid, "Invalid iterator");

			return mIterator != mEnd;
		}

		ResourceIterator operator++(int){
			WT_ASSERT(mValid, "Invalid iterator");

			mIterator++;

			return *this;
		}

		T* operator->(){
			WT_ASSERT(mValid, "Invalid iterator");

			return mIterator->second;
		}

		T* operator*(){
			WT_ASSERT(mValid, "Invalid iterator");

			return mIterator->second;
		}

	private:
		bool mValid;
		typename ResourceMap::iterator mIterator;
		typename ResourceMap::iterator mEnd;

		friend class AResourceManager;

	protected:
		ResourceIterator(typename ResourceMap::iterator start, typename ResourceMap::iterator end) : mIterator(start), mEnd(end), mValid(true){
		}
	}; // </ResourceIterator>

protected:
	ResourceMap mResources;
	AResourceLoader<T>* mLoader;

	virtual void free(T* resource){
		mResources.erase(resource->getHandle());
		delete resource;
	}

	virtual T* allocate(AResourceGroup<T>* parent, const String& resourceName){
		// Generate a new handle
		ResourceHandle handle = newHandle();

		// Allcoate the resource & store it
		T* rsrc = new T(this, handle, resourceName);
		rsrc->setResourceType(mType);
		mResources[handle] = rsrc;
		rsrc->setGroup(parent);

		return rsrc;
	}

	ResourceHandle newHandle(){
		return mHandleCount++;
	}

public:

	AResourceSystem* getResourceSystem(){
		return mResourceSystem;
	}

	AResourceManager(AResourceSystem* assets, IResource::ResourceType type) : mHandleCount(0), mLoader(NULL), mResourceSystem(assets), mType(type){
		AResourceGroup<T>::setResourceAllocator(this);
		AResourceGroup<T>::setName("$ROOT");
		AResourceGroup<T>::setResourceSystem(mResourceSystem);
	}

	ResourceIterator getResourceIterator(){
		return ResourceIterator(
			mResources.begin(), mResources.end()
			);
	}

	~AResourceManager(){
		destroy();
	}

	void setLoader(AResourceLoader<T>* ptr){
		mLoader = ptr;
	}

	void load(T* src){
		load(src->getUri(), src);
	}

	void save(T* src){
		Sp<AIOStream> stream = mResourceSystem->getFileSystem()->open(src->getUri(), AIOStream::eMODE_WRITE);

		if(!stream->isOpen()){
			WT_THROW("Error openning resource file \"%s\"", src->getUri().c_str());
		}

		save(stream, src);
	}

	void loadSet(AResourceGroup<T>::ResourceSet& resources){
		for(AResourceGroup<T>::ResourceSet::iterator iter=resources.begin(); iter!=resources.end(); iter++){
			load(*iter);
		}
	}

	void createSet(AResourceGroup<T>::ResourceSet& resources){
		for(AResourceGroup<T>::ResourceSet::iterator iter=resources.begin(); iter!=resources.end(); iter++){
			createResource(*iter);
		}
	}

	T* load(AIOStream* stream, T* dst){
		mLoader->load(stream, dst);

		dst->setResourceState(AResource<T>::eSTATE_LOADED);

		return dst;
	}

	void save(AIOStream* stream, T* src){
		mLoader->save(stream, src);
	}

	void save(const String& path, T* src){
		WT_ASSERT(mLoader != NULL, "No resoruce loader set, cannot save \"%s\"", path.c_str());

		mLoader->save(path, src);
	}

	void load(const String& path, T* src){
		if(!src->isResourceLoaded() && mLoader != NULL){
			if(src->getUri().size()){
				try{
					Sp<AIOStream> stream = mResourceSystem->getFileSystem()->open(src->getUri(), AIOStream::eMODE_READ);
					WT_ASSERT(stream->isReadable(), "Error openning file stream \"%s\"", src->getUri().c_str());
					mLoader->load(stream, src);
				}catch(...){
					TRACEE("Error loading resource \"%s\"", src->getPath().c_str());
				}
			}
		}
	}

	void loadAll(){
		for(AResourceManager<T>::ResourceMap::iterator i=AResourceManager<T>::mResources.begin();
			i!=AResourceManager<T>::mResources.end(); i++){
				load(i->second);
		}
	}

	AResourceLoader<T>* getLoader() const{
		return mLoader;
	}

	void createResource(T* rsrc){
		if(!rsrc->isResourceReady() && mLoader != NULL){
			mLoader->create(rsrc);
		}

		rsrc->setResourceState(AResource<T>::eSTATE_READY);
	}

	void createAll(){
		for(AResourceManager<T>::ResourceMap::iterator i=AResourceManager<T>::mResources.begin();
			i!=AResourceManager<T>::mResources.end(); i++){
				createResource(i->second);
		}
	}

	uint32_t getNumResources() const{
		return mResources.size();
	}

	virtual void destroy(){
		AResourceGroup<T>::destroy();

		for(ResourceMap::iterator i=mResources.begin(); i!=mResources.end(); i++){
			delete i->second;
		}

		mResources.clear();
	}

	T* get(ResourceHandle handle){
		return mResources[handle];
	}

	IResource::ResourceType getResourceType(){
		return mType;
	}

private:
	ResourceHandle mHandleCount;
	typedef std::map<ResourceHandle, T*> ResourceMap;
	AResourceSystem* mResourceSystem;
	IResource::ResourceType mType;
};


}; // </wt>

#endif
