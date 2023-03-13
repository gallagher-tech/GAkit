#pragma once
#include "ga/defines.h"
#include "ga/events.h"
#include "ga/graph/node.h"
#include "ga/signal.h"
#include "ga/timeout.h"
#include <algorithm>
#include <map>
#include <memory>
#include <vector>

namespace ga {

/**
 * @brief Scene is a view controller.
 *	- contains a single root node, which holds the scene's node / view hierarchy.
 * 	- handles UI events (mouse, touch, etc.)
 */
class Scene : public std::enable_shared_from_this<Scene>
{
public:
	// Static Scene factory
	// in place of public constructor
	template <CLASS_INHERITS( SceneT, ga::Scene )>
	static std::shared_ptr<SceneT> create();
	static std::shared_ptr<Scene> create() { return Scene::create<Scene>(); }

	virtual ~Scene();

	virtual void setup();
	virtual void update();
	virtual void draw();

	template <CLASS_INHERITS( NodeT, ga::Node )>
	std::shared_ptr<NodeT> addNode( std::shared_ptr<NodeT> child = nullptr );
	std::shared_ptr<ga::Node> addNode();

	bool removeNode( std::shared_ptr<Node> node );
	void clearNodes();

	bool hasNode( std::shared_ptr<Node> node );

	std::shared_ptr<Node> getRootNode();
	std::vector<std::shared_ptr<Node>> getNodes();
	void forEachNode( std::function<void( std::shared_ptr<Node> )> fn );

	void setName( const std::string& name );
	const std::string& getName();

	virtual void handleMouseEvent( MouseEvent& mouseEvent );
	virtual void handleTouchEvent( TouchEvent& touchEvent );
	virtual void handleKeyEvent( KeyEvent& keyEvent );

	// signals
	Signal<KeyEvent&> onKeyEvent;
	Signal<MouseEvent&> onMouseEvent;
	Signal<TouchEvent&> onTouchEvent;

protected:
	Scene();

	void updateNodes();  // update root node and all children
	void drawNodes();    // draw root node and all children

	friend void Node::setScene( std::shared_ptr<Scene> scene );
	//void addToHierarchy( std::shared_ptr<Node> node );
	//void removeFromHierarchy( std::shared_ptr<Node> node );

	friend void Node::drawTree();
	size_t nextDrawIndex( std::shared_ptr<Node> node );

	friend size_t Node::getSceneDrawIndex() const;
	void forceAssignDrawIndices();

	std::string m_name;
	std::shared_ptr<Node> m_rootNode;
	ga::TimeoutManager m_timeoutManager;

	std::vector<std::weak_ptr<Node>> m_drawnNodes;  // sorted by draw order
};

// template implementations
// ------------------------

template <class SceneT, typename>
std::shared_ptr<SceneT> Scene::create()
{
	auto scene = std::shared_ptr<SceneT>( new SceneT() );
	scene->setup();
	return scene;
}

template <class NodeT, typename>
std::shared_ptr<NodeT> Scene::addNode( std::shared_ptr<NodeT> node )
{
	return m_rootNode->addChild( node );
}
}  // namespace ga
