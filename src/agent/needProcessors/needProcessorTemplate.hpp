#ifndef $T_NEEDPROCESSOR_HPP
#define $T_NEEDPROCESSOR_HPP
#include <map>
#include <base2.0/ept/eptParser.hpp>
#include "../../needProcessor.hpp"
#include "../agent.hpp"
#include "../need.hpp"
#include "../../world/time.hpp"

#include "../../object/objectManager.hpp"
#include "../../object/object.hpp"

/*To use template:
 * In both .cpp and .hpp:
 *  Replace $T with CAPS name (e.g $T -> HUNGER)
 *  Replace $t with Proper name (e.g $t -> Hunger)
 *  Replace $l with lower name (e.g. $l -> hunger)
 *  In .cpp, change the include "CHANGEME.hpp" to the name of this .hpp
 * Cut the text below into the makefile
        under needProcessor:
$(OBJ_DIR)/$lNeedProcessor.o: src/agent/needProcessors/$lNeedProcessor.hpp src/agent/needProcessors/$lNeedProcessor.cpp
	$(FLAGS) src/agent/needProcessors/$lNeedProcessor.hpp
	$(FLAGS) src/agent/needProcessors/$lNeedProcessor.cpp
        To horizon (after needProcessor):
$(OBJ_DIR)/$lNeedProcessor.o
        To link (after needProcessor):
$lNeedProcessor.o
 * (if your file is named e.g. hungerNeedProcessor.hpp)
 *
 * Create an .ept spec in needs, then create a chains .ept in chains.
 * Add the need to needDirectory.ept, then make sure to init and add to
 * the needDirectory. You can then put it in a species spec using its ID.
 * 
 * DELETE THIS COMMENT*/

/* --$tNeedProcessor--
 * $tNeedProcessor performs all the logic for a $t need.
 * */
class $tNeedProcessor:public NeedProcessor
{
    private:
        Need defaultNeed;
    public:
        int needID;

        $tNeedProcessor();
        virtual ~$tNeedProcessor();
        //Use this function to set defaults for this need (you must call this!)
        virtual bool initialize(eptFile* spec);
        //deltaTime should be the time since this need was last updated
        virtual int updateNeed(Agent* agent, Object* obj, ObjectManager* objectManager, Need* currentNeed, Time* deltaTime);
        //Sets the passed need's fields to defaults (specified by defaultNeed)
        virtual void initNeed(Need* currentNeed);
};

#endif
