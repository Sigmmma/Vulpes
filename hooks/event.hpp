/*
    Original work Copyright (c) 2018 Halogen002. MIT License.
    Modified work Copyright (c) 2019 gbMichelle.

    Modifications:
        -Added function templates and macros for adding events to Event Holders.
         To avoid needing to rewrite the functions each time.
*/
#pragma once

#include <vector>

enum EventPriority {
    /// These events are called before the default priority events are called.
    EVENT_PRIORITY_BEFORE,

    /// This is the default priority where most events are called.
    EVENT_PRIORITY_DEFAULT,

    /// These events are called after the default priority. This priority gets the last say.
    EVENT_PRIORITY_AFTER,

    /// These events are called last. This is used for monitoring values. Do not change any of the parameters in this.
    EVENT_PRIORITY_FINAL
};

/// This is a function typename that has no arguments and returns nothing.
typedef void (*event_no_args)();

/// This struct is used for events and uses a priority and function pointer.
template <class T>
struct Event {
    /// This is the function pointer used by the event.
    T function = nullptr;

    /// This is the priority of the event.
    EventPriority priority = EVENT_PRIORITY_DEFAULT;

    Event() {}
    Event(T new_function, EventPriority new_priority = EVENT_PRIORITY_DEFAULT) : function(new_function), priority(new_priority) {}
    Event(const Event &copy) : function(copy.function), priority(copy.priority) {}
};

/// This function calls events in order.
///
/// Functions in the same priority are called based on their order in the events vector.
template<typename T, typename ... Args>
static inline void call_in_order(std::vector<Event<T>> events, Args&& ... args) {
    auto count = events.size();
    for(typename std::vector<Event<T>>::size_type i=0;i<count;i++) {
        if(events[i].priority == EVENT_PRIORITY_BEFORE) events[i].function(std::forward<Args>(args) ...);
    }
    for(typename std::vector<Event<T>>::size_type i=0;i<count;i++) {
        if(events[i].priority == EVENT_PRIORITY_DEFAULT) events[i].function(std::forward<Args>(args) ...);
    }
    for(typename std::vector<Event<T>>::size_type i=0;i<count;i++) {
        if(events[i].priority == EVENT_PRIORITY_AFTER) events[i].function(std::forward<Args>(args) ...);
    }
    for(typename std::vector<Event<T>>::size_type i=0;i<count;i++) {
        if(events[i].priority == EVENT_PRIORITY_FINAL) events[i].function(std::forward<Args>(args) ...);
    }
}

/// This function calls events in order, but the event can be denied by any function. Denying will prevent other events from firing.
///
/// Functions in the same priority are called based on their order in the events vector.
template<typename T, typename ... Args>
static inline void call_in_order_allow(std::vector<Event<T>> events, bool &allow, Args&& ... args) {
    auto count = events.size();
    for(typename std::vector<Event<T>>::size_type i=0;i<count && allow;i++) {
        if(events[i].priority == EVENT_PRIORITY_BEFORE) allow = events[i].function(std::forward<Args>(args) ...);
    }
    for(typename std::vector<Event<T>>::size_type i=0;i<count && allow;i++) {
        if(events[i].priority == EVENT_PRIORITY_DEFAULT) allow = events[i].function(std::forward<Args>(args) ...);
    }
    for(typename std::vector<Event<T>>::size_type i=0;i<count && allow;i++) {
        if(events[i].priority == EVENT_PRIORITY_AFTER) allow = events[i].function(std::forward<Args>(args) ...);
    }
    for(typename std::vector<Event<T>>::size_type i=0;i<count && allow;i++) {
        if(events[i].priority == EVENT_PRIORITY_FINAL) allow = events[i].function(std::forward<Args>(args) ...);
    }
}

/// The general style of Kava's event add and remove functions reworked into
/// templates so that they don't need to be rewritten each time.

template<typename EventsHolder, typename EventFunc>
static inline void add_event(EventsHolder e_hook(), EventFunc event_function, EventPriority priority = EVENT_PRIORITY_DEFAULT) {
    EventsHolder events = e_hook();
    for(size_t i=0;i<events->size();i++) {
        if((*events)[i].function == event_function) return;
    }
    events->emplace_back(event_function, priority);
}

template<typename EventsHolder, typename EventFunc>
static inline void del_event(EventsHolder e_hook(), EventFunc &event_function) {
    EventsHolder events = e_hook();
    for(size_t i=0;i<events->size();i++) {
        if((*events)[i].function == event_function) {
            events->erase(events->begin() + i);
            return;
        }
    }
}

#define ADD_EVENT_P(e_hook, event_func, priority) add_event<e_hook>(e_hook ## _list, event_func, priority)
#define ADD_EVENT(e_hook, event_func) add_event(e_hook ## _list, event_func)
#define DEL_EVENT(e_hook, event_func) del_event(e_hook ## _list, event_func)
