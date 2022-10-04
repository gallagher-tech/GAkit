#include "timer.h"

namespace ga {

Timer::Timer()
{
}

// -------- setters

void Timer::setStart( const ga::TimePoint &timePoint )
{
	mBegin_t = timePoint;
}

void Timer::setEnd( const ga::TimePoint &timePoint )
{
	mEnd_t = timePoint;
}

void Timer::set( const ga::TimePoint &start, const ga::TimePoint &end )
{
	setStart( start );
	setEnd( end );
}

void Timer::clear()
{
	mBegin_t = TimePoint();
	mEnd_t   = TimePoint();
}

void Timer::startNow()
{
	mBegin_t = Clock::now();
}

void Timer::startNow( long durationMillis )
{
	startNow();
	setEnd( mBegin_t + Millis( durationMillis ) );
}

// -------- getters

double Timer::duration()
{
	return static_cast<double>( durMillis( mBegin_t, mEnd_t ) );
}

double Timer::elapsedMillis()
{
	return static_cast<double>( durMillis( mBegin_t, Clock::now() ) );
}

double Timer::elapsedSeconds()
{
	return elapsedMillis() * .001;
}

double Timer::elapsedPercent()
{
	auto dur = duration();
	if ( dur == 0. )
		return 0.;
	else
		return elapsedMillis() / dur;
}

}  // namespace ga
