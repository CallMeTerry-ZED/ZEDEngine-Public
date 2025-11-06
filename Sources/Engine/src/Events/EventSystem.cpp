/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Events/EventSystem.h"
#include <utility>

namespace ZED
{
    // Static singleton accessor
    EventSystem& EventSystem::Get()
    {
        static EventSystem instance;
        return instance;
    }

    int EventSystem::Subscribe(EventType type, Handler handler)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        int id = m_NextId++;
        m_Handlers[type].push_back({id, std::move(handler)});
        return id;
    }

    void EventSystem::Unsubscribe(EventType type, int id)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Handlers.find(type);
        if (it != m_Handlers.end())
        {
            auto& vec = it->second;
            vec.erase(std::remove_if(vec.begin(), vec.end(), [id](const Subscription& sub) {
                return sub.id == id;
            }), vec.end());
        }
    }

    void EventSystem::Post(const Event& e)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_EventQueue.push(e);
    }

    void EventSystem::PostDeferred(const Event& e)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_DeferredQueue.push(e);
    }

    void EventSystem::DispatchDeferred()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        while (!m_DeferredQueue.empty())
        {
            m_EventQueue.push(m_DeferredQueue.front());
            m_DeferredQueue.pop();
        }
    }

    void EventSystem::Dispatch()
    {
        std::queue<Event> localQueue;
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            localQueue.swap(m_EventQueue);
        }

        // Process outside the lock to allow posting within handlers
        while (!localQueue.empty())
        {
            const Event e = localQueue.front();
            localQueue.pop();

            std::vector<Subscription> subs;
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                auto it = m_Handlers.find(e.type);
                if (it != m_Handlers.end())
                {
                    subs = it->second;
                }
            }

            for (const auto& sub : subs)
            {
                try {
                    sub.handler(e);
                }
                catch (...) {
                    // Ignore exceptions in handlers
                }
            }
        }
    }
}