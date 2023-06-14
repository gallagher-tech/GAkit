#pragma once
#include "json.hpp"  // nlohmann::json

namespace ga {
using Json = nlohmann::json;

namespace json {
	// json parser utility functions

	template <typename T>
	inline T getValue( const ga::Json& json, const std::string& keyOrPointer, const T& defaultValue )
	{
		T ret = defaultValue;
		try {
			if ( keyOrPointer.empty() || keyOrPointer.at( 0 ) == '/' ) {
				// allow for json pointer paths - these are either empty or begin with '/'
				ret = json.at(ga::Json::json_pointer( keyOrPointer )).get<T>();
			} else {
				// assume key is a normal key for a json object
				ret = json.at(keyOrPointer).get<T>();
			}
		} catch ( ... ) {
			// todo: log warning? accept an ostream& for logging?
		}
		return ret;
	}

	inline std::string getValue( const ga::Json& json, const std::string& keyOrPointer, const char* defaultValue )
	{
		return getValue( json, keyOrPointer, std::string( defaultValue ) );
	}

}  // namespace json
}  // namespace ga