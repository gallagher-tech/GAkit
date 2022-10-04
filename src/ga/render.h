#pragma once
#include "ga/color.h"
#include "ga/math.h"
#include <map>
#include <vector>

namespace ga {

enum class MatrixType
{
	MODEL,
	VIEW,
	PROJECTION
};

class Renderer
{
public:
	void pushMatrix( MatrixType type = MatrixType::MODEL );
	void popMatrix( MatrixType type = MatrixType::MODEL );

	void setMatrix( const ga::mat4& matrix, MatrixType type = MatrixType::MODEL );
	const ga::mat4& getMatrix( MatrixType type = MatrixType::MODEL );

	void multMatrix( const ga::mat4& matrix, MatrixType type = MatrixType::MODEL );

	// space helpers
	void translate( const vec3& translation );
	void translate( const vec2& translation );
	void rotate( const quat& rotation );
	void scale( const vec3& scale );

	// color / style management
	void setGlobalColor( const ga::Color& color );
	const ga::Color& getGlobalColor();

	// gl convenience functions
	void clear( const ga::Color& color );  // clear fbo

protected:
	std::map<MatrixType, ga::mat4> m_matrices {
	    { MatrixType::MODEL, ga::mat4( 1.f ) },
	    { MatrixType::VIEW, ga::mat4( 1.f ) },
	    { MatrixType::PROJECTION, ga::mat4( 1.f ) } };
	std::map<MatrixType, std::vector<ga::mat4>> m_matrixStack;
	Color m_globalColor { 1.f, 1.f, 1.f, 1.f };
};

// singleton
inline Renderer& getRenderer()
{
	static Renderer r;
	return r;
}

struct MatrixScope
{
	MatrixScope( MatrixType type = MatrixType::MODEL, Renderer& renderer = getRenderer() )
	    : m_type( type )
	    , m_renderer( &renderer )
	{
		m_renderer->pushMatrix( type );
	}
	~MatrixScope()
	{
		m_renderer->popMatrix();
	}

protected:
	MatrixType m_type;
	Renderer* m_renderer;
};

}  // namespace ga