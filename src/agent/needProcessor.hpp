#ifndef NEEDPROCESSOR_HPP
#define NEEDPROCESSOR_HPP
#include <map>
#include <base2.0/ept/eptParser.hpp>
#include "agent.hpp"
#include "need.hpp"
#include "../world/time.hpp"

#include "../object/objectManager.hpp"
#include "../object/object.hpp"
/* --NeedProcessor--
 * NeedProcessors perform all the logic for a type of need.
 * For example, a simple Hunger NeedProcessor would subtract 10 from the need
 * every 30 seconds in updateNeed(), then when the need reaches its detriment
 * threshold, idk (TODO)
 *
 * This class is designed to be overloaded to do any custom need logic
 * */
class NeedProcessor
{
    private:
        Need defaultNeed;
    public:
        int needID;
        NeedProcessor();
        virtual ~NeedProcessor();
        //Use this function to set defaults for this need (you must call this!)
        virtual bool initialize(eptFile* spec);
        //deltaTime should be the time since this need was last updated
        virtual int updateNeed(Agent* agent, Object* obj, ObjectManager* objectManager, Need* currentNeed, Time* deltaTime);
        //Sets the passed need's fields to defaults (specified by defaultNeed)
        virtual void initNeed(Need* currentNeed);
};

//A map of needProcessors with needIDs meant to be their keys
typedef std::map<int, NeedProcessor*> NeedProcessorDir;

#endif
