#pragma once
#include "ga/json.h"

// ---------------- json conversions ----------------

namespace ga {

// ga::vec4
inline void from_json( const ga::Json& j, ga::vec4& v )
{
	try {
		v = { j.at( 0 ), j.at( 1 ), j.at( 2 ), j.at( 3 ) };
	} catch ( std::exception& e ) {
		std::stringstream ss;
		ss << "Error, vec4 from Json: " << j.dump() << " - " << e.what();
		throw std::domain_error( ss.str() );
	}
}
inline void to_json( ga::Json& j, const ga::vec4& v )
{
	j = { v[0], v[1], v[2], v[3] };
}

// ga::quat
inline void from_json( const ga::Json& j, ga::quat& q )
{
	try {
		q.x = j.at( 0 );
		q.y = j.at( 1 );
		q.z = j.at( 2 );
		q.w = j.at( 3 );
	} catch ( std::exception& e ) {
		std::stringstream ss;
		ss << "Error, quat from Json: " << j.dump() << " - " << e.what();
		throw std::domain_error( ss.str() );
	}
}
inline void to_json( ga::Json& j, const ga::quat& q )
{
	j = { q.x, q.y, q.z, q.w };
}

// ga::vec3
inline void from_json( const ga::Json& j, ga::vec3& v )
{
	try {
		v = { j.at( 0 ), j.at( 1 ), j.at( 2 ) };
	} catch ( std::exception& e ) {
		std::stringstream ss;
		ss << "Error, vec3 from Json: " << j.dump() << " - " << e.what();
		throw std::domain_error( ss.str() );
	}
}
inline void to_json( ga::Json& j, const ga::vec3& v )
{
	j = { v[0], v[1], v[2] };
}

// ga::vec2
inline void from_json( const ga::Json& j, ga::vec2& v )
{
	try {
		v = { j.at( 0 ), j.at( 1 ) };
	} catch ( std::exception& e ) {
		std::stringstream ss;
		ss << "Error, vec2 from Json: " << j.dump() << " - " << e.what();
		throw std::domain_error( ss.str() );
	}
}
inline void to_json( ga::Json& j, const ga::vec2& v )
{
	j = { v[0], v[1] };
}

// ga::mat4
inline void from_json( const ga::Json& j, ga::mat4& m )
{
	ga::mat4 _m = m;
	try {
		// glm is column major - convert from row major
		for ( int r = 0; r < 4; ++r ) {
			for ( int c = 0; c < 4; ++c ) {
				_m[c][r] = j[r][c];
			}
		}
	} catch ( std::exception& e ) {
		std::stringstream ss;
		ss << "Error, mat4 from Json: " << j.dump() << " - " << e.what();
		throw std::domain_error( ss.str() );
	}
	m = _m;
}

inline void to_json( ga::Json& j, const ga::mat4& m )
{
	j.clear();
	// store row major - convert from column major
	for ( int r = 0; r < 4; ++r ) {
		ga::Json row;
		for ( int c = 0; c < 4; ++c ) {
			row.push_back( m[c][r] );
		}
		j.push_back( row );
	}
}

// ga::Transform
//inline void from_json(const ga::Json& j, ga::Transform& t)
//{
//    ga::Transform _t = t;
//    try {
//        _t.setTranslation(j.at("translation").get<ga::vec3>());
//        _t.setRotation(j.at("rotation").get<ga::quat>());
//        _t.setScale(j.at("scale").get<ga::vec3>());
//    } catch (std::exception& e) {
//        std::stringstream ss;
//        ss << "Error, ga::Transform from Json: " << j.dump() << " - " << e.what();
//        throw std::domain_error(ss.str());
//    }
//    t = _t;
//}
//
//inline void to_json(ga::Json& j, const ga::Transform& t)
//{
//    j.clear();
//    j["translation"] = t.getTranslation();
//    j["rotation"] = t.getRotation();
//    j["scale"] = t.getScale();
//}
}  // namespace ga

// todo: overload glm namespace?
