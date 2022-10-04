#pragma once
#include "crossguid/crossguid.hpp"

// universally unique identifiers (aka guid - globally unique id)
// https://en.wikipedia.org/wiki/Universally_unique_identifier
// 128-bit ids, in string hex format: "7bcd757f-5b10-4f9b-af69-1a1f226f3b3e"

namespace ga {

using Uuid = xg::Guid;  // cross-platform uuid class
inline Uuid newUuid()
{
	return xg::newGuid();
}
}  // namespace ga