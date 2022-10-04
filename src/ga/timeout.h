/**
 * @file timeout.h
 * @author Tyler Henry, Gallagher & Associates
 * @brief 
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Timer.h"

namespace ga {

//
//	Timeout represents a function callback
//	with an attached timer, for scheduling execution
//

struct Timeout
{

	Timer timer;
	std::function<void( void )> callback;
	std::string name;
	void clear()
	{
		timer.clear();
		callback = nullptr;
		name     = "";
	}
};

//
// TimeoutManager
// owns a set of function callbacks that fire after a specified wait
//

class TimeoutManager
{

public:
	// -----------------------------------------------------------------------------------
	// register a Timeout callback to be called 'delayMs' milliseconds from now
	// returns the name of the Timeout (generated from the function pointer if left blank)
	// -----------------------------------------------------------------------------------
	inline std::string setTimeout( long long delayMs, std::function<void( void )> callback, std::string name = "" )
	{
		if ( !callback ) return "";

		m_timeouts.push_back( Timeout() );
		m_timeouts.back().callback = callback;
		m_timeouts.back().timer.startNow( delayMs );

		if ( name.empty() ) {
			// use the callback pointer address as the name, if name is empty
			// get underlying address of function ptr - https://stackoverflow.com/a/44236212/5195277
			std::stringstream ss;
			ss << *( size_t* )( char* )&m_timeouts.back().callback;
			name = ss.str();
		}

		m_timeouts.back().name = name;

		// sort the timeouts chronologically (by end time)
		std::sort( m_timeouts.begin(), m_timeouts.end(),
		           []( Timeout& a, Timeout& b ) {
			           return a.timer.getEnd() < b.timer.getEnd();
		           } );
		return name;
	}

	// ------------------------
	// fire and remove timeouts
	// ------------------------
	inline void updateTimeouts()
	{

		m_timeouts.erase(
		    std::remove_if( m_timeouts.begin(), m_timeouts.end(), []( ga::Timeout& evt ) -> bool {
			    if ( evt.timer.isDone() ) {
				    if ( evt.callback ) {
					    try {
						    evt.callback();
						    // TODO: ga::log
						    // ga::log::verbose << "[" << evt.name << "] Timeout fired";
					    } catch ( std::exception& e ) {
						    // TODO: ga::log
						    std::cout << "ERROR [" << evt.name << "] Timeout callback exception:\n\t" << e.what() << std::endl;
					    }
				    }
				    return true;
			    }
			    return !evt.timer.isSet();  // prune unset events
		    } ),
		    m_timeouts.end() );
	}

	// ------------------------
	// cancel a timeout by name
	// ------------------------
	inline bool cancelTimeout( std::string name )
	{
		bool did = false;

		m_timeouts.erase(
		    std::remove_if( m_timeouts.begin(), m_timeouts.end(), [&]( Timeout& evt ) -> bool {
			    if ( evt.name == name ) {
				    evt.clear();
				    return did = true;
			    }
			    return false;
		    } ),
		    m_timeouts.end() );
		return did;
	}

	// ---------------------------------------
	// return the registered timeout callbacks
	// ---------------------------------------
	inline std::vector<Timeout>& getTimeouts()
	{
		return m_timeouts;
	}

protected:
	std::vector<Timeout> m_timeouts;
};

}  // namespace ga
