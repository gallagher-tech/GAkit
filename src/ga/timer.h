#pragma once
#include <chrono>
#include <functional>
#include <iostream>
#include <string>

namespace ga {

// ---------------------
// Time typedefs and utils
// ---------------------

using Clock     = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Millis    = std::chrono::milliseconds;

// get millis since epoch for a point in time
inline long timeMillis( const TimePoint& timePoint )
{
	return ( long )std::chrono::duration_cast<std::chrono::milliseconds>( timePoint.time_since_epoch() ).count();
}

// get current millis since epoch
inline long timeMillis()
{
	return ( long )std::chrono::duration_cast<std::chrono::milliseconds>( Clock::now().time_since_epoch() ).count();
}

// get duration in millis between two points in time
inline long durMillis( const TimePoint& begin, const TimePoint& end )
{
	return ( long )std::chrono::duration_cast<std::chrono::milliseconds>( end - begin ).count();
}

// ostream operator for a point in time, as millis since epoch
inline std::ostream& operator<<( std::ostream& stream, const TimePoint& timePoint )
{
	return stream << timeMillis( timePoint );
}

// ---------------------
// Timer class
// ---------------------

class Timer
{
public:
	Timer();

	// -------- setters

	virtual void setStart( const TimePoint& time );
	virtual void setEnd( const TimePoint& time );

	virtual void set( const TimePoint& start, const TimePoint& end );
	virtual void clear();

	virtual void startNow();                       // ends at set end time point
	virtual void startNow( long durationMillis );  // ends at calc'd end point from now

	// -------- getters

	inline virtual const TimePoint& getStart() const { return mBegin_t; }
	inline virtual const TimePoint& getEnd() const { return mEnd_t; }

	inline virtual bool hasStart() { return mBegin_t != TimePoint(); }
	inline virtual bool hasEnd() { return mEnd_t != TimePoint(); }
	inline virtual bool isSet() { return hasStart() && hasEnd(); }
	inline virtual bool isStarted() { return hasStart() && Clock::now() >= mBegin_t; }
	inline virtual bool isDone() { return hasEnd() && Clock::now() >= mEnd_t; }
	inline virtual bool isActive() { return hasStart() && Clock::now() >= mBegin_t && ( hasEnd() ? Clock::now() < mEnd_t : true ); }

	double duration();
	double elapsedMillis();
	double elapsedSeconds();
	double elapsedPercent();

protected:
	TimePoint mBegin_t, mEnd_t;
};

}  // namespace ga
