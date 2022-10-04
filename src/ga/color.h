#pragma once
#include "ga/defines.h"
#include "ga/math.h"

#ifdef GA_OPENFRAMEWORKS
#include "ofColor.h"
#endif

namespace ga {

// color is simply a vec4 - r,g,b,a
using Color = ga::vec4;

// TODO: hsb lerp

#ifdef GA_OPENFRAMEWORKS
inline Color toGa( const ofFloatColor& color )
{
	return Color( color.r, color.g, color.b, color.a );
}

inline ofFloatColor toOf( const Color& color )
{
	return ofFloatColor( color.r, color.g, color.b, color.a );
}
#endif

}  // namespace ga