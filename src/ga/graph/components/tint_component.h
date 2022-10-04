#pragma once
#include "ga/color.h"
#include "ga/render.h"
#include "ga/graph/component.h"

namespace ga {

class Tint : public Component
{
public:
	Color color { 1.f };

protected:
	friend class Node;
	void setNode( std::shared_ptr<Node> node ) override
	{
		Component::setNode( node );
		// hook into node's draw cycle
		if ( node ) {
			m_connWillDraw = node->onWillDraw.connect( [this]() {
				m_pGlobalColor = getRenderer().getGlobalColor();
				getRenderer().setGlobalColor( m_pGlobalColor * color );
			} );

			m_connDidDraw = node->onDidDraw.connect( [this]() {
				getRenderer().setGlobalColor( m_pGlobalColor );
			} );
		} else {
			m_connWillDraw.disconnect();
			m_connDidDraw.disconnect();
		}
	}
	ScopedConnection m_connWillDraw, m_connDidDraw;
	Color m_pGlobalColor;
};

}  // namespace ga