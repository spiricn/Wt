#ifndef WT_THREAD_H
#define WT_THREAD_H

namespace wt{


class Thread{
private:
	bool mRunning, mFinished;

public:
	Thread();

	virtual ~Thread();

	bool isRunning() const;

	bool isFinished() const;

	void start();

	static void sleep(float sec);

	bool wait(float sec=-1.0f);

	virtual void run() = 0;

#if defined(WIN32)
private:
	DWORD mWin32Id;
	HANDLE mWin32Handle;

	static DWORD WINAPI startThread(LPVOID param);
#endif

}; // </Thread>


}; // </wt>

#endif // </WT_THREAD_H>