#pragma once
#include "ga/defines.h"
#include "ga/math.h"
#include "ga/resource.h"
#include <sstream>
#include <string>

#ifdef GA_OPENFRAMEWORKS
#include "ofTrueTypeFont.h"
#endif

namespace ga {

#ifdef GA_OPENFRAMEWORKS
class Font : public ofTrueTypeFont
{
public:
	const ofTrueTypeFontSettings& getSettings()
	{
		return ofTrueTypeFont::settings;
	}
	float getWidth( const std::string& str ) const
	{
		return getStringBoundingBox( str, 0, 0 ).width;
	}
	float getHeight( const std::string& str ) const
	{
		return getStringBoundingBox( str, 0, 0 ).width;
	}
	Rect getBounds( const std::string& str ) const
	{
		return getStringBoundingBox( str, 0, 0 );
	}
};

inline bool load( Font& font, const ofTrueTypeFontSettings& settings )
{
	return font.load( settings );
}

#endif
// todo: Font for Cinder

// ---------------------------
// Global Texture Cache
// ---------------------------
using FontCache = ResourceCache<Font>;

inline FontCache& fontCache()
{
	static FontCache texCache;
	return texCache;
}

// ---------------------------

struct FontStyle
{
	FontStyle() {}
	FontStyle( const std::string& file_, int size_, const std::string& name = "" )
	    : file( file_ )
	    , size( size_ )
	{
#ifdef GA_OPENFRAMEWORKS
		ofTrueTypeFontSettings settings( file, size );
		settings.antialiased = true;
		settings.contours    = false;
		settings.simplifyAmt = 0.3f;
		settings.dpi         = 72;
		settings.ranges      = {
            ofUnicode::Latin1Supplement,
            ofUnicode::LatinExtendedAdditional,
            ofUnicode::Latin,
            ofUnicode::LatinA,
            ofUnicode::GeneralPunctuation,
        };

		m_name = name;
		if ( m_name.empty() ) {
			std::stringstream ss;
			ss << file.substr( file.find_last_of( "/\\" ) + 1 ) << "@" << size;
			m_name = ss.str();
		}

		if ( !fontCache().load( name, settings ) ) {
			m_name = "";
		}
#endif
	}

	bool loaded() { return !m_name.empty(); }
	const std::string& name() { return m_name; }
	const std::shared_ptr<Font> font() { return fontCache().get( m_name ); }

	std::string file;
	int size;

protected:
	std::string m_name;
};

}  // namespace ga