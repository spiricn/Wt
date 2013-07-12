#ifndef WT_MUTEX_H
#define WT_MUTEX_H

namespace wt{

class Mutex{
public:
	Mutex();

	~Mutex();

	void lock();

	bool tryLock(float timeout);

	void unlock();

#if defined(WIN32)
private:
	CRITICAL_SECTION mWin32Handle;

public:
	CRITICAL_SECTION& getWin32Handle(){
		return mWin32Handle;
	}
#endif // </WIN32>

}; // </Mutex>

}; // </wt>

#endif // </WT_MUTEX_H>