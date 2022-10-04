#pragma once
#include "ga/graph/component.h"
#include "ga/signal.h"
#include "ga/tween.h"
#include <map>

namespace ga {

// Tween Timeline
class Timeline : public virtual Component
{
public:
	void update() override
	{
		std::vector<std::shared_ptr<TweenBase>> deleteKeys;
		for ( auto& el : m_tweenRefMap ) {
			auto& tween    = el.first;
			auto& updateFn = el.second;
			bool keep      = false;
			if ( tween ) {
				keep = tween->update_();
				if ( updateFn && tween->isStarted_() ) {
					updateFn( tween );
				}
				if ( tween->isDone_() ) {
					tween->end( true );
				}
			}
			if ( !keep ) {
				deleteKeys.push_back( tween );
			}
		}
		bool wasEmpty = m_tweenRefMap.empty();
		for ( auto& key : deleteKeys ) {
			m_tweenRefMap.erase( key );
		}
		bool isEmpty = m_tweenRefMap.empty();
		if ( !wasEmpty && isEmpty ) {
			onTimelineDone( this );
		} else if ( wasEmpty && !isEmpty ) {
			onTimelineStart( this );
		}
	}

	template <typename T, typename Fn>
	void add( std::shared_ptr<Tween<T>> tween, Fn updateFn = nullptr )
	{
		if ( updateFn ) {
			m_tweenRefMap[tween] = [updateFn]( std::shared_ptr<TweenBase> t ) { updateFn( std::static_pointer_cast<Tween<T>>( t )->getValue() ); };
		} else {
			m_tweenRefMap[tween] = nullptr;
		}
	}

	template <typename T>
	void add( std::shared_ptr<Tween<T>> tween, std::function<void( const T& )> updateFn = nullptr )
	{
		if ( updateFn ) {
			m_tweenRefMap[tween] = [updateFn]( std::shared_ptr<TweenBase> t ) { updateFn( std::static_pointer_cast<Tween<T>>( t )->getValue() ); };
		} else {
			m_tweenRefMap[tween] = nullptr;
		}
	}

	template <typename T, typename Fn>
	std::shared_ptr<Tween<T>> add( const T& startVal, const T& endVal, std::function<float( float )> easeFn, Fn updateFn = nullptr, std::function<void()> onDone = nullptr )
	{
		auto tween = std::make_shared<Tween<T>>( startVal, endVal, easeFn, onDone );
		this->add( tween, std::function<void( const T& )>( updateFn ) );
		return tween;
	}

	template <typename T>
	std::shared_ptr<Tween<T>> add( const T& startVal, const T& endVal, std::function<float( float )> easeFn, std::function<void( const T& )> updateFn = nullptr, std::function<void()> onDone = nullptr )
	{
		auto tween = std::make_shared<Tween<T>>( startVal, endVal, easeFn, onDone );
		this->add( tween, updateFn );
		return tween;
	}

	template <typename T>
	bool setTweenUpdate( std::shared_ptr<Tween<T>> tween, std::function<void( const T& )> updateFn )
	{
		try {
			m_tweenRefMap.at( tween ) = [updateFn]( std::shared_ptr<TweenBase> t ) { updateFn( std::static_pointer_cast<Tween<T>>( t )->getValue() ); };
			return true;
		} catch ( ... ) {
			return false;
		}
	}

	void clear()
	{
		m_tweenRefMap.clear();
	}

	std::vector<std::shared_ptr<TweenBase>> getTweens()
	{
		std::vector<std::shared_ptr<TweenBase>> refs;
		refs.reserve( m_tweenRefMap.size() );
		for ( auto& el : m_tweenRefMap ) {
			refs.push_back( el.first );
		}
		return refs;
	}

	bool isActive()
	{
		return m_tweenRefMap.size();
	}

	ga::Signal<Timeline*> onTimelineStart, onTimelineDone;

protected:
	std::map<std::shared_ptr<TweenBase>, std::function<void( std::shared_ptr<TweenBase> )>> m_tweenRefMap;
};
}  // namespace ga