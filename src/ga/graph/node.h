/**
 * @file node.h
 * @author Tyler Henry, Gallagher & Associates
 * @brief 
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include "ga/defines.h"
#include "ga/graph/component.h"
#include "ga/transform.h"
#include "ga/uuid.h"
#include "ga/signal.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace ga {

class Scene;

/**
 * @brief Node represents a basic "node" (or view) in the scenegraph.
 * 
 * Nodes keep:
 *  - name (std::string)
 *  - unique id
 *  - local Transform
 *  - Component list
 *  - reference to a Scene owner
 *  - reference to a parent Node
 *  - vector of references to child Nodes
 * 
 */
class Node : public std::enable_shared_from_this<Node>, public Transform
{
public:
	// Static Node factory
	// in place of public constructor
	template <CLASS_INHERITS( NodeT, ga::Node )>
	static std::shared_ptr<NodeT> create();
	static std::shared_ptr<Node> create() { return Node::create<Node>(); }

	// parent / child hierarchy
	// ------------------------

	template <CLASS_INHERITS( NodeT, ga::Node )>
	std::shared_ptr<NodeT> addChild( std::shared_ptr<NodeT> child = nullptr );
	std::shared_ptr<Node> addChild();

	template <CLASS_INHERITS( NodeT, ga::Node )>
	std::shared_ptr<NodeT> insertChild( size_t index, std::shared_ptr<NodeT> child = nullptr );
	std::shared_ptr<Node> insertChild( size_t index );

	void detach();  // remove this node and its children from its parent and scene

	bool removeChild( std::shared_ptr<Node> child );      // returns true if found / removed
	bool removeDescendant( std::shared_ptr<Node> node );  // searches entire hierarchy for Node and removes
	void clearChildren();                                 // remove all children

	void sortChildren( std::function<bool( const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b )> comparisonFn );

	bool isParentOf( const std::shared_ptr<Node>& child ) const;
	bool isChildOf( const std::shared_ptr<Node>& parent ) const;
	inline bool hasChild( const std::shared_ptr<Node>& child ) const { return isParentOf( child ); }
	bool hasDescendant( const std::shared_ptr<Node>& node ) const;

	std::shared_ptr<Node> getParent() const;
	const std::vector<std::shared_ptr<Node>>& getChildren() const;

	inline bool hasChildren() const { return !m_children.empty(); }
	inline bool hasParent() const { return !m_parent.expired(); }
	inline bool isRoot() const { return !hasParent(); }  // root == no parent

	inline bool hasScene() const { return !m_scene.expired(); }
	std::shared_ptr<Scene> getScene() const;

	size_t getSceneHierarchyLevel() const;  // 0 == root, 1 == child of root, 2 == grandchild, etc.
	size_t getSceneDrawIndex() const;

	// setters / getters

	void setName( const std::string& name ) { m_name = name; }
	const std::string& getName() const { return m_name; }

	void setUuid( const ga::Uuid& uuid ) { m_uuid = uuid; }
	const ga::Uuid& getUuid() { return m_uuid; }

	ga::Transform& getTransform() { return *this; }

	// custom draw / update functions

	void setDrawFn( std::function<void()> drawFn ) { m_drawFn = drawFn; }
	void resetDrawFn()
	{
		m_drawFn = [this]() { draw(); };
	}

	void setUpdateFn( std::function<void()> updateFn ) { m_updateFn = updateFn; }
	void resetUpdateFn()
	{
		m_updateFn = [this]() { update(); };
	}

	// scene space transformations

	// the global / scene space transformation of this node
	ga::Transform getSceneTransform();

	// helper to convert a local position to scene space
	vec3 localPosToScene( const vec3& pos ) { return getSceneTransform().getMatrix() * vec4( pos, 1. ); }
	// helper to convert a scene position to local space
	vec3 scenePosToLocal( const vec3& pos ) { return glm::inverse( getSceneTransform().getMatrix() ) * vec4( pos, 1. ); }

	// enable / disable node

	void disableUpdate();  // scene WON'T update() this Node (or its children)
	void enableUpdate();   // scene WILL update() this Node (and its children)

	bool isUpdateEnabled() const { return m_isUpdateEnabled; }

	void disableDraw();  // scene WON'T draw() this Node (or its children)
	void enableDraw();   // scene WILL draw() this Node (and its children)

	bool isDrawEnabled() const { return m_isDrawEnabled; }

	// /// TODO - these are temporary methods for testing. should be replaced with iostream overloads
	// void printChildren();
	// void printParent();
	// void printHierarchy();
	// void printScene();

	// components

	template <CLASS_INHERITS( ComponentT, Component )>
	std::shared_ptr<ComponentT> createComponent();

	template <CLASS_INHERITS( ComponentT, Component )>
	std::shared_ptr<ComponentT> addComponent( std::shared_ptr<ComponentT> component );

	template <CLASS_INHERITS( ComponentT, Component )>
	std::shared_ptr<ComponentT> getComponent();

	template <CLASS_INHERITS( ComponentT, Component )>
	bool removeComponent();

