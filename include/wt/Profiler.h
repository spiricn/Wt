#ifndef WT_PROFILER_H
#define WT_PROFILER_H

#include "wt/Timer.h"
#include "wt/Log.h"

namespace wt{

class Profiler{
private:
	typedef std::map<String, float> TimeMap;
	Timer mTimer;
	String mMonitor;
	TimeMap mTimeMap;
public:

	void start(const String& name){
		mMonitor = name;
		mTimer.reset();
	}

	void stop(){
		float result = mTimer.getSeconds();
		mTimeMap[mMonitor]  = result;
	}

	void dump(){
		float totalTime = 0.0f;
		for(TimeMap::iterator i=mTimeMap.begin(); i!=mTimeMap.end(); i++){
			totalTime += i->second;
		}

		LOGI("Profiler", "--------------------------------");
		for(TimeMap::iterator i=mTimeMap.begin(); i!=mTimeMap.end(); i++){
			LOGI("Profiler", "\"%s\" t=%dms p=%.2f%%",
				i->first.c_str(), (Uint32)(i->second*1000.0f), (i->second/totalTime)*100.0f);
		}
		LOGI("Profiler", "--------------------------------");
	}

}; // </Profiler>

} // </wt>

#endif // </WT_PROFILER_H>