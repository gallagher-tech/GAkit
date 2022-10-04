#pragma once
#include "ga/math.h"
#include <functional>
#include <vector>

namespace ga {

using EasingFn = std::function<float( float )>;

namespace ease {

	//
	// standardized easing / tweening functions -
	//		p = f(t)
	//
	//      convert 't' time (percent 0-1) to 'p' position (percent 0-1)
	//		based on an easing curve - see http://robertpenner.com/easing/penner_chapter7_tweening.pdf
	//

	// exponential curves
	// ------------------
	// https://github.com/jesusgollonet/ofpennereasing/blob/master/PennerEasing/Expo.cpp

	inline float expoIn( float t )
	{  // t is percent (0.-1.) of transition
		return ( t == 0.0f ) ? 0.0f : pow( 2.0f, 10.0f * ( t - 1.0f ) );
	}

	inline float expoOut( float t )
	{
		return ( t == 1.0f ) ? 1.0f : 1.0f - pow( 2.0f, -10.0f * t );
	}

	inline float expoInOut( float t )
	{
		if ( t == 0.0f ) {
			return 0.0f;
		}

		if ( t == 1.0f ) {
			return 1.0f;
		}

		if ( ( t /= 0.5f ) < 1.0f ) {
			return 0.5f * pow( 2.0f, 10.0f * ( t - 1.0f ) );
		}

		return 0.5f * ( -pow( 2.0f, -10.0f * --t ) + 2.0f );
	}

	// cubic / quadratic curves
	// ------------------------

	inline float cubeInQuadOut( float t )
	{
		if ( t < 0.5f ) {
			return 4.0f * t * t * t;
		}  // cubic in

		return ( -2.0f * t * t ) + ( 4.0f * t ) - 1.0f;  // quadratic out
	}

	// material design curves
	// ----------------------
	// https://material.io/design/motion/speed.html#easing

	// Material "standard ease" (decelerate in, accelerate out)
	inline float material( float t )
	{
		return cubicBezier( t, 0.4f, 0.0f, 0.2f, 1.0f );
	}

	// Material "accelerate" (e.g. exit screen)
	inline float materialEnter( float t )
	{
		return cubicBezier( t, 0.0f, 0.0f, 0.2f, 1.0f );
	}

	// Material "decelerate" (e.g. enter screen)
	inline float materialExit( float t )
	{
		return cubicBezier( t, 0.4f, 0.0f, 1.0f, 1.0f );
	}
}  // namespace ease

enum class EaseType
{
	LINEAR,
	EXPO_IN,
	EXPO_OUT,
	EXPO_IN_OUT,
	CUBE_IN_QUAD_OUT,
	MATERIAL,
	MATERIAL_ENTER,
	MATERIAL_EXIT,
	CUSTOM,
	// for loops:
	NUM_TYPES,
	DEFAULT = LINEAR,
};

inline std::function<float( float )> easeFn( const EaseType& type )
{
	using namespace ease;
	switch ( type ) {
		case EaseType::EXPO_IN:
			return expoIn;
		case EaseType::EXPO_OUT:
			return expoOut;
		case EaseType::EXPO_IN_OUT:
			return expoInOut;
		case EaseType::CUBE_IN_QUAD_OUT:
			return cubeInQuadOut;
		case EaseType::MATERIAL:
			return material;
		case EaseType::MATERIAL_ENTER:
			return materialEnter;
		case EaseType::MATERIAL_EXIT:
			return materialExit;
		case EaseType::LINEAR:
		default:
			return []( float t ) { return t; };
	}
}

}  // namespace ga
