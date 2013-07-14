#ifndef WT_ARESOURCEMANAGER_H
#define WT_ARESOURCEMANAGER_H


#include "wt/stdafx.h"

#include "wt/AResourceGroup.h"

#include "wt/Exception.h"
#include "wt/AResourceLoader.h"
#include "wt/AResourceSystem.h"

namespace wt{

class AResourceSystem;

template <class T>
class AResourceManager : public AResourceAllocator<T>, public AResourceGroup<T>{
private:
	ResourceHandle mHandleCount;
	typedef std::map<ResourceHandle, T*> ResourceMap;
	AResourceSystem* mResourceSystem;

public:

	class ResourceIterator{
	friend class AResourceManager;
	private:
		typename ResourceMap::iterator mIterator;
		typename ResourceMap::iterator mEnd;

	protected:
		ResourceIterator(typename ResourceMap::iterator start, typename ResourceMap::iterator end) : mIterator(start), mEnd(end){
		}

	public:

		bool next(){
			return mIterator != mEnd;
		}

		ResourceIterator operator++(int){
			mIterator++;

			return *this;
		}

		T* operator->(){
			return mIterator->second;
		}

		T* operator*(){
			return mIterator->second;
		}

	};

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
		T* rsrc = new T(handle, resourceName);
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
					// TODO acquire stream from elsewhere
					//FileIOStream stream(i->second->getUri(), AIOStream::eMODE_READ);
					Sp<AIOStream> stream = mResourceSystem->getFileSystem()->open(i->second->getUri(), AIOStream::eMODE_READ);

					try{
						load(stream, i->second);
					}catch(...){
						LOGE("Error loading resource from uri \"%s\"", i->second->getUri().c_str());
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

};


}; // </wt>

#endif
