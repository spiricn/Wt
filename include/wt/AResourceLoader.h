#ifndef WT_ARESOURCELOADER_H
#define WT_ARESOURCELOADER_H


#include "wt/stdafx.h"
#include "wt/AIOStream.h"
#include "wt/FileIOStream.h"
#include "wt/Sp.h"

namespace wt{


class AResourceProvider{
public:
	virtual Sp<AIOStream> openStream(const String& uri, AIOStream::Mode mode) = 0;
};

class FileResourceProvider{
public:
	Sp<AIOStream> openStream(const String& uri, AIOStream::Mode mode){
		return new FileIOStream(uri, mode);
	}
};

template<class T>
class AResourceLoader{
public:
	virtual void load(AIOStream* stream, T* dst) = 0;

	virtual void save(AIOStream* stream, T* src) = 0;

	virtual void create(T* resource){
	}

	

}; // <AResourceLoader>

}; // </wt>

#endif
