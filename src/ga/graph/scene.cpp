#include "ga/graph/scene.h"

namespace ga {

Scene::Scene()
    : m_rootNode( Node::create() )
{
}

Scene::~Scene()
{
}

// protected method, called by Scene:create<SceneT>()
void Scene::setup()
{
	m_rootNode->setName( "root" );
	m_rootNode->setScene( shared_from_this() );
}

void Scene::update()
{
	m_timeoutManager.updateTimeouts();
	updateNodes();
}

void Scene::draw()
{
	drawNodes();
}

std::shared_ptr<ga::Node> Scene::addNode()
{
	return addNode<ga::Node>();
}

bool Scene::removeNode( std::shared_ptr<Node> node )
{
	return m_rootNode->removeChild( node );
}

void Scene::clearNodes()
{
	m_rootNode->clearChildren();
}

bool Scene::hasNode( std::shared_ptr<Node> node )
{
	return m_rootNode->hasDescendant( node );
}

std::shared_ptr<Node> Scene::getRootNode()
{
	return m_rootNode;
}

std::vector<std::shared_ptr<Node>> Scene::getNodes()
{
	return m_rootNode->getChildren();
}

void Scene::forEachNode( std::function<void( std::shared_ptr<Node> )> fn )
{
	if ( m_rootNode ) {
		m_rootNode->walkTree( fn );
	}
}

void Scene::setName( const std::string& name )
{
	m_name = name;
}

const std::string& Scene::getName()
{
	return m_name;
}

void Scene::handleMouseEvent( MouseEvent& mouseEvent )
{
	// trigger signal
	onMouseEvent( mouseEvent );
}

void Scene::handleTouchEvent( TouchEvent& touchEvent )
{
	// trigger signal
	onTouchEvent( touchEvent );
}

void Scene::handleKeyEvent( KeyEvent& keyEvent )
{
	// trigger signal
	onKeyEvent( keyEvent );
}

// --- protected - internal

void Scene::updateNodes()
{
	if ( m_rootNode )
		m_rootNode->updateTree();
}

void Scene::drawNodes()
{
	m_drawnNodes.clear();
	if ( m_rootNode )
		m_rootNode->drawTree();
}

size_t Scene::nextDrawIndex( std::shared_ptr<Node> node )
{
	if ( !node )
		return 0;
	m_drawnNodes.push_back( node );
	return m_drawnNodes.size() - 1;
}

void Scene::forceAssignDrawIndices()
{
	m_drawnNodes.clear();
	auto fn = [this]( std::shared_ptr<Node> node ) {
		if ( node ) {
			auto index = nextDrawIndex( node );
			node->setDrawIndex( index );
		}
	};
	forEachNode( fn );
}

}  // namespace ga