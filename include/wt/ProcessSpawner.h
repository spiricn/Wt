#ifndef WT_PROCESSSPAWNER_H
#define WT_PROCESSSPAWNER_H

#include "wt/AProcess.h"

namespace wt
{

class ProcessSpawner : public AProcess{
public:
	ProcessSpawner(uint32_t numProcesses, ...);

	~ProcessSpawner();

	void onProcStart();

	void onProcUpdate(float dt);

	void onProcEnd();

private:
	typedef std::vector<ProcPtr> ProcessList;

	ProcessList mProcesses;

}; // </ProcessSpawner>

} // </wt>

#endif // </WT_PROCESSSPAWNER_H>
