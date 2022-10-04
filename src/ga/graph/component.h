#pragma once
#include "ga/defines.h"
#include <memory>

namespace ga {

class Node;
class Scene;

class Component
{
public:
	Component()          = default;
	virtual ~Component() = default;

	Component( const Component& other )
	{
		// do not copy m_node
	}

	Component& operator=(const Component& other)
	{
		// do not copy m_node
		return *this;
	}

	std::shared_ptr<Node> getNode() const
	{
		return m_node.lock();
	}

protected:
	friend class Node;

	// called by Node
	virtual void update() {}
	virtual void draw() {}
	virtual void setNode( std::shared_ptr<Node> node )
	{
		m_node = node;
	}
	virtual void setScene( std::shared_ptr<Scene> scene ) {}

	std::weak_ptr<Node> m_node;  // node owner
};

}  // namespace ga