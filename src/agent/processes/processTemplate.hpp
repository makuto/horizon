#ifndef $T_PROCESS_HPP
#define $T_PROCESS_HPP
#include <vector>
#include "../process.hpp"
#include "../../world/time.hpp"
//#include "agent.hpp" //Forward declared
#include "../need.hpp"

#include "../../object/objectManager.hpp"
#include "../../object/object.hpp"

/*To use template:
 * Save As -> yourProcess.hpp (and .cpp) (e.g. useItemProcess.hpp, useItemProcess.cpp)
 * In both .cpp and .hpp:
 *  Replace $T with CAPS name (e.g $T -> USEITEM)
 *  Replace $t with Proper name (e.g $t -> UseItem)
 *  Replace $l with lower name (e.g. $l -> useItem)
 * 
 *  In .cpp, change the include "CHANGEME.hpp" to the name of this .hpp
 * 
 * Cut the text below into the makefile
        --------------
        under process:
        --------------
$(OBJ_DIR)/$lProcess.o: src/agent/processes/$lProcess.hpp src/agent/processes/$lProcess.cpp
	$(FLAGS) src/agent/processes/$lProcess.hpp
	$(FLAGS) src/agent/processes/$lProcess.cpp
        ---------------------------
        To horizon (after process):
        ---------------------------
$(OBJ_DIR)/$lProcess.o
        -------------------------
        To link (after process):
        -------------------------
$lProcess.o

 *
 * 
 * DELETE THIS COMMENT*/

/* --$tProcess--
 * TODO: Document this process
 * */
struct Agent;


class $tProcess:public Process
{
    public:
        virtual ~$tProcess();
        //Return the difficulty of the process, or -1 if impossible. index
        //contains the index of the process in its chain
        virtual int getDifficulty(Agent* agent, Object* obj, Need* need, int index);
        //Update should return 0 if process didn't complete (run next frame)
        //1 if process is ready to go to the next process in chain
        //-1 if process chain should end immediately
        virtual int update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime);
};
#endif
