#ifndef WT_TIMEACCUMULATOR_H
#define WT_TIMEACCUMULATOR_H

namespace wt{

class TimeAccumulator{
private:
	float mTimeStep;
	float mTime;

public:
	TimeAccumulator(float timeStep);

	void setStep(float step);

	float getStep() const;

	void setTime(float time);

	bool update(float dt);

}; // </TimeAccumulator>


}; // </wt>

#endif // </WT_TIMEACCUMULATOR_H>