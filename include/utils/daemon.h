#pragma once
#include <atomic>
#include <functional>
#include <thread>

namespace Clipd::Utils
{
/**
 * @brief A base class for persistent background tasks.
 */
class Daemon
{
public:
    Daemon() : m_is_running( false ) {}
    virtual ~Daemon() = default;

    /**
     * @brief Start the daemon thread in the background.
     *
     * @details The started thread repeatedly calls Daemon::loop() as fast as possible. This is a
     * non-blocking function call.
     */
    virtual void start()
    {
        if( m_is_running )
        {
            return;
        }

        m_is_running = true;
        m_thread = std::thread( [this]() {
            while( this->m_is_running )
            {
                this->loop();
            }
        } );
    }

    /**
     * @brief Stops the background thread.
     *
     * @details Sets the thread loop condition to false, so the thread will terminate once the
     * currently executing Daemon::loop() call terminates.
     */
    virtual void stop()
    {
        m_is_running = false;
    }

    /**
     * @brief Join the wrapped thread and wait for it to terminate.
     */
    void join()
    {
        m_thread.join();
    }

protected:
    /**
     * @brief The background loop body.
     */
    virtual void loop() = 0;

private:
    std::thread m_thread;
    std::atomic<bool> m_is_running;
};
} // namespace Clipd::Utils
