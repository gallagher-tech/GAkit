#pragma once
#include "ga/graph/component.h"
#include "ga/layout.h"
#include "ga/math.h"
#include "ga/texture.h"

namespace ga {

class Image : public ga::Component
{
public:
	std::string textureName = "";
	ga::Rect bounds2D       = { 0, 0, 0, 0 };
	ga::FitMode fitMode     = ga::FitMode::NONE;
	HorzAlign horzAlign     = HorzAlign::LEFT;
	VertAlign vertAlign     = VertAlign::TOP;
	bool crop               = false;

	Image() = default;

	Image( const std::string& texName,
	       const ga::Rect& bounds,
	       const ga::FitMode& fit,
	       const HorzAlign& hAlign = HorzAlign::LEFT,
	       const VertAlign& vAlign = VertAlign::TOP,
	       const bool& bCrop       = false )
	    : textureName( texName )
	    , bounds2D( bounds )
	    , fitMode( fit )
	    , horzAlign( hAlign )
	    , vertAlign( vAlign )
	    , crop( bCrop )
	{
	}

	void draw()
	{
		if ( auto tex = ga::textureCache().get( textureName ) ) {
			if ( tex->isAllocated() ) {
				ga::vec2 texDims { tex->getWidth(), tex->getHeight() };
				auto texScale     = ga::calcScaleToFit( texDims, bounds2D.size(), fitMode );
				auto texSize      = texDims * texScale;
				auto boundsAnchor = bounds2D.position() + anchor( horzAlign, vertAlign ) * bounds2D.size();
				auto texPos       = boundsAnchor - anchor( horzAlign, vertAlign ) * texSize;

				if ( !crop ) {
					tex->draw( texPos, texSize.x, texSize.y );

				} else {
					auto cropPtA = glm::max( bounds2D.min(), texPos );
					auto cropPtB = glm::min( bounds2D.max(), texPos + texSize );
					auto cropSz  = cropPtB - cropPtA;
					auto subPtA  = ( cropPtA - texPos ) / texScale;
					auto subPtB  = ( cropPtB - texPos ) / texScale;
					auto subSz   = subPtB - subPtA;
					tex->drawSubsection( cropPtA.x, cropPtA.y, cropSz.x, cropSz.y, subPtA.x, subPtA.y, subSz.x, subSz.y );
					// tex->drawSubsection(ofRectangle(cropPtA, cropPtB), ofRectangle(subPtA, subPtB));
				}
			} else {
				// texture is unallocated
				// todo: log warning?
			}
		} else {
			// texture not found in cache
			// todo: log warning?
		}
	}

	ga::Rect getDrawBounds()
	{
		auto drawBounds = bounds2D;
		if ( auto img = ga::textureCache().get( textureName ) ) {
			if ( img->isAllocated() ) {
				ga::vec2 texDims { img->getWidth(), img->getHeight() };
				auto texScale     = ga::calcScaleToFit( texDims, bounds2D.size(), fitMode );
				auto texSize      = texDims * texScale;
				auto boundsAnchor = bounds2D.position() + anchor( horzAlign, vertAlign ) * bounds2D.size();
				auto texPos       = boundsAnchor - anchor( horzAlign, vertAlign ) * texSize;
				if ( !crop ) {
					drawBounds = ga::Rect { texPos, texPos + texSize };
				} else {
					drawBounds = ga::Rect { glm::max( bounds2D.min(), texPos ),
					                        glm::min( bounds2D.max(), texPos + texSize ) };
				}
			}
		}
		return drawBounds;
	}
};
}  // namespace ga