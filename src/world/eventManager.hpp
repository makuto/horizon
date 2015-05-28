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
 * */
enum EVENT_TYPE: int
{
    ATTACK = 0,
    ITEM_DROPPED
    //NOTE: Remember to modify getEventsInRangeCache and EventManager constructcor
    //when adding new event types (no way to iterate through enums)
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
class EventManager
{
    private:
        EventMap events;
        Event** eventQueryCache;
    public:
        EventManager();
        ~EventManager();
        //Add an event of the specified type (NOTE: Manhattan radius)
        void addEvent(EVENT_TYPE eventType, Coord& position, Time* time, float radius, void* trigger);
        //Returns a pointer to a cached array of Event pointers to the events
        //of the requested type. Use size to know how many events are returned.
        Event** getEventsOfTypeCache(EVENT_TYPE eventType, int& size);
        //Returns a pointer to a cached array of Event pointers to the events
        //within the provided radius (manhattan). Use size to know how
        //many events are returned.
        Event** getEventsInRangeCache(Coord& position, float radius, int& size);
};
#endif
