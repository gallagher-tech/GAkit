#include "ga/graph/node.h"
#include "ga/graph/scene.h"
#include "ga/render.h"

namespace ga {

std::shared_ptr<Node> Node::addChild()
{
	return addChild<Node>();
}

std::shared_ptr<Node> Node::insertChild( size_t index )
{
	return insertChild<Node>( index );
}

void Node::detach()
{
	if ( auto p = m_parent.lock() ) {
		p->removeChild( shared_from_this() );
	}
}

bool Node::removeChild( std::shared_ptr<Node> child )
{
	bool removed = false;
	auto fn      = [&removed, &child]( std::shared_ptr<Node> ref ) {
        if ( ref == child ) {
            removed = true;
            child->setParent( nullptr );
            child->setScene( nullptr );  // resets scene for child and all descendants
            return true;
        }
        return false;
	};
	m_children.erase( std::remove_if( m_children.begin(), m_children.end(), fn ),
	                  m_children.end() );
	return removed;
}

bool Node::removeDescendant( std::shared_ptr<Node> node )
{
	if ( removeChild( node ) )
		return true;                    // check immediate children
	for ( auto& child : m_children ) {  // check hierarchy
		if ( child && child->removeDescendant( node ) )
			return true;
	}
	return false;
}

void Node::clearChildren()
{
	for ( auto& child : m_children ) {
		child->setParent( nullptr );
		child->setScene( nullptr );
	}
	m_children.clear();
}

void Node::sortChildren( std::function<bool( const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b )> comparisonFn )
{
	std::sort( m_children.begin(), m_children.end(), comparisonFn );
}

bool Node::isParentOf( const std::shared_ptr<Node>& child ) const
{
	return std::find( m_children.begin(), m_children.end(), child ) != m_children.end();
}

bool Node::isChildOf( const std::shared_ptr<Node>& parent ) const
{
	return m_parent.lock() == parent;
}

bool Node::hasDescendant( const std::shared_ptr<Node>& node ) const
{
	if ( isParentOf( node ) )
		return true;                    // check immediate children
	for ( auto& child : m_children ) {  // check hierarchy
		if ( child && child->hasDescendant( node ) )
			return true;
	}
	return false;
}

std::shared_ptr<Scene> Node::getScene() const
{
	return m_scene.lock();
}

size_t Node::getSceneHierarchyLevel() const
{
	size_t level = 0;
	auto parent  = getParent();
	while ( parent != nullptr ) {
		++level;
		parent = parent->getParent();
	}
	return level;
}

size_t Node::getSceneDrawIndex() const
{
	if ( m_drawIndex == 0 && getScene() && !isRoot() ) {
		// Draw index is re-assigned every Scene::draw() call.
		// Only Root nodes and Nodes that have not been drawn yet should have drawIndex == 0.
		// Here we assume the Node has not yet been drawn, so we force the scene to re-calc the draw indices.
		getScene()->forceAssignDrawIndices();	
	}
	return m_drawIndex;
}

ga::Transform Node::getSceneTransform()
{
	if ( auto parent = m_parent.lock() ) {
		return parent->getSceneTransform() * getTransform();  //getLocalTransform();
	} else {
		return getTransform();  //getLocalTransform();
	}
}

void Node::disableDraw()
{
	m_isDrawEnabled = false;
}

void Node::enableDraw()
{
	m_isDrawEnabled = true;
}

void Node::disableUpdate()
{
	m_isUpdateEnabled = false;
}

void Node::enableUpdate()
{
	m_isUpdateEnabled = true;
}

std::shared_ptr<Node> Node::getParent() const
{
	return m_parent.lock();
}

const std::vector<std::shared_ptr<Node>>& Node::getChildren() const
{
	return m_children;
}

// --- internal methods

Node::Node()
    : m_uuid( ga::newUuid() )
    , m_updateFn( [this]() { update(); } )
    , m_drawFn( [this]() { draw(); } )
    , m_drawIndex( 0 )
{
}

void Node::setParent( std::shared_ptr<Node> parent )
{
	m_parent = parent;
}

// virtual methods
void Node::setup()
{
}

void Node::update()
{
}

void Node::draw()
{
}

void Node::onDrawIndexChange()
{
}

void Node::setDrawIndex( size_t index )
{
	if ( m_drawIndex != index ) {
		m_drawIndex = index;
		onDrawIndexChange();
	}
}

void Node::updateTree()
{
	if ( !m_isUpdateEnabled )
		return;

	onWillUpdate();

	// update components
	for ( auto& c : m_components ) {
		if ( c.second )
			c.second->update();
	}

	if ( m_updateFn )
		m_updateFn();

	onWillUpdateChildren();

	for ( auto& child : m_children ) {
		child->updateTree();
	}

	onDidUpdateChildren();
	onDidUpdate();
}

void Node::drawTree()
{
	if ( auto scene = m_scene.lock() ) {
		// store draw index for later comparison, etc.
		setDrawIndex( scene->nextDrawIndex( shared_from_this() ) );
	}

	if ( !m_isDrawEnabled )
		return;

	// transform to local space
	getRenderer().pushMatrix();
	getRenderer().multMatrix( Transform::getMatrix() );

	onWillDraw();

	// draw components
	for ( auto& c : m_components ) {
		if ( c.second )
			c.second->draw();
	}

	if ( m_drawFn )
		m_drawFn();

	onWillDrawChildren();

	for ( auto& child : m_children ) {
		child->drawTree();
	}

	onDidDrawChildren();
	onDidDraw();

	getRenderer().popMatrix();
}

void Node::walkTree( std::function<void( std::shared_ptr<Node> )> fn )
{
	if ( fn ) fn( shared_from_this() );

	for ( auto& child : m_children ) {
		child->walkTree( fn );
	}
}

void Node::setScene( std::shared_ptr<Scene> scene )
{
	m_scene = scene;
	// notify components
	for ( auto& c : m_components ) {
		if ( c.second )
			c.second->setScene( scene );
	}
	// set scene for all children and descendants
	for ( auto& child : m_children ) {
		if ( child )
			child->setScene( scene );
	}
}
}  // namespace ga