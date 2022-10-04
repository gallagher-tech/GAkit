#pragma once
#include "ga/color.h"
#include "ga/fbo.h"
#include "ga/font.h"
#include "ga/graph/component.h"
#include "ga/math.h"

namespace ga {

class Paragraph : public ga::Component
{
public:
	enum class HorzAlignment
	{
		LEFT,
		CENTER,
		RIGHT
	};
	enum class VertAlignment
	{
		TOP,
		MIDDLE,
		BOTTOM
	};

	Paragraph();

	Paragraph& setFont( std::shared_ptr<Font> font, bool resetLeadingAndSpacing = true );
	std::shared_ptr<Font> getFont() { return m_font; }

	Paragraph& setBoldFont( std::shared_ptr<Font> font );
	std::shared_ptr<Font> getBoldFont() { return m_boldFont; }

	Paragraph& setItalicFont( std::shared_ptr<Font> font );
	std::shared_ptr<Font> getItalicFont() { return m_italicFont; }

	Paragraph& setBoldItalicFont( std::shared_ptr<Font> font );
	std::shared_ptr<Font> getBoldItalicFont() { return m_boldItalicFont; }

	Paragraph& setText( std::string text );
	std::string getText() { return m_text; }

	Paragraph& setHorizontalAlignment( HorzAlignment alignment );
	Paragraph& setVerticalAlignment( VertAlignment alignment );
	HorzAlignment getHorizontalAlignment() { return m_hAlignment; }
	VertAlignment getVerticalAlignment() { return m_vAlignment; }

	// position offset from 0,0,0
	Paragraph& setTextOffset( ga::vec3 textOffset );
	Paragraph& setTextOffset( ga::vec2 textOffset ) { return setTextOffset( ga::vec3( textOffset, 0 ) ); }

	ga::vec3 getTextOffset() { return m_offset; }

	// text box size
	Paragraph& setSize( ga::vec2 size );
	ga::vec2 getSize() { return m_size; }

	Paragraph& setTextColor( const ga::Color textColor );
	ga::Color getTextColor() { return m_textColor; }

	// spacing between lines
	Paragraph& setLeading( float px );
	float getLeading() { return m_leading; }

	// spacing between words
	Paragraph& setWordSpacing( float px );
	float getWordSpacing() { return m_spacing; }

	// toggle markdown (bold and italic) parsing
	Paragraph& setIsMarkdownText( bool isMarkdown );
	bool getIsMarkdownText() const { return m_isMarkdownFormatted; }

	// enable/disable fbo cache
	Paragraph& setFboCacheEnabled( bool enable );
	bool getIsFboCacheEnabled() { return m_cacheToFbo; }

	void draw();

	std::vector<ga::Rect> getWordBounds();
	ga::Rect getParagraphBounds();
	int getNumberOfLines();

	void cleanLayout();

protected:
	enum class FontStyle
	{
		REGULAR,
		BOLD,
		ITALIC,
		BOLD_ITALIC
	};

	struct StyledText
	{
		std::string text;
		std::vector<FontStyle> styles;  // char-by-char styles
	};

	struct StyledToken
	{
		std::string text;
		FontStyle style;
		ga::Rect bounds;
	};

	struct Word
	{
		std::string text;
		float x, y, w, h;
		std::vector<FontStyle> style;           // char-by-char style
		std::vector<StyledToken> styledTokens;  // bounds per styled part of text
	};

	std::vector<Word> m_words;
	std::vector<std::vector<Word*>> m_lines;

	std::shared_ptr<Font> fontForStyle( FontStyle style )
	{
		switch ( style ) {
			case FontStyle::BOLD:
				return m_boldFont ? m_boldFont : m_font;
			case FontStyle::ITALIC:
				return m_italicFont ? m_italicFont : m_font;
			case FontStyle::BOLD_ITALIC: {
				if ( m_boldItalicFont )
					return m_boldItalicFont;
				if ( m_italicFont )
					return m_italicFont;
				if ( m_boldFont )
					return m_boldFont;
				return m_font;
			}
			default:
				return m_font;
		}
	}

	void calcWordRect( Word& word );

	void drawParagraph( bool forceRedraw = false )
	{
		if ( m_font == nullptr || m_text.empty() ) {
			return;
		}
		if ( !m_cacheToFbo || forceRedraw ) {
			for ( auto& word : m_words ) {
				// m_font->draw( word.text, word.x, word.y );
				for ( auto& token : word.styledTokens ) {
					fontForStyle( token.style )->drawString( token.text, word.x + token.bounds.x, word.y );
				}
			}
		} else {
			if ( m_fbo.isAllocated() ) {
				m_fbo.draw( m_fboRect.x, m_fboRect.y, m_fbo.getWidth(), m_fbo.getHeight() );
			}
		}
	}

	StyledText Paragraph::parseMarkdownStyles( const std::string& text );

