#ifndef EVENTMANAGER_CPP
#define EVENTMANAGER_CPP
#include "eventManager.hpp"
#include "world.hpp"
//The max number of events returned via getEvents*
const unsigned int EVENT_QUERY_CACHE_SIZE = 10;
//The max number of events of a single type in one pool
const unsigned int EVENT_POOL_SIZE = 20;
//The length of the newCellsToUpdate array
const unsigned int CELLS_TO_UPDATE_SIZE = 20;
//Used to easily iterate through all event types
const EVENT_TYPE EVENT_TYPES[] = {EVENT_TYPE::ATTACK, EVENT_TYPE::ITEM_DROPPED};
const unsigned int NUM_EVENT_TYPES = 2;
EventManager::EventManager(World* newWorld)
{
    world = newWorld;
    //Initialize event maps
    currentEvents = new EventMap;
    newEvents = new EventMap;
    for (unsigned int i = 0; i < NUM_EVENT_TYPES; i++)
    {
        (*currentEvents)[EVENT_TYPES[i]] = new Pool<Event>(EVENT_POOL_SIZE);
        (*newEvents)[EVENT_TYPES[i]] = new Pool<Event>(EVENT_POOL_SIZE);
    }
    //Initialize eventQueryCache
    eventQueryCache = new Event*[EVENT_QUERY_CACHE_SIZE];
    //Initialize newCellsToUpdate list
    newCellsToUpdate = new CellIndex[CELLS_TO_UPDATE_SIZE];
    numCellsToUpdate = 0;
    currentCellsToUpdate = new CellIndex[CELLS_TO_UPDATE_SIZE];
}
EventManager::~EventManager()
{
    delete[] eventQueryCache;
    delete[] newCellsToUpdate;
    delete[] currentCellsToUpdate;
    
    for (EventMap::iterator it = currentEvents->begin(); it != currentEvents->end(); ++it)
    {
        delete it->second;
    }
    for (EventMap::iterator it = newEvents->begin(); it != newEvents->end(); ++it)
    {
        delete it->second;
    }
    delete currentEvents;
    delete newEvents;
}
//Add an event of the specified type
void EventManager::addEvent(EVENT_TYPE eventType, Coord& position, Time* time, float radius, void* trigger)
{
    PoolData<Event>* newPooledEvent = (*newEvents)[eventType]->getNewData();
    if (!newPooledEvent) return;
    Event* newEvent = &newPooledEvent->data;
    newEvent->type = eventType;
    newEvent->position = position;
    newEvent->time = time;
    newEvent->radius = radius;
    newEvent->trigger = trigger;
    
    //Add cells in the event's radius to cellsToUpdate
    Coord topLeft = position;
    topLeft.addVector(-radius, -radius);
    int size;
    CellIndex* affectedCells = world->getIntersectingCells(topLeft, radius * 2, radius * 2, size);
    for (int i = 0; i < size; i++)
    {
        newCellsToUpdate[numCellsToUpdate] = affectedCells[i];
        numCellsToUpdate++;
    }
}
//Returns a pointer to a cached array of Event pointers to the events
//of the requested type. Use size to know how many events are returned.
Event** EventManager::getEventsOfTypeCache(EVENT_TYPE eventType, int& size)
{
    Pool<Event>* currentEventPool = (*currentEvents)[eventType];
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
    for (unsigned int i = 0; i < NUM_EVENT_TYPES; i++)
    {
        Pool<Event>* currentEventPool = (*currentEvents)[EVENT_TYPES[i]];
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
//Makes newEvents currentEvents and wipes the old events. Call this
//function after every world update. Returns a list of CellIndexes
//that need to be updated (they have new events). Use size to iterate
CellIndex* EventManager::update(int& size)
{
    //Swap EventMap buffers
    EventMap* tempMap = currentEvents;
    currentEvents = newEvents;
    for (unsigned int i = 0; i < NUM_EVENT_TYPES; i++)
    {
        (*tempMap)[EVENT_TYPES[i]]->clear();
    }
    newEvents = tempMap;
    
    size = numCellsToUpdate;
    numCellsToUpdate = 0; //Reset numCellsToUpdate for newEvents
    CellIndex* tempCells = currentCellsToUpdate;
    //Swap cellsToUpdate 
    currentCellsToUpdate = newCellsToUpdate;
    newCellsToUpdate = tempCells;
    return currentCellsToUpdate; 
}
#endif

