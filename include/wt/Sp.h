#ifndef WT_SP_H
#define WT_SP_H

#include "Wt/Defines.h"
#include "Wt/Exception.h"

namespace wt{

template<class T>
class Sp{
private:
	wt::Uint32* mRefCount;
	T* mRef;

	void incRefCount(){
		if(mRefCount){
			if(*mRefCount){
				(*mRefCount)++;
			}
		}
	}

	void decRefCount(){
		if(mRefCount){
			(*mRefCount)--;
			if(*mRefCount == 0){
				delete mRef;
				delete mRefCount;
				mRef = NULL;
				mRefCount = NULL;
			}
		}
	}

public:
	Sp(T* ptr) : mRef(ptr), mRefCount(new wt::Uint32){
		*mRefCount = 1;
	}

	Sp() : mRefCount(0), mRef(NULL){
	}

	Sp(const Sp& other) : mRefCount(0), mRef(NULL){
		*this = other;
	}

	~Sp(){
		decRefCount();
	}

	Uint32 getRefCount() const{
		return mRefCount?*mRefCount:0;
	}

	Sp& operator=(const Sp& other){
		decRefCount();

		mRefCount = other.mRefCount;
		mRef = other.mRef;

		incRefCount();

		return *this;
	}

	operator bool() const{
		return mRef != NULL;
	}

	/* dereferencing operators */
	T* operator->(){
#ifdef WT_CHECKED
		WT_ASSERT(mRef != NULL, "Null pointer dereferencing");
#endif
		return mRef;
	}

	T& operator*(){
#ifdef WT_CHECKED
		WT_ASSERT(mRef != NULL, "Null pointer dereferencing");
#endif
		return *mRef;
	}

	const T* operator->() const{
#ifdef WT_CHECKED
		WT_ASSERT(mRef != NULL, "Null pointer dereferencing");
#endif
		return mRef;
	}

	const T& operator*() const{
#ifdef WT_CHECKED
		WT_ASSERT(mRef != NULL, "Null pointer dereferencing");
#endif
		return *mRef;
	}

	/* cast operators */
	operator T*(){
		return mRef;
	}

	operator const T*() const{
		return mRef;
	}


	T* get(){
		return mRef;
	}

	const T* get() const{
		return mRef;
	}
}; // </Sp>


}; // </Wt>

#endif