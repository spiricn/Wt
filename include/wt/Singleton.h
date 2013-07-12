#ifndef WT_SINGLETON_H
#define WT_SINGLETON_H


#include "wt/stdafx.h"

namespace wt{


template<class T>
class Singleton{
private:
	static T* mInstance;
public:
	static T& getSingleton(){
		if(mInstance == NULL){
			mInstance = new T();
		}

		return *mInstance;
	}

	static void releaseSingleton(){
		if(mInstance != NULL){
			delete mInstance;
			mInstance = NULL;
		}
	}
}; // </Singleton>


template<class T>
T* Singleton<T>::mInstance = NULL;

}; // </wt>

#endif
