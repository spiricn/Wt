#ifndef WT_ARESOURCEMANAGER_H
#define WT_ARESOURCEMANAGER_H


#include "wt/stdafx.h"

#include "wt/AResourceGroup.h"

#include "wt/Exception.h"
#include "wt/AResourceLoader.h"
#include "wt/AResourceSystem.h"

namespace wt{

class AResourceSystem;

class IResourceManager{
public:
};

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

	AResourceManager(AResourceSystem* assets) : mHandleCount(0), mLoader(NULL), mResourceSystem(assets){
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


	T* load(AIOStream* stream, T* dst){
		mLoader->load(stream, dst);

		dst->setResourceState(AResource<T>::eLOADED);

		return dst;
	}

	void save(AIOStream* stream, T* src){
		WT_ASSERT(mLoader != NULL, "No resoruce loader set, cannot save \"%s\"", path.c_str());

		mLoader->save(stream, src);
	}

	void save(const String& path, T* src){
		WT_ASSERT(mLoader != NULL, "No resoruce loader set, cannot save \"%s\"", path.c_str());

		mLoader->save(path, src);
	}

	void load(const String& path, T* src){
		WT_ASSERT(mLoader != NULL, "No resoruce loader set, cannot load \"%s\"", path.c_str());

		mLoader->load(path, src);
	}

	void loadAll(){
		for(AResourceManager<T>::ResourceMap::iterator i=AResourceManager<T>::mResources.begin();
			i!=AResourceManager<T>::mResources.end(); i++){
				if(!i->second->isResourceLoaded() && mLoader != NULL){
					if(i->second->getUri().size()){
						Sp<AIOStream> stream = mResourceSystem->getFileSystem()->open(i->second->getUri(), AIOStream::eMODE_READ);

						try{
							load(stream, i->second);
						}catch(...){
							LOGE("Error loading resource from uri \"%s\"", i->second->getUri().c_str());
						}
					}
				}
		}
	}

	AResourceLoader<T>* getLoader() const{
		return mLoader;
	}

	void createAll(){
		for(AResourceManager<T>::ResourceMap::iterator i=AResourceManager<T>::mResources.begin();
			i!=AResourceManager<T>::mResources.end(); i++){
				if(!i->second->isResourceReady() && mLoader != NULL){
					mLoader->create(i->second);
					i->second->setResourceState(AResource<T>::eREADY);
				}
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

private:
	ResourceHandle mHandleCount;
	typedef std::map<ResourceHandle, T*> ResourceMap;
	AResourceSystem* mResourceSystem;
};


}; // </wt>

#endif
