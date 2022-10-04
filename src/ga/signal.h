#pragma once
#include "sigslot/sigslot.hpp"

namespace ga {

template <class... T>
using Signal = sigslot::signal<T...>;

using Connection       = sigslot::connection;
using ScopedConnection = sigslot::scoped_connection;

using Observer = sigslot::observer;
}  // namespace ga