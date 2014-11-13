#ifndef NEEDPROCESSOR_HPP
#define NEEDPROCESSOR_HPP
#include <base2.0/ept/eptParser.hpp>
#include "agent.hpp"
#include "need.hpp"
#include "../world/time.hpp"
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

        NeedProcessor(eptFile* spec);
        virtual ~NeedProcessor();
        virtual int updateNeed(Agent* agent, Need* currentNeed, Time* currentTime);
        //Sets the passed need's fields to defaults (specified by defaultNeed)
        void initNeed(Need* currentNeed);
};
#endif