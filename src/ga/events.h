#pragma once
#include "ga/math.h"
#include "ga/timer.h"

namespace ga {

struct Event
{
	Event()
	    : time( ga::Clock::now() )
	    , captured( false )
	{
	}

	TimePoint time;  // time when event occured
	bool captured;   // flag to stop event propagation
};

struct KeyEvent : public Event
{
	enum class Type
	{
		PRESS,
		RELEASE
	};
	Type type;
	int key;
};

struct MouseEvent : public Event
{
	enum class Type
	{
		MOVE,
		PRESS,
		DRAG,
		RELEASE,
		SCROLL_UP,  // wheel
		SCROLL_DOWN
	};
	Type type;
	vec2 position;
	int button;  // LEFT = 0, RIGHT = 1, CENTER = 2, ...
};

struct TouchEvent : public Event
{
	enum class Type
	{
		PRESS,
		DRAG,
		RELEASE,
		CANCEL
	};

	Type type;
	ga::vec2 position;  // window space
	ga::vec2 size;
	float angle;     // degrees 0-359
	float pressure;  // 0-1024

	/*
	ofTouchEventArgs:
	-----------------
	int id;
	int time;
	int numTouches;
	float minoraxis, majoraxis;
	float xspeed, yspeed;
	float xaccel, yaccel;
	*/
};

}  // namespace ga