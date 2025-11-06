/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#pragma once

#include "Event.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>

namespace ZED
{
    /**
     * EventSystem is a lightweight publish/subscribe dispatcher designed
     * to decouple engine modules.  Modules can post events without
     * knowledge of who will handle them and subscribe only to the
     * event types they care about.  Events can be dispatched
     * immediately or deferred for later processing.
     */
    class ZEDENGINE_API EventSystem
    {
    public:
        using Handler = std::function<void(const Event&)>;

        /**
         * Returns the singleton instance of the EventSystem.
         */
        static EventSystem& Get();

        /**
         * Subscribe to a specific event type.  Returns a subscription ID
         * that can be used to unsubscribe later.  Handlers will be
         * invoked in the order they were added.
         */
        int Subscribe(EventType type, Handler handler);

        /**
         * Unsubscribe a previously registered handler for a specific event type.
         */
        void Unsubscribe(EventType type, int id);

        /**
         * Post an event for immediate dispatch.  The event will be
         * delivered on the next call to Dispatch().
         */
        void Post(const Event& e);

        /**
         * Post an event to the deferred queue.  Deferred events are
         * transferred to the immediate queue on the next call to
         * DispatchDeferred() and then delivered during Dispatch().
         */
        void PostDeferred(const Event& e);

        /**
         * Move deferred events into the immediate queue.  Call this
         * once per frame before calling Dispatch() if you want to
         * process deferred events.
         */
        void DispatchDeferred();

        /**
         * Dispatch all events currently in the immediate queue.
         */
        void Dispatch();

    private:
        // Internal representation of a subscription entry
        struct Subscription
        {
            int id;
            Handler handler;
        };

        // Protects access to the handler maps and queues
        std::mutex m_Mutex;
        int m_NextId = 1;
        std::unordered_map<EventType, std::vector<Subscription>> m_Handlers;
        std::queue<Event> m_EventQueue;
        std::queue<Event> m_DeferredQueue;

        // Private constructor for singleton pattern
        EventSystem() = default;
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
    };
}

#endif // EVENTSYSTEM_H