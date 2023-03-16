#pragma once
#include "ga/math.h"

namespace ga {
enum class HorzAlign
{
	LEFT,
	CENTER,
	RIGHT
};
enum class VertAlign
{
	TOP,
	MIDDLE,
	BOTTOM
};

// helper for alignment, returns anchor in percent
inline vec2 anchor( HorzAlign h, VertAlign v )
{
	ga::vec2 p { 0, 0 };
	switch ( h ) {
		case HorzAlign::CENTER:
			p.x = .5;
			break;
		case HorzAlign::RIGHT:
			p.x = 1.;
			break;
	}
	switch ( v ) {
		case VertAlign::MIDDLE:
			p.y = .5;
			break;
		case VertAlign::BOTTOM:
			p.y = 1.;
			break;
	}
	return p;
}

/*
 FitMode - describes how to scale an object to fit a container object
 */
enum class FitMode
{
	NONE,       // no auto scaling
	FIT,        // scale proportionally to fit entirely within container
	STRETCH,    // match width and height to container
	COVER,      // scale to cover container
	FIT_WIDTH,  // fit width, scale height accordingly
	FIT_HEIGHT,
	FIT_DEPTH  // for 3D
};

inline vec3 calcScaleToFit( const vec3& sourceSize, const vec3& containerSize, const FitMode& fitMode )
{
	vec3 scale = vec3( 1.f );

	// calculate sizing based on fit mode
	switch ( fitMode ) {

		case FitMode::FIT: {
			vec3 s   = containerSize / sourceSize;
			float s0 = std::min( std::min( s.x, s.y ), s.z );
			scale    = vec3( s0 );
			break;
		}

		case FitMode::STRETCH: {
			scale = containerSize / sourceSize;
			break;
		}

		case FitMode::COVER: {
			vec3 s   = containerSize / sourceSize;
			float s1 = std::max( std::max( s.x, s.y ), s.z );
			scale    = vec3( s1 );
			break;
		}

		case FitMode::FIT_WIDTH: {
			float sx = containerSize.x / sourceSize.x;
			scale    = vec3( sx );
			break;
		}

		case FitMode::FIT_HEIGHT: {
			float sy = containerSize.y / sourceSize.y;
			scale    = vec3( sy );
			break;
		}

		case FitMode::FIT_DEPTH: {
			float sz = containerSize.z / sourceSize.z;
			scale    = vec3( sz );
			break;
		}

		case FitMode::NONE:
		default:
			break;
	}

	return scale;
}

inline vec2 calcScaleToFit( const vec2& sourceSize, const vec2& containerSize, const FitMode& fitMode )
{
	return calcScaleToFit( vec3( sourceSize, 1.f ), vec3( containerSize, 1.f ), fitMode );
}

}  // namespace ga