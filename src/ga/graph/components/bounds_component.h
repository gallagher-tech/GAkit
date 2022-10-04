#pragma once
#include "ga/graph/component.h"
#include "ga/math.h"
#include <vector>

namespace ga {

// 3D axis aligned bounding box
class Bounds : public Component
{
public:
	vec3 min = vec3( 0.f );
	vec3 max = vec3( 0.f );

	Bounds() {}
	Bounds( vec2 min_, vec2 max_ )
	    : min( min_, 0. )
	    , max( max_, 0. )
	{
	}
	Bounds( vec3 min_, vec3 max_ )
	    : min( min_ )
	    , max( max_ )
	{
	}
	Bounds( const Rect& rect )
	    : min( rect.min(), 0 )
	    , max( rect.max(), 0 )
	{
	}
	Bounds( const std::vector<vec3>& points )
	{
		min = vec3( std::numeric_limits<float>::max() );
		max = vec3( std::numeric_limits<float>::min() );
		for ( auto& pt : points ) {
			include( pt );
		}
	}

	inline Bounds& include( vec3 pt )
	{
		for ( int i = 0; i < 3; ++i ) {
			min[i] = std::min( pt[i], min[i] );
			max[i] = std::max( pt[i], max[i] );
		}
		return *this;
	}

	inline Bounds& translate( const vec3& delta )
	{
		min += delta;
		max += delta;
	}

	inline vec3 center() const
	{
		return min + ( max - min ) * .5f;
	}

	inline vec3 size() const
	{
		return max - min;
	}

	bool contains( const vec3& pos ) const
	{
		return glm::all( glm::lessThanEqual( min, pos ) ) && glm::all( glm::greaterThanEqual( max, pos ) );
	}
};
}  // namespace ga