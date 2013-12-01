#ifndef WT_USERDATAHOLDER_H
#define WT_USERDATAHOLDER_H

namespace wt
{

class UserDataHolder{
public:
	UserDataHolder();

	void* getUserData() const;

	void setUserData(void* data);

	bool isUserDataSet() const;

private:
	void* mUserData;
	bool mUserDataSet;
}; // </UserDataHolder>

} // </wt>

#endif // </WT_USERDATAHOLDER_H>
