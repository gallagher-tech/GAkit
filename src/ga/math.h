/**
 * @file math.h
 * @author Tyler Henry, Gallagher & Associates
 * @brief 
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include "ga/defines.h"
#include "glm/glm.hpp"
#include "glm/gtx/matrix_decompose.hpp"  // needed for matrix decomposition
#include "glm/gtx/quaternion.hpp"
#include <algorithm>
#include <limits>

#ifdef GA_OPENFRAMEWORKS
#include "ofRectangle.h"
#endif

namespace ga {

namespace math {

	const double pi     = 4. * std::atan( 1. );  // tan(1/4 PI) = 1
	const double halfPi = pi * .5;

	const double radToDegFactor = 180. / math::pi;  // degrees = 180/pi * radians
	const double degToRadFactor = math::pi / 180.;  // radians = pi/180 * degrees

}  // namespace math

// pull into ga namespace:
using glm::mat2;
using glm::mat2x3;
using glm::mat3;
using glm::mat4;
using glm::quat;
using glm::vec2;
using glm::vec3;
using glm::vec4;

template <typename T>
inline T toDegrees( const T& radians )
{
	return math::radToDegFactor * radians;
}

inline double toDegrees( const double& radians )
{
	return math::radToDegFactor * radians;
}

template <typename T>
inline T toRadians( const T& degrees )
{
	return math::degToRadFactor * degrees;
}

inline double toRadians( const double& degrees )
{
	return math::degToRadFactor * degrees;
}

/**
 * @brief A 2D axis-aligned rectangle, with x,y (float) position and w,h (float) size components.  
 *   
 * The x,y position components are the "principle point" of the rectangle.
 * The w,h components are *added* to the x,y point to find the 3 other corners.
 */
struct Rect
{
	Rect() {}
	Rect( float xPos, float yPos, float width, float height )
	    : x( xPos )
	    , y( yPos )
	    , w( width )
	    , h( height )
	{
	}
	Rect( const vec2& ptA, const vec2& ptB )
	{
		auto sz = ptB - ptA;
		x       = ptA.x;
		y       = ptA.y;
		w       = sz.x;
		h       = sz.y;
	}
#ifdef GA_OPENFRAMEWORKS
	Rect( const ofRectangle& rect )
	    : x( rect.x )
	    , y( rect.y )
	    , w( rect.width )
	    , h( rect.height )
	{
	}
#endif

	float x, y, w, h;

	vec2 position() const { return vec2 { x, y }; }
	vec2 size() const { return vec2 { w, h }; }
	float area() const { return std::abs( w * h ); }
	vec2 min() const { return vec2( std::min( x, x + w ), std::min( y, y + h ) ); }  // same as position(), unless width or height < 0
	vec2 max() const { return vec2( std::max( x, x + w ), std::max( y, y + h ) ); }
	bool contains( const vec2& pt ) const
	{
		vec2 a = min();
		vec2 b = max();
		return pt.x >= a.x && pt.y >= a.y && pt.x <= b.x && pt.y <= b.y;
	}
};

// /**
//  * @brief A 2D axis-aligned bounding box.
//  *
//  */
// struct Bounds2D
// {
// 	vec2 min, max;
//     bool isInside( vec2
// 	vec2 center() const { return ( min + max ) * .5f; }
// 	float width() const { return max.x - min.x; }
// 	float height() const { return max.y - min.y; }
// };

/**
 * @brief A 3D axis-aligned bounding box.
 * 
 */
struct Bounds3D
{
	vec3 min, max;
	vec3 center() const { return ( min + max ) * .5f; }
	vec3 size() const { return max - min; }
	float width() const { return max.x - min.x; }
	float height() const { return max.y - min.y; }
	float depth() const { return max.z - min.z; }
};

/**
 * @brief Clamp a value between a minimum and a maximum
 * 
 * @tparam T type must support < and > comparison
 */
template <typename T>
inline const T& clamp( const T& val, const T& min, const T& max )
{
	return ( val < min )   ? min
	       : ( val > max ) ? max
	                       : val;
}

