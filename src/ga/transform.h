#pragma once
#include "ga/math.h"

namespace ga {

/**
 * @brief Transform represents a basic model matrix: translate * rotate * scale
 * Stored internally as ga::mat4 (4x4 matrix) as well as:
 *  - ga::vec3 translation
 *  - ga::quat rotation
 *  - ga::vec3 scale
 * 
 */
class Transform
{

public:
	Transform()
	    : m_transform( 1.f ), m_translation( 0.f ), m_rotation(), m_scale( 1.f )
	{
		flagDirty();
	}

	Transform( const mat4& transform )
	{
		setMatrix( transform );
	}

	Transform( const vec3& translation, const quat& rotation, const vec3& scale )
	    : m_transform( 1.f ), m_translation( translation ), m_scale( scale ), m_rotation( rotation )
	{
		flagDirty();
	}

	// setters

	inline Transform& setMatrix( const mat4& matrix )
	{
		m_transform = matrix;
		decompose();
		return *this;
	}

	inline Transform& set( const mat4& matrix )
	{
		return setMatrix( matrix );
	}

	inline Transform& setTranslation( const vec3& translation )
	{
		m_translation = translation;
		flagDirty();
		return *this;
	}

	inline Transform& setTranslation( const vec2& translation )
	{
		return setTranslation( vec3 { translation.x, translation.y, m_translation.z } );
	}

	inline Transform& setRotation( const quat& rotation )
	{
		m_rotation = rotation;
		flagDirty();
		return *this;
	}

	inline Transform& setScale( const vec3& scale )
	{
		m_scale = scale;
		flagDirty();
		return *this;
	}

	inline Transform& setEulerRotation( const vec3& radians )
	{
		return setRotation( quat( radians ) );
	}

	// getters

	inline const vec3& getTranslation() const { return m_translation; }
	inline const quat& getRotation() const { return m_rotation; }
	inline const vec3& getScale() const { return m_scale; }
	inline vec3 getEulerRotation() const { return glm::eulerAngles( m_rotation ); }

	inline const mat4& getMatrix() const
	{
		clean();
		return m_transform;
	}

	// helpers

	inline Transform& translate( const vec3& vec ) { return setTranslation( m_translation + vec ); }
	inline Transform& translate( const vec2& vec ) { return setTranslation( { m_translation.x + vec.x, m_translation.y + vec.y, m_translation.z } ); }
	inline Transform& translateX( float x ) { return setTranslation( { m_translation.x + x, m_translation.y, m_translation.z } ); }
	inline Transform& translateY( float y ) { return setTranslation( { m_translation.x, m_translation.y + y, m_translation.z } ); }
	inline Transform& translateZ( float z ) { return setTranslation( { m_translation.x, m_translation.y, m_translation.z + z } ); }
	inline Transform& scale( const vec3& pct ) { return setScale( m_scale * pct ); }
	inline Transform& scale( float pct ) { return setScale( m_scale * pct ); }
	inline Transform& rotate( const vec3& eulerRadians ) { return setEulerRotation( eulerRadians + glm::eulerAngles( m_rotation ) ); }
	inline Transform& rotateAround( const vec3& axis, float radians ) { return rotate( axis * radians ); }
	inline Transform& rotateX( float radians ) { return rotateAround( { 1, 0, 0 }, radians ); }
	inline Transform& rotateY( float radians ) { return rotateAround( { 0, 1, 0 }, radians ); }
	inline Transform& rotateZ( float radians ) { return rotateAround( { 0, 0, 1 }, radians ); }

	// implicit conversion to mat4

	inline operator const mat4&() const { return getMatrix(); }
	inline Transform& operator=( const mat4& m ) { return setMatrix( m ); }
	inline Transform operator*( const mat4& m ) { return Transform( getMatrix() * m ); }
	inline Transform operator*( const Transform& t ) { return Transform( getMatrix() * t.getMatrix() ); }

protected:
	vec3 m_translation;
	vec3 m_scale;
	quat m_rotation;
	mat4 m_transform;
	bool m_dirty;  // matrix needs rebuilding

	inline void flagDirty() { m_dirty = true; }

	inline void clean() const
	{
		if ( m_dirty ) {
			// hack to keep internal matrix synced while keeping const-ness
			auto* self = const_cast<Transform*>( this );
			// rebuild model matrix - translate, rotate, scale
			self->m_transform = glm::translate( mat4( 1.0 ), m_translation );
			self->m_transform = m_transform * glm::toMat4( m_rotation );
			self->m_transform = glm::scale( m_transform, m_scale );
			self->m_dirty     = false;
		}
	}

	inline void decompose()
	{
		// decomposes the internal transform matrix into translation, rotation, scale components
		vec3 skew;
		vec4 perspective;  // unused here but needed by the method signature
		glm::decompose( m_transform, m_scale, m_rotation, m_translation, skew, perspective );
		m_dirty = false;
	}
};

}  // namespace ga