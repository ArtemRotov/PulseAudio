#include <pulse/thread-mainloop.h>
#include <QDebug>
#include "MainLoopLocker.h"

using namespace pulse;

MainLoopLocker::MainLoopLocker(pa_threaded_mainloop* ptr)
    : m_isLocked(false)
    , m_mainLoop(ptr)
{
    if (!m_mainLoop)
        qDebug() << "MainLoopLocker: pointer to main loop is nullptr!";
    else
        lock();
}

MainLoopLocker::~MainLoopLocker()
{
    unlock();
}

void MainLoopLocker::lock()
{
    if (m_mainLoop && !m_isLocked)
    {
        pa_threaded_mainloop_lock(m_mainLoop);
        m_isLocked = true;
    }
    else
        qDebug() << "MainLoopLocker::lock() not done!";
}

void MainLoopLocker::unlock()
{
    if (m_mainLoop && m_isLocked)
    {
        pa_threaded_mainloop_unlock(m_mainLoop);
        m_isLocked = false;
    }
    else
        qDebug() << "MainLoopLocker::unlock() not done!";
}
