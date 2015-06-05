#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP
#include "../utilities/pool.hpp"
#include "coord.hpp"
#include "time.hpp"
#include <map>
/* --EventManager--
 * EventManager organizes short-term location-/radius-based events at the
 * World level (no organization by cells - organized by types). Listeners
 * are in charge of querying the events themselves (as opposed to dispatchers)
 *
 * EventManager holds two maps and pools of events - currentEvents and newEvents.
 * When an event is first posted, it is put in newEvents. Once the next frame
 * update starts, currentEvents and newEvents are flipped. Events only persist
 * for one update cycle, but EventManager ensures that any cells that would
 * need to know about the event know about it. All events are delayed by
 * one frame.
 * */
enum EVENT_TYPE: int
{
    ATTACK = 0,
    ITEM_DROPPED
    //NOTE: Remember to modify EVENT_TYPES and NUM_EVENT_TYPES when adding new types
};
struct Event
{
    EVENT_TYPE type;    //The Event Type
    Coord position;     //The position of the event
    Time* time;         //When the event occurred
    float radius;       //The effective radius of the event (manhattan distance)
    void* trigger;      //A pointer to the thing that triggered the
                        //Event. This is assumed to be a static_cast pointer
};

typedef std::map<EVENT_TYPE, Pool<Event>*> EventMap;

class World;
class EventManager
{
    private:
        EventMap* currentEvents;
        EventMap* newEvents;
        Event** eventQueryCache;
        //Holds all cells affected by newEvents (in their radius).
        CellIndex* newCellsToUpdate;
        int numCellsToUpdate;
        //Holds the array of cells currently updating
        CellIndex* currentCellsToUpdate;
        //Needed for getIntersectingCells
        World* world;
    public:
        EventManager(World* newWorld);
        ~EventManager();
        //Add an event of the specified type (NOTE: Manhattan radius).
        //Adding an event also adds CellIndex entries into newCellsToUpdate
        //in order to ensure cells with events are updated at the next step
        void addEvent(EVENT_TYPE eventType, Coord& position, Time* time, float radius, void* trigger);
        //Returns a pointer to a cached array of Event pointers to the events
        //of the requested type. Use size to know how many events are returned.
        Event** getEventsOfTypeCache(EVENT_TYPE eventType, int& size);
        //Returns a pointer to a cached array of Event pointers to the events
        //within the provided radius (manhattan). Use size to know how
        //many events are returned.
        Event** getEventsInRangeCache(Coord& position, float radius, int& size);
        //Makes newEvents currentEvents and wipes the old events. Call this
        //function after every world update. Returns a list of CellIndexes
        //that need to be updated (they have new events). Use size to iterate
        CellIndex* update(int& size);
};
#endif
