#ifndef HUNGER_NEEDPROCESSOR_HPP
#define HUNGER_NEEDPROCESSOR_HPP
#include <map>
#include <base2.0/ept/eptParser.hpp>
#include "../needProcessor.hpp"
#include "../agent.hpp"
#include "../need.hpp"
#include "../../world/time.hpp"

#include "../../object/objectManager.hpp"
#include "../../object/object.hpp"

/* --HungerNeedProcessor--
 * HungerNeedProcessor performs all the logic for a Hunger need.
 * */
class HungerNeedProcessor:public NeedProcessor
{
    private:
        Need defaultNeed;
    public:
        int needID;

        HungerNeedProcessor();
        virtual ~HungerNeedProcessor();
        virtual bool initialize(eptFile* spec);
        //deltaTime should be the time since this need was last updated
        virtual int updateNeed(Agent* agent, Object* obj, ObjectManager* objectManager, Need* currentNeed, Time* deltaTime);
        //Sets the passed need's fields to defaults (specified by defaultNeed)
        virtual void initNeed(Need* currentNeed);
};

#endif
