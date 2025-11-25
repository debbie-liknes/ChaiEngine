#include <ChaiPhysics/ChaiPhysics.h>

namespace chai
{
    ChaiPhysics::ChaiPhysics() {}

    void ChaiPhysics::init()
    {
        m_thread = std::thread(&ChaiPhysics::run, this);
    }

    ChaiPhysics::~ChaiPhysics()
    {
        if (m_thread.joinable()) {
            m_done = true;
            m_thread.join();
        }
    }

    bool ChaiPhysics::isDone() const
    {
        return m_done;
    }

    void ChaiPhysics::run()
    {
        while (!isDone()) {
            _sleep(1000.0 / 30.0);
        }
    }
}