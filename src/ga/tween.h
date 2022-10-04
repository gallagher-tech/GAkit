#pragma once
#include "ga/easing.h"
#include "ga/math.h"
#include "ga/signal.h"
#include "ga/timer.h"

namespace ga {

class TweenBase
{
public:
	friend class Timeline;
	virtual ~TweenBase() = default;
	void setName( const std::string& name ) { m_name = name; }
	const std::string& getName() { return m_name; }

protected:
	std::string m_name                    = "";
	virtual bool update_()                = 0;
	virtual bool isDone_()                = 0;
	virtual bool isStarted_()             = 0;
	virtual void end( bool fireCallback ) = 0;
};

// Tween class
template <typename T>
class Tween : public TweenBase, public Timer
{
public:
	Tween( const T& startVal, const T& endVal, std::function<float( float )> easeFn )
	    : Timer()
	    , m_startVal( startVal )
	    , m_endVal( endVal )
	    , m_easeFn( easeFn )
	    , m_onDone( nullptr )
	    , m_boundPtr( nullptr )
	{
	}
	Tween( const T& startVal, const T& endVal, std::function<float( float )> easeFn, std::function<void()> onDone )
	    : Timer()
	    , m_startVal( startVal )
	    , m_endVal( endVal )
	    , m_easeFn( easeFn )
	    , m_onDone( onDone )
	    , m_boundPtr( nullptr )
	{
	}
	Tween() {};

	friend class Timeline;

	Tween& set( const T& startVal, const T& endVal, std::function<float( float )> easeFn, std::function<void()> onDone = nullptr )
	{
		return setStartVal( startVal ).setEndVal( endVal ).setEaseFn( easeFn ).setOnDone( onDone );
	}

	Tween& setStartVal( const T& startVal )
	{
		m_startVal = startVal;
		return *this;
	}
	Tween& setEndVal( const T& endVal )
	{
		m_endVal = endVal;
		return *this;
	}
	Tween& setEaseFn( ga::EaseType easeFnType )
	{
		m_easeFnType = easeFnType;
		m_easeFn     = ga::easeFn( easeFnType );
		return *this;
	}
	Tween& setEaseFn( std::function<float( float )> easeFn )
	{
		m_easeFnType = ga::EaseType::CUSTOM;
		m_easeFn     = easeFn;
		return *this;
	}
	// assign callback on done
	Tween& setOnDone( std::function<void()> onDone )
	{
		m_onDone = onDone;
		return *this;
	}

	// bind a ptr to update with the animation value
	Tween& bind( T* ptr )
	{
		m_boundPtr = ptr;
		return *this;
	}
	Tween& unbind()
	{
		m_boundPtr = nullptr;
		return *this;
	}

	const T& getStartVal() const { return m_startVal; }
	const T& getEndVal() const { return m_endVal; }

	const T& getValue() const { return m_val; }

	// ends at preset end time
	virtual void startNow() override
	{
		m_val = m_startVal;
		Timer::startNow();
	}

	// recalculates end time from now
	virtual void startNow( long durationMillis ) override
	{
		m_val = m_startVal;
		Timer::startNow( durationMillis );
	}

	void startAfterDelay( long delayMs, long durationMs )
	{
		auto start = ga::Clock::now() + ga::Millis( delayMs );
		Timer::setStart( start );
		Timer::setEnd( start + ga::Millis( durationMs ) );
	}

	// update and return current value (call at frame rate when using bound ptr)
	const T& update()
	{
		if ( Timer::isSet() && Timer::isActive() ) {  // current time is between begin and end
			m_val = ga::interpolate( m_startVal, m_endVal, Timer::elapsedPercent(), m_easeFn, true );
			updateBoundPtr();
		}
		if ( Timer::isDone() ) {
			end();
		}
		return m_val;
	}

	// end animation now (optionally firing callback), return end value
	const T& endNow( bool fireCallback = true )
	{
		end( fireCallback );
		return m_val;
	}

protected:
	T m_val, m_startVal, m_endVal;           // val tweens from startVal to endVal
	std::function<float( float )> m_easeFn;  // easing function: p = f(t) - see Ease.h
	ga::EaseType m_easeFnType;
	std::function<void()> m_onDone;  // callback
	T* m_boundPtr;
	bool updateBoundPtr()
	{
		auto ptr = m_boundPtr;
		if ( ptr ) {
			try {
				*ptr = m_val;
			} catch ( std::exception& e ) {
				// todo: log exception
				m_boundPtr = nullptr;  // clear pointer since it's broken
			}
		}
		return ptr != nullptr;
	}

	void end( bool fireCallback = true ) override
	{
		m_val = m_endVal;
		updateBoundPtr();
		Timer::clear();
		// fire on-done signal
		if ( fireCallback && m_onDone ) {
			// callback
			try {
				m_onDone();
			} catch ( std::exception& e ) {
				// todo: log exception
				m_onDone = nullptr;  // clear callback since it's broken
			}
		}
	}

	bool update_() override
	{
		// Timer::update()
		if ( Timer::isSet() ) {
			if ( Timer::isActive() ) {  // current time is between begin and end
				m_val = ga::interpolate( m_startVal, m_endVal, Timer::elapsedPercent(), m_easeFn, true );
			} else if ( Timer::isDone() ) {
				m_val = m_endVal;
			}
			updateBoundPtr();
		}
		return isSet() && !isDone();
	}

	bool isDone_() override
	{
		return Timer::isDone();
	}

	bool isStarted_() override
	{
		return Timer::isStarted();
	}
};

}  // namespace ga