/**
 * @brief Map a value from one range to another.
 * 
 * Optionally, clamp the result to stay within the output range.
 * @tparam T type must support +, -, /, * operators (and <,> for clamp)
 * 
 */
template <typename T>
inline T map( const T& val, const T& inMin, const T& inMax, const T& outMin, const T& outMax, bool bClamp = false )
{
	T r = val;
	try {  // check for 0 divide
		r = ( val - inMin ) / ( inMax - inMin ) * ( outMax - outMin ) + outMin;
	} catch ( ... ) {
	}
	return bClamp ? clamp( r, outMin, outMax ) : r;
}

// linear interpolations
// ----------------------

template <typename T>
inline T lerp( const T& a, const T& b, const T& pct )
{
	return ( b - a ) * pct + a;
}

template <typename T>
inline T lerp( const T& a, const T& b, float pct )
{
	return ( b - a ) * pct + a;
}

inline float lerp( float a, float b, float pct )
{
	return ( b - a ) * pct + a;
}

inline quat lerp( const quat& a, const quat& b, float pct )
{
	return slerp( a, b, pct );
}

// quaternion rotation
inline quat slerp( const quat& a, const quat& b, float pct )
{
	return glm::slerp( a, b, pct );  // spherical lerp
}

// cubic bezier curve - https://cubic-bezier.com/
inline float cubicBezier( float x, float x1, float y1, float x2, float y2 )
{
	if ( x == 0. )
		return 0.;
	if ( x == 1. )
		return 1.;

	// from http://www.flong.com/texts/code/shapers_bez/

	float y0a = 0.;  // initial y
	float x0a = 0.;  // initial x
	float y1a = y1;  // 1st influence y
	float x1a = x1;  // 1st influence x
	float y2a = y2;  // 2nd influence y
	float x2a = x2;  // 2nd influence x
	float y3a = 1.;  // final y
	float x3a = 1.;  // final x

	float A = x3a - 3.f * x2a + 3.f * x1a - x0a;
	float B = 3.f * x2a - 6.f * x1a + 3.f * x0a;
	float C = 3.f * x1a - 3.f * x0a;
	float D = x0a;

	float E = y3a - 3.f * y2a + 3.f * y1a - y0a;
	float F = 3.f * y2a - 6.f * y1a + 3.f * y0a;
	float G = 3.f * y1a - 3.f * y0a;
	float H = y0a;

	auto slopeFromT = []( float t, float A, float B, float C ) -> float {
		return 1.f / ( 3.f * A * t * t + 2.f * B * t + C );  // dtdx
	};
	auto xFromT = []( float t, float A, float B, float C, float D ) -> float {
		return A * ( t * t * t ) + B * ( t * t ) + C * t + D;
	};
	auto yFromT = []( float t, float E, float F, float G, float H ) -> float {
		return E * ( t * t * t ) + F * ( t * t ) + G * t + H;
	};

	// Solve for t given x (using Newton-Raphelson), then solve for y given t.
	// Assume for the first guess that t = x.
	float currentt            = x;
	int nRefinementIterations = 5;
	for ( int i = 0; i < nRefinementIterations; i++ ) {
		float currentx     = xFromT( currentt, A, B, C, D );
		float currentslope = slopeFromT( currentt, A, B, C );
		currentt -= ( currentx - x ) * ( currentslope );
		currentt = clamp( currentt, 0.f, 1.f );
	}

	return yFromT( currentt, E, F, G, H );
}

// templated interpolation
// use an ease function to interpolate from one value to another
template <typename T>
inline T interpolate( const T& a, const T& b, float pct, std::function<float( float )> easeFn, bool bClamp = false )
{
	pct = bClamp ? ga::clamp( easeFn( pct ), 0.f, 1.f ) : easeFn( pct );  // use easing on the pct
	return ga::lerp( a, b, pct );                                         // then lerp based on eased pct
}

}  // namespace ga