	template <CLASS_INHERITS( ComponentT, Component )>
	ComponentT& component();

	// signals

	ga::Signal<>
	    onWillUpdate, onDidUpdate,
	    onWillUpdateChildren, onDidUpdateChildren,
	    onWillDraw, onDidDraw,
	    onWillDrawChildren, onDidDrawChildren;

	virtual ~Node() {}

protected:
	Node();  // Nodes should only be created by Node::create()
	virtual void setup();
	virtual void update();
	virtual void draw();

	// useful for things that deal with draw order, i.e. events
	virtual void onDrawIndexChange();
	void setDrawIndex( size_t index );

	friend class Scene;

	void setScene( std::shared_ptr<Scene> scene );
	void setParent( std::shared_ptr<Node> parent );

	// update and draw hierarchy
	void updateTree();
	void drawTree();

	void walkTree( std::function<void( std::shared_ptr<Node> )> fn );  // run arbitrary function on self and children

	std::weak_ptr<Scene> m_scene;
	std::weak_ptr<Node> m_parent;
	std::vector<std::shared_ptr<Node>> m_children;

	// components
	std::unordered_map<std::type_index, std::shared_ptr<Component>> m_components;

	// built-in properties
	std::string m_name;
	ga::Uuid m_uuid;
	size_t m_drawIndex;
	std::function<void()> m_updateFn;
	std::function<void()> m_drawFn;
	// std::shared_ptr<Mesh> m_mesh;
	// std::shared_ptr<Matrial> m_material;

	bool m_isDrawEnabled   = true;
	bool m_isUpdateEnabled = true;
};

// template implementations
// ---------------------------------------------

// ---------------------------------------------
template <class NodeT, typename>
std::shared_ptr<NodeT> Node::create()
{
	auto node = std::shared_ptr<NodeT>( new NodeT() );
	node->setup();
	return node;
}

// ---------------------------------------------
template <class NodeT, typename>
std::shared_ptr<NodeT> Node::addChild( std::shared_ptr<NodeT> child )
{
	if ( !child ) {
		child = create<NodeT>();
	}
	if ( !isParentOf( child ) ) {
		child->detach();
		m_children.push_back( child );
		child->setParent( shared_from_this() );
		child->setScene( m_scene.lock() );
	}
	return child;
}

// ---------------------------------------------
template <class NodeT, typename>
std::shared_ptr<NodeT> Node::insertChild( size_t index, std::shared_ptr<NodeT> child )
{
	if ( !child ) {
		child = create<NodeT>();
	}
	if ( !isParentOf( child ) ) {
		child->detach();
		index = std::min( index, m_children.size() );
		m_children.emplace( m_children.begin() + index, child );
		child->setParent( shared_from_this() );
		child->setScene( m_scene.lock() );
	}
	return child;
}

// ---------------------------------------------
template <class ComponentT, typename>
std::shared_ptr<ComponentT> Node::createComponent()
{
	return addComponent( std::shared_ptr<ComponentT>( new ComponentT() ) );
}

// ---------------------------------------------
template <class ComponentT, typename>
std::shared_ptr<ComponentT> Node::addComponent( std::shared_ptr<ComponentT> componentPtr )
{
	if ( !componentPtr )
		componentPtr = std::shared_ptr<ComponentT>( new ComponentT() );
	auto p = std::make_pair( std::type_index( typeid( ComponentT ) ), componentPtr );
	auto r = m_components.insert( p );  // returns <it, bool>
	if ( r.second ) {
		componentPtr->setNode( shared_from_this() );
	}
	return r.second ? componentPtr : nullptr;
}

// --------------------------------------------
template <class ComponentT, typename>
std::shared_ptr<ComponentT> Node::getComponent()
{
	auto it = m_components.find( std::type_index( typeid( ComponentT ) ) );
	if ( it != m_components.end() ) {
		return std::dynamic_pointer_cast<ComponentT>( it->second );
	} else {
		return nullptr;
	}
}

// --------------------------------------------
// auto creates component, if not found or null
template <class ComponentT, typename>
ComponentT& Node::component()
{
	auto it = m_components.find( std::type_index( typeid( ComponentT ) ) );
	std::shared_ptr<ComponentT> component;
	if ( it != m_components.end() ) {
		if ( it->second ) {
			component = std::dynamic_pointer_cast<ComponentT>( it->second );
		} else {
			// component null, remove before re-create
			removeComponent<ComponentT>();
		}
	}
	if ( !component ) {
		component = createComponent<ComponentT>();
		if ( !component ) {
			throw std::runtime_error( "Error creating Component of type: " + std::string( typeid( ComponentT ).name() ) );
		}
	}
	return *component;
}

// ---------------------------------------------
template <class ComponentT, typename>
bool Node::removeComponent()
{
	auto it = m_components.find( typeid( ComponentT ) );
	if ( it != m_components.end() ) {
		//if (it->second) {
		//    it->second->m_components = nullptr;
		//}
		m_components.erase( it );
		return true;
	} else {
		return false;
	}
}

}  // namespace ga
