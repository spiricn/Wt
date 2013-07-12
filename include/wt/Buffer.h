#ifndef WT_BUFFER_H
#define WT_BUFFER_H


#include "wt/stdafx.h"

#include "wt/Exception.h"

namespace wt{

template<class T>
class Buffer{
private:
	T* mData;
	Uint32 mCapacity, mPosPut, mPosGet;

public:
	Buffer() : mData(NULL), mCapacity(0), mPosPut(0), mPosGet(0){
	}

	Buffer(Uint32 capacity){
		mData=NULL;
		mCapacity=0;
		mPosPut=0;
		mPosGet=0;
		create(capacity);
	}

	void copy(Buffer& dst) const{
		dst.create(mCapacity);
		dst.put(mData, mCapacity);
		dst.seekp(0);
	}

	void create(Uint32 capacity){
		destroy();

		mCapacity = capacity;
		mPosPut=0;
		mPosGet=0;
		mData = new T[mCapacity];
	}

	void clearMem(unsigned char val=0x00){
		memset(mData, val, getSize());
	}

	void resize(Uint32 newCapacity){
		T* newData = new T[newCapacity];

		memcpy(newData, mData, min(mCapacity, newCapacity)*sizeof(T));
		mCapacity = newCapacity;

		T* tmp = mData;
		mData = newData;

		if(newCapacity < mCapacity){
			if(mPosGet >= mCapacity){
				mPosGet = mCapacity-1;
			}

			if(mPosPut >= mCapacity){
				mPosPut = mCapacity-1;
			}
		}

		delete[] tmp;
	}

	/** returns the maximum number of elements of type T this buffer is able to contain */
	Uint32 getCapacity() const{
		return mCapacity;
	}

	/** returns the total buffer size in BYTES */
	Uint32 getSize() const{
		return mCapacity*sizeof(T);
	}

	Uint32 tellg() const{
		return mPosGet;
	}

	Uint32 tellp() const{
		return mPosPut;
	}

	void seekp(Uint32 pos){
		#ifdef WT_CHECKED
		if(pos >= mCapacity){
			WT_THROW("Seek position out of bounds (bounds=%d, pos=%d)",
				mCapacity, pos);
		}
		#endif

		mPosPut = pos;
	}

	void seekg(Uint32 pos){
		#ifdef WT_CHECKED
		if(pos >= mCapacity){
			WT_THROW("Seek position out of bounds (bounds=%d, pos=%d)",
				mCapacity, pos);
		}
		#endif

		mPosGet = pos;
	}

	void destroy(){
		if(mData != NULL){
			delete[] mData;
			mData=NULL;
		}

		mCapacity=0;
		mPosGet=0;
		mPosPut=0;
	}

	T* getData(){
		return mData;
	}

	const T* getData() const{
		return mData;
	}

	void put(const T* t, Uint32 count){
		#ifdef WT_CHECKED
		if(mPosPut+count > mCapacity){
			WT_THROW("Put failed, buffer full (size %d pos %d count %d)", mCapacity, mPosPut, count);
		}
		#endif

		memcpy(mData+mPosPut, t, count*sizeof(T));
		mPosPut += count;
	}

	T& get(){
		#ifdef WT_CHECKED
		if(mPosGet >= mCapacity){
			WT_THROW("Get failed, position out of bounds (pos=%d bounds=%d)",
				mPosGet, mCapacity);
		}
		#endif

		return mData[mPosGet++];
	}

	const T& get() const{
		return (const T&)get();
	}

	void put(const T& t){
		#ifdef WT_CHECKED
		if(mPosPut>=mCapacity){
			WT_THROW("Put failed, buffer full (size %d)", mCapacity);
		}
		#endif

		mData[mPosPut++] = T(t);
	}

	T& operator[](Uint32 index){
		#ifdef WT_CHECKED
		if(index >= mCapacity){
			WT_THROW("Attempted to index buffer (size %d) index %d", mCapacity, index);
		}
		#endif

		return mData[index];
	}

	const T& operator[](Uint32 index) const{
		#ifdef WT_CHECKED
		if(index >= mCapacity){
			WT_THROW("Attempted to index buffer (size %d) index %d",
				mCapacity, index);
		}
		#endif

		return mData[index];
	}

	~Buffer(){
		destroy();
	}

}; // </Buffer>

typedef Buffer<float> FloatBuffer;

typedef Buffer<Uint8> ByteBuffer;

}; // </wt>

#endif
