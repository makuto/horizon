#ifndef USEITEM_PROCESS_HPP
#define USEITEM_PROCESS_HPP
#include <vector>
#include "../process.hpp"
#include "../../world/time.hpp"
//#include "agent.hpp" //Forward declared
#include "../need.hpp"

#include "../../object/objectManager.hpp"
#include "../../object/object.hpp"

#include "../../item/itemDatabase.hpp"
#include "../../item/itemManager.hpp"
/* --UseItemProcess--
 * UseItemProcess searches the agent's inventory for a consumable item
 * that will raise the requested need. It does this by looking up each
 * item in the ItemDatabase and seeing if the requested need is in
 * the consumable's needBenefits. If there is no beneficial need, difficulty
 * is impossible.
 *
 * UseItemProcess can be used in a chain; if it is not the first process,
 * it will trust that previous processes acquire beneficial items.
 *
 * UseItem currently works only with consumables (type 1)
 * */
struct Agent;


class UseItemProcess:public Process
{
    private:
        int counter;
        ItemDatabase* itemDB;
        ItemManager* itemManager;
    public:
        virtual ~UseItemProcess();
        
        void setup(ItemDatabase* newItemDB, ItemManager* newItemManager);
        //Return the difficulty of the process, or -1 if impossible. index
        //contains the index of the process in its chain
        virtual int getDifficulty(Agent* agent, Need* need, int index);
        //Update should return 0 if process didn't complete (run next frame)
        //1 if process is ready to go to the next process in chain
        //-1 if process chain should end immediately
        virtual int update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime);
};
#endif