	std::string m_text;       // raw text, could contain **bold** and *italics*
	StyledText m_styledText;  // styled text, parsed for **bold** and *italics*
	std::shared_ptr<Font> m_font, m_boldFont, m_italicFont, m_boldItalicFont;

	glm::vec3 m_offset;
	glm::vec2 m_size;

	ga::Color m_textColor;
	HorzAlignment m_hAlignment;
	VertAlignment m_vAlignment;
	float m_leading;
	float m_spacing;
	bool m_isMarkdownFormatted;  // bold and italic formatting

	ga::Fbo m_fbo;
	ga::Rect m_fboRect;

	bool m_isLayoutDirty = true;
	bool m_cacheToFbo    = false;

	// --- below methods are adapted from ofxParagraph ---
	/*
	Copyright (C) 2014 Stephen Braitsch [http://braitsch.io]

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
	*/

	void calculateTextFlow()
	{
		if ( !m_font )
			return;
		m_words.clear();
		float lineHeight = m_font->getSize();
		std::string str  = m_styledText.text;

		// break paragraph into words //
		auto spPosition = str.find( " " );
		auto nlPosition = str.find( "\n" );
		auto position   = spPosition < nlPosition ? spPosition : nlPosition;
		size_t textPos  = 0;

		while ( position != std::string::npos ) {
			std::string s = str.substr( 0, position );
			//auto rect     = m_font->rect( s );
			Word word = { s };  //, rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight() };
			word.style.insert( word.style.end(), m_styledText.styles.begin() + textPos, m_styledText.styles.begin() + textPos + position );
			calcWordRect( word );
			m_words.push_back( word );
			str.erase( 0, position + 1 );
			textPos += position + 1;

			if ( position == nlPosition ) {
				word = { "\n", 0.f, 0.f, 0.f, 0.f, { FontStyle::REGULAR } };
				m_words.push_back( word );  // add new line "word"
			}

			spPosition = str.find( " " );
			nlPosition = str.find( "\n" );
			position   = spPosition < nlPosition ? spPosition : nlPosition;
		}
		// append the last word //
		//auto rect     = m_font->rect( str );
		Word word = { str };  //, rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight() };
		word.style.insert( word.style.end(), m_styledText.styles.begin() + textPos, m_styledText.styles.end() );
		calcWordRect( word );
		m_words.push_back( word );

		// assign words to lines //
		float x = 0;
		float y = lineHeight * .85;  // origin is top of the text, but ofTTF draws from baseline

		m_lines.clear();
		std::vector<Word*> line;
		for ( auto& word : m_words ) {

			if ( word.text == "\n" ) {
				// new line character forces new line

				x = 0;
				y += m_leading;
				word.x = x;
				word.y = y;
				if ( line.size() > 0 ) {
					m_lines.push_back( line );
				}
				line.clear();

			} else if ( x + word.w < m_size.x ) {
				// append word to current line

				word.x = x;
				word.y = y;
				x += word.w + m_spacing;
				line.push_back( &word );

			} else {
				// create new line starting with this word

				if ( line.size() > 0 )
					y += m_leading;
				word.x = 0;
				word.y = y;  // account for descender - todo: fix
				x      = word.w + m_spacing;
				if ( line.size() > 0 )
					m_lines.push_back( line );
				line.clear();
				line.push_back( &word );
			}
		}
		// append the last line //
		m_lines.push_back( line );
		// mHeight = mLines.size() * ( lineHeight + leading );

		// reposition words for right & center aligned paragraphs //
		if ( m_hAlignment == HorzAlignment::CENTER ) {
			for ( auto& line : m_lines ) {
				int lineWidth = 0;
				for ( auto* word : line ) {
					lineWidth += word->w;
				}
				lineWidth += m_spacing * ( line.size() - 1 );
				// calculate the amount each word should move over //
				float offset = ( m_size.x - lineWidth ) / 2;
				for ( auto* word : line ) {
					word->x += offset;
				}
			}
		} else if ( m_hAlignment == HorzAlignment::RIGHT ) {
			for ( auto& line : m_lines ) {
				auto* lword = line.back();
				// calculate the distance the last word in each line is from the right boundary //
				int offset = m_size.x - ( lword->x + lword->w );
				for ( auto* word : line ) {
					word->x += offset;
				}
			}
		}

		if ( m_vAlignment == VertAlignment::MIDDLE ) {
			float pHeight = lineHeight + ( m_lines.size() - 1 ) * m_leading;
			float yOffset = ( m_size.y - pHeight ) * .5;
			for ( auto& word : m_words ) {
				word.y += yOffset;
			}
		} else if ( m_vAlignment == VertAlignment::BOTTOM ) {
			float pHeight = lineHeight + ( m_lines.size() - 1 ) * m_leading;
			float yOffset = ( m_size.y - pHeight );
			for ( auto& word : m_words ) {
				word.y += yOffset;
			}
		}
	}
};
}  // namespace ga