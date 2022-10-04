#pragma once
#include "ga/defines.h"
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

namespace ga {

template <class ResourceT, class... Args>
bool loadResource( ResourceT& r, Args... args )
{
	return load( r, args... );
}

/**
	 * @brief resource cache class, represents a container for a resource type
	 * organized by name (string)
	 *
	 * @tparam ResourceT - any class where the following function 
	 *   is defined within the class's namespace:
	 * 
	 *     template <class ...Args>
	 *     bool load( ResourceT& resource, Args... args );
	 *
	 */
template <class ResourceT>
class ResourceCache
{
public:
	// retrieve a Resource from the Cache by name, return nullptr if not found
	std::shared_ptr<ResourceT> get( const std::string& name )
	{
		auto it = cache.find( name );
		return it != cache.end() ? it->second : nullptr;
	}

	// load a Resource into the Cache from disk, return nullptr on failure
	template <class... Args>
	std::shared_ptr<ResourceT> load( const std::string& name, Args... args )
	{
		auto rPtr = std::make_shared<ResourceT>();
		if ( ga::loadResource( *rPtr.get(), args... ) ) {
			cache[name] = std::move( rPtr );
			return cache[name];
		}
		return nullptr;
	}

	//// add a loaded Resource to the cache (using move semantics)
	//bool add( ResourceT&& resource, bool overwrite = true )
	//{
	//	if ( !overwrite && cache.find( resource.name ) != cache.end() ) {
	//		return false;
	//	}
	//	cache[name] = std::move( resource );
	//	return true;
	//}

	// get the names / keys of the cache
	std::vector<std::string> getNames()
	{
		std::vector<std::string> names;
		names.reserve( cache.size() );
		for ( auto& el : cache ) {
			names.push_back( el.first );
		}
		return names;
	}

	std::unordered_map<std::string, std::shared_ptr<ResourceT>> cache;
};

}  // namespace ga