#include "ga/render.h"
#include "ga/defines.h"
#include "ga/gl.h"

#ifdef GA_OPENFRAMEWORKS
#include "ofGraphics.h"
#endif

namespace ga {
void Renderer::pushMatrix( MatrixType type )
{
	m_matrixStack[type].push_back( m_matrices[type] );

#ifdef GA_OPENFRAMEWORKS
	switch ( type ) {
		case MatrixType::MODEL:
		case MatrixType::VIEW:
			ofSetMatrixMode( ofMatrixMode::OF_MATRIX_MODELVIEW );
			break;
		case MatrixType::PROJECTION:
			ofSetMatrixMode( ofMatrixMode::OF_MATRIX_PROJECTION );
			break;
	}
	ofPushMatrix();
#endif
}

void Renderer::popMatrix( MatrixType type )
{
	if ( !m_matrixStack[type].empty() ) {
		m_matrices[type] = m_matrixStack[type].back();
		m_matrixStack[type].pop_back();

#ifdef GA_OPENFRAMEWORKS
		switch ( type ) {
			case MatrixType::MODEL:
			case MatrixType::VIEW:
				ofSetMatrixMode( ofMatrixMode::OF_MATRIX_MODELVIEW );
				break;
			case MatrixType::PROJECTION:
				ofSetMatrixMode( ofMatrixMode::OF_MATRIX_PROJECTION );
				break;
		}
		ofPopMatrix();
#endif

	} else {
		// todo: log error
	}
}

void Renderer::setMatrix( const ga::mat4& matrix, MatrixType type )
{
	m_matrices[type] = matrix;

#ifdef GA_OPENFRAMEWORKS
	switch ( type ) {
		case MatrixType::MODEL:
			ofSetMatrixMode( ofMatrixMode::OF_MATRIX_MODELVIEW );
			ofLoadMatrix( matrix );
			break;
		case MatrixType::VIEW:
			ofLoadViewMatrix( matrix );
			break;
		case MatrixType::PROJECTION:
			ofSetMatrixMode( ofMatrixMode::OF_MATRIX_PROJECTION );
			ofLoadMatrix( matrix );
			break;
	}
#endif
}

const ga::mat4& Renderer::getMatrix( MatrixType type )
{
	return m_matrices[type];
}

void Renderer::multMatrix( const ga::mat4& matrix, MatrixType type )
{
	m_matrices[type] = m_matrices[type] * matrix;

#ifdef GA_OPENFRAMEWORKS
	switch ( type ) {
		case MatrixType::MODEL:
			ofSetMatrixMode( ofMatrixMode::OF_MATRIX_MODELVIEW );
			ofMultMatrix( matrix );
			break;
		case MatrixType::VIEW:
			ofMultViewMatrix( matrix );
			break;
		case MatrixType::PROJECTION:
			ofSetMatrixMode( ofMatrixMode::OF_MATRIX_PROJECTION );
			ofMultMatrix( matrix );
			break;
	}
#endif
}

void Renderer::translate( const vec3& translation )
{
	multMatrix( glm::translate( mat4( 1. ), translation ) );
}

void Renderer::translate( const vec2& translation )
{
	multMatrix( glm::translate( mat4( 1. ), vec3( translation, 0.f ) ) );
}

void Renderer::rotate( const quat& rotation )
{
	multMatrix( glm::toMat4( rotation ), MatrixType::MODEL );
}

void Renderer::scale( const vec3& scale )
{
	multMatrix( glm::scale( mat4( 1. ), scale ) );
}

void Renderer::setGlobalColor( const Color& color )
{
	m_globalColor = color;
#ifdef GA_OPENFRAMEWORKS
	ofSetColor( toOf( color ) );
#endif
}

const Color& Renderer::getGlobalColor()
{
	return m_globalColor;
}

void Renderer::clear( const Color& color )
{
	glClearColor( color.r, color.g, color.b, color.a );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

}  // namespace ga