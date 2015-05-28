#ifndef EVENTMANAGER_CPP
#define EVENTMANAGER_CPP
#include "eventManager.hpp"
//The max number of events returned via getEvents*
const unsigned int EVENT_QUERY_CACHE_SIZE = 10;
//The max number of events of a single type in one pool
const unsigned int EVENT_POOL_SIZE = 20;
EventManager::EventManager()
{
    events[EVENT_TYPE::ATTACK] = new Pool<Event>(EVENT_POOL_SIZE);
    events[EVENT_TYPE::ITEM_DROPPED] = new Pool<Event>(EVENT_POOL_SIZE);
    eventQueryCache = new Event*[EVENT_QUERY_CACHE_SIZE];
}
EventManager::~EventManager()
{
    delete[] eventQueryCache;
    for (EventMap::iterator it = events.begin(); it != events.end(); ++it)
    {
        delete it->second;
    }
}
//Add an event of the specified type
void EventManager::addEvent(EVENT_TYPE eventType, Coord& position, Time* time, float radius, void* trigger)
{
    PoolData<Event>* newPooledEvent = events[eventType]->getNewData();
    Event* newEvent = &newPooledEvent->data;
    newEvent->type = eventType;
    newEvent->position = position;
    newEvent->time = time;
    newEvent->radius = radius;
    newEvent->trigger = trigger;
}
//Returns a pointer to a cached array of Event pointers to the events
//of the requested type. Use size to know how many events are returned.
Event** EventManager::getEventsOfTypeCache(EVENT_TYPE eventType, int& size)
{
    Pool<Event>* currentEventPool = events[eventType];
    PoolData<Event>* currentEvent = currentEventPool->getFirstActiveData();
    unsigned int counter = 0;
    while (currentEvent && counter < EVENT_QUERY_CACHE_SIZE)
    {
        eventQueryCache[counter] = &currentEvent->data;
        currentEvent = currentEventPool->getNextActiveData(currentEvent);
        counter++;
    }
    size = counter;
    return eventQueryCache;
}
//Returns a pointer to a cached array of Event pointers to the events
//within the provided range (manhattan). Use size to know how many events are returned.
Event** EventManager::getEventsInRangeCache(Coord& position, float radius, int& size)
{
    unsigned int counter = 0;
    EVENT_TYPE eventTypes[] = {EVENT_TYPE::ATTACK, EVENT_TYPE::ITEM_DROPPED};
    for (int i = 0; i < 2; i++)
    {
        Pool<Event>* currentEventPool = events[eventTypes[i]];
        PoolData<Event>* currentEvent = currentEventPool->getFirstActiveData();
        while (currentEvent && counter < EVENT_QUERY_CACHE_SIZE)
        {
            float manhattanDistance = position.getManhattanTo(currentEvent->data.position);
            if (manhattanDistance < radius + currentEvent->data.radius)
            {
                eventQueryCache[counter] = &currentEvent->data;
                counter++;
            }
            currentEvent = currentEventPool->getNextActiveData(currentEvent);
        }
    }
    size = counter;
    return eventQueryCache;
}
#endif

