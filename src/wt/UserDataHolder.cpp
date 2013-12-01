#include "wt/stdafx.h"

#include "wt/UserDataHolder.h"

namespace wt
{

UserDataHolder::UserDataHolder() : mUserDataSet(false), mUserData(NULL){
}

void* UserDataHolder::getUserData() const{
	return mUserData;
}

void UserDataHolder::setUserData(void* data){
	mUserData = data;
	mUserDataSet = true;
}

bool UserDataHolder::isUserDataSet() const{
	return mUserDataSet;
}

} // </wt>
