#include "ga/graph/components/paragraph_component.h"
#include "ga/render.h"
#include <regex>

namespace ga {

Paragraph::Paragraph()
    : m_offset( 0 )
    , m_size( -1 )
    , m_hAlignment( HorzAlignment::LEFT )
    , m_vAlignment( VertAlignment::TOP )
    , m_leading( -1 )
    , m_spacing( -1 )
    , m_isMarkdownFormatted( true )
    , m_isLayoutDirty( true )
    , m_cacheToFbo( false )
{
}

Paragraph& Paragraph::setFont( std::shared_ptr<Font> font, bool resetLeadingAndSpacing )
{
	m_font = font;
	if ( font ) {
		if ( resetLeadingAndSpacing || getLeading() == -1 )
			setLeading( font->getSize() * 1.25 );
		if ( resetLeadingAndSpacing || getWordSpacing() == -1 )
			setWordSpacing( font->getStringBoundingBox( "-", 0, 0 ).width );
	}
	m_isLayoutDirty = true;
	return *this;
}

Paragraph& Paragraph::setBoldFont( std::shared_ptr<Font> font )
{
	m_boldFont      = font;
	m_isLayoutDirty = true;
	return *this;
}

Paragraph& Paragraph::setItalicFont( std::shared_ptr<Font> font )
{
	m_italicFont    = font;
	m_isLayoutDirty = true;
	return *this;
}

Paragraph& Paragraph::setBoldItalicFont( std::shared_ptr<Font> font )
{
	m_boldItalicFont = font;
	m_isLayoutDirty  = true;
	return *this;
}

Paragraph& Paragraph::setText( std::string text )
{
	if ( m_text != text ) {
		m_text          = text;
		m_isLayoutDirty = true;
	}
	return *this;
}

Paragraph& Paragraph::setHorizontalAlignment( HorzAlignment alignment )
{
	if ( m_hAlignment != alignment ) {
		m_hAlignment    = alignment;
		m_isLayoutDirty = true;
	}
	return *this;
}

Paragraph& Paragraph::setVerticalAlignment( VertAlignment alignment )
{
	if ( m_vAlignment != alignment ) {
		m_vAlignment    = alignment;
		m_isLayoutDirty = true;
	}
	return *this;
}

// position offset from 0,0,0
Paragraph& Paragraph::setTextOffset( ga::vec3 textOffset )
{
	m_offset = textOffset;
	return *this;
}

// text box size

Paragraph& Paragraph::setSize( ga::vec2 size )
{
	if ( size != m_size ) {
		m_size          = size;
		m_isLayoutDirty = true;
	}
	return *this;
}

Paragraph& Paragraph::setTextColor( const ga::Color textColor )
{
	m_textColor = textColor;
	return *this;
}

// spacing between lines

Paragraph& Paragraph::setLeading( float px )
{
	if ( m_leading != px ) {
		m_leading       = px;
		m_isLayoutDirty = true;
	}
	return *this;
}

// spacing between words

Paragraph& Paragraph::setWordSpacing( float px )
{
	if ( m_spacing != px ) {
		m_spacing       = px;
		m_isLayoutDirty = true;
	}
	return *this;
}

Paragraph& Paragraph::setIsMarkdownText( bool isMarkdown )
{
	m_isMarkdownFormatted = isMarkdown;
	return *this;
}

Paragraph& Paragraph::setFboCacheEnabled( bool enable )
{
	m_cacheToFbo = enable;
	return *this;
}

void Paragraph::draw()
{
	cleanLayout();
	getRenderer().pushMatrix();
	getRenderer().translate( m_offset );
	auto pColor = getRenderer().getGlobalColor();
	getRenderer().setGlobalColor( pColor * m_textColor );

	drawParagraph();

	getRenderer().setGlobalColor( pColor );
	getRenderer().popMatrix();
}

std::vector<ga::Rect> Paragraph::getWordBounds()
{
	cleanLayout();
	std::vector<ga::Rect> bounds;
	bounds.reserve( m_words.size() );
	float wordBoundaryPadding = 0;
	float lineHeight          = m_font->getSize();
	for ( auto& word : m_words ) {
		bounds.emplace_back(
		    word.x - wordBoundaryPadding,
		    word.y - lineHeight - wordBoundaryPadding,
		    word.w + ( wordBoundaryPadding * 2 ),
		    lineHeight + ( wordBoundaryPadding * 2 ) );
	}
	return bounds;
}

ga::Rect Paragraph::getParagraphBounds()
{
	cleanLayout();
	auto wordBounds = getWordBounds();
	auto minPt      = ga::vec2( std::numeric_limits<float>::max() );
	auto maxPt      = ga::vec2( std::numeric_limits<float>::min() );
	for ( auto& wordBox : wordBounds ) {

		minPt.x = std::min( minPt.x, wordBox.x );
		minPt.y = std::min( minPt.y, wordBox.y );
		maxPt.x = std::max( maxPt.x, wordBox.x + wordBox.w );
		maxPt.y = std::max( maxPt.y, wordBox.y + wordBox.h );
	}
	return ga::Rect( minPt, maxPt );
}

int Paragraph::getNumberOfLines()
{
	cleanLayout();
	return m_lines.size();
}

void Paragraph::cleanLayout()
{
	// recalc line breaks
	if ( m_isLayoutDirty ) {
		if ( m_isMarkdownFormatted ) {
			m_styledText = parseMarkdownStyles( m_text );
		} else {
			m_styledText.text   = m_text;
			m_styledText.styles = std::vector<FontStyle>( m_text.length(), FontStyle::REGULAR );
		}
		calculateTextFlow();
		m_isLayoutDirty = false;

		if ( m_cacheToFbo && m_font ) {
			m_fboRect = getParagraphBounds();
			m_fboRect.h += m_font->getSize();  // add extra to account for descenders... todo: fix the word/paragraph bounds?
			if ( m_fbo.getWidth() < m_fboRect.w || m_fbo.getHeight() < m_fboRect.h ) {
				m_fbo.allocate( m_fboRect.w, m_fboRect.h, GL_RGBA );
			}

			m_fbo.begin();
			auto bg = m_textColor;
			bg.a    = 0.f;
			getRenderer().clear( bg );
			getRenderer().translate( -1.f * m_fboRect.position() );
			drawParagraph( true );
			m_fbo.end();
		}
	}
}

void Paragraph::calcWordRect( Word& word )
{
	if ( !m_font || word.text.empty() )
		return;
	if ( word.style.size() < word.text.length() ) {
		// append REGULAR font style to style vector to match word length
		word.style.insert( word.style.end(), word.text.length() - word.style.size(), FontStyle::REGULAR );
	}
	word.x = 0;
	word.y = 0;
	word.w = 0;
	word.h = 0;

	auto font       = m_font;
	auto boldFont   = m_boldFont ? m_boldFont : m_font;
	auto italicFont = m_italicFont ? m_italicFont : m_font;

	// split word text into tokens by style vector

	auto findNextStyle = []( const std::vector<FontStyle>& styles, size_t currPos, FontStyle currStyle ) -> size_t {
		for ( size_t pos = currPos + 1; pos < styles.size(); ++pos ) {
			if ( styles[pos] != currStyle ) {
				return pos;
			}
		}
		return styles.size();
	};

	FontStyle currStyle = word.style[0];
	size_t tokenBegin   = 0;
	size_t tokenEnd     = findNextStyle( word.style, 0, currStyle );

	while ( tokenEnd < word.style.size() ) {
		StyledToken token;
		token.text   = word.text.substr( tokenBegin, tokenEnd );
		token.style  = currStyle;
		token.bounds = fontForStyle( currStyle )->getBounds( token.text );
		if ( word.styledTokens.size() ) {
			token.bounds.x = word.styledTokens.back().bounds.x + word.styledTokens.back().bounds.w;
		}
		word.styledTokens.push_back( token );
		tokenBegin = tokenEnd;
		currStyle  = word.style[tokenBegin];
		tokenEnd   = findNextStyle( word.style, tokenBegin, currStyle );

		if ( word.h < token.bounds.h )
			word.h = token.bounds.h;  // grow word bounds
	}
	StyledToken token;
	token.text   = word.text.substr( tokenBegin, tokenEnd );
	token.style  = currStyle;
	token.bounds = fontForStyle( currStyle )->getBounds( token.text );
	if ( word.styledTokens.size() ) {
		token.bounds.x = word.styledTokens.back().bounds.x + word.styledTokens.back().bounds.w;
	}
	word.styledTokens.push_back( token );
	// grow word bounds
	if ( word.h < token.bounds.h )
		word.h = token.bounds.h;
	word.w = word.styledTokens.back().bounds.x + word.styledTokens.back().bounds.w;
}

Paragraph::StyledText Paragraph::parseMarkdownStyles( const std::string& text )
{
	StyledText styledTextBold, styledText;

	const auto bReg = std::regex { "(\\*\\*|__)(.*?)\\1" };  // **bold**
	const auto iReg = std::regex { "(\\*|_)(.+?)\\1" };      // *italics*

	// parse **bold**
	std::string str = text;
	std::smatch m;
	while ( std::regex_search( str, m, bReg ) ) {  // find next **bold** block

		styledTextBold.text += m.prefix();                                                                     // add text before **bold** block
		styledTextBold.styles.insert( styledTextBold.styles.end(), m.prefix().length(), FontStyle::REGULAR );  // mark text before **bold** block as REGULAR

		styledTextBold.text += m[2];                                                                  // add **bold** text
		styledTextBold.styles.insert( styledTextBold.styles.end(), m[2].length(), FontStyle::BOLD );  // mark **bold** text as BOLD

		str = m.suffix();  // the text after the **bold** block is parsed next
	}
	if ( !str.empty() ) {                                                                               // done with parsing the string (no more **bold** blocks)
		styledTextBold.text += str;                                                                     // append the rest of the string
		styledTextBold.styles.insert( styledTextBold.styles.end(), str.length(), FontStyle::REGULAR );  // mark as REGULAR
	}

	// parse *italics*
	str               = styledTextBold.text;
	auto boldStylesIt = styledTextBold.styles.begin();
	while ( std::regex_search( str, m, iReg ) ) {

		styledText.text += m.prefix();
		styledText.styles.insert( styledText.styles.end(), boldStylesIt, boldStylesIt + m.prefix().length() );

		styledText.text += m[2];
		// check each char of m[2] for previously parsed BOLD
		boldStylesIt += m.prefix().length();
		auto matchBoldStylesIt = boldStylesIt + m[1].length();  // move bold styles iterator to start of m[2]
		for ( int i = 0; i < m[2].length(); ++i ) {
			auto boldStyle = *( matchBoldStylesIt + i );
			if ( boldStyle == FontStyle::BOLD ) {
				styledText.styles.push_back( FontStyle::BOLD_ITALIC );
			} else {
				styledText.styles.push_back( FontStyle::ITALIC );
			}
		}

		str = m.suffix();
		boldStylesIt += m[0].length();
	}
	if ( !str.empty() ) {
		styledText.text += str;
		styledText.styles.insert( styledText.styles.end(), boldStylesIt, boldStylesIt + str.length() );
	}
	return styledText;
}

}  // namespace ga
