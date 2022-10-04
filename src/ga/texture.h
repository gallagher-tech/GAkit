#pragma once
#include "ga/defines.h"
#include "ga/resource.h"

#ifdef GA_OPENFRAMEWORKS
#include "ofImage.h"

namespace ga {
class Texture : public ofTexture
{
};

inline bool load( Texture& texture, const std::string& path )
{
	return ofLoadImage( texture, path );
}
#endif
// todo: extend Texture for Cinder, or more bare-metal implementation

// ---------------------------
// Global Texture Cache
// ---------------------------
using TextureCache = ResourceCache<Texture>;

inline TextureCache& textureCache()
{
	static TextureCache texCache;
	return texCache;
}
}