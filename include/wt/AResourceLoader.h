#ifndef WT_ARESOURCELOADER_H
#define WT_ARESOURCELOADER_H


#include "wt/stdafx.h"



namespace wt{

template<class T>
class AResourceLoader{
public:
	virtual void load(const String& path, T* dst){
	}

	virtual void create(T* resource){
	}

	virtual void save(const String& path, T* src){
	}

}; // <AResourceLoader>

}; // </wt>

#endif
