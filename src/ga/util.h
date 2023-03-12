#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>
#include <sstream>
#include <vector>

namespace ga {

// string utilities
// ----------------------

inline std::vector<std::string> splitString( const std::string& str, char delim = ' ' )
{
	std::vector<std::string> result;
	std::stringstream ss( str );
	std::string token;
	while ( std::getline( ss, token, delim ) ) {
		result.push_back( token );
	}
	return result;
}

inline std::string toLower( std::string str )
{
	std::transform( str.begin(), str.end(), str.begin(), []( char c ) { return std::tolower( c, std::locale() ); } );
	return str;
}

inline std::string toUpper( std::string str )
{
	std::transform( str.begin(), str.end(), str.begin(), []( char c ) { return std::toupper( c, std::locale() ); } );

	return str;
}

// Helper RAII class to auto cleanup / do something on early return
struct scope_guard
{
	scope_guard( const std::function<void()>& f )
	    : _f( f )
	{
	}
	~scope_guard()
	{
		if ( _f )
			_f();
	}

protected:
	std::function<void()> _f;
};

}  // namespace ga
