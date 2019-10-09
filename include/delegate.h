#pragma once
#include "functor.h"

#include <list>
#include <mutex>

template <typename T>
class Delegate;

/**
 * @brief A multicast delegate object.
 *
 * @details Subscribes and unsubscribes functors, and calls them with the provided arguments.
 *
 * @tparam Ret_t The stored functor return type.
 * @tparam Args_t The stored functor argument types.
 */
template <class Ret_t, class... Args_t>
class Delegate<Ret_t( Args_t... )>
{
public:
    /**
     * @brief Add the given functor to the delegate.
     */
    void subscribe( Functor<Ret_t( Args_t... )> functor )
    {
        std::unique_lock<std::mutex> lock( m_functors_mutex );
        m_functors.push_back( functor );
    }

    /**
     * @brief Remove the given functor from the delegate, if it exists.
     */
    void unsubscribe( Functor<Ret_t( Args_t... )> functor )
    {
        std::unique_lock<std::mutex> lock( m_functors_mutex );
        m_functors.remove( functor );
    }

    /**
     * @brief Call all of the contained functors with the given arguments.
     */
    void operator()( Args_t... args )
    {
        std::unique_lock<std::mutex> lock( m_functors_mutex );
        for( const auto& functor : m_functors )
        {
            functor( args... );
        }
    }

private:
    std::list<Functor<Ret_t( Args_t... )>> m_functors;
    std::mutex m_functors_mutex;
};
