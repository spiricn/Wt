#ifndef WTE_ATOOL_H
#define WTE_ATOOL_H


class AToolManager;

class ATool{
friend class AToolManager;

private:
	AToolManager* mManager;
	bool mIsFocused;

protected:
public:
	ATool(AToolManager* manager) : mManager(manager), mIsFocused(false){
	}

	AToolManager* getToolManager() const{
		return mManager;
	}

	virtual void onToolLostFocus(){
		mIsFocused = false;
	}

	virtual void onToolGainFocus(){
		mIsFocused = true;
	}

	bool isToolFocused() const{
		return mIsFocused;
	}
};

class AToolManager{
public:
	virtual void requestFocus(ATool* tool) = 0;
	virtual void giveupFocus(ATool* tool) = 0;
};


#endif