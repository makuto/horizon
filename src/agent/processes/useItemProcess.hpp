#ifndef USEITEM_PROCESS_HPP
#define USEITEM_PROCESS_HPP
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
 *  Replace USEITEM with CAPS name (e.g USEITEM -> USEITEM)
 *  Replace UseItem with Proper name (e.g UseItem -> UseItem)
 *  Replace useItem with lower name (e.g. useItem -> useItem)
 * 
 *  In .cpp, change the include "CHANGEME.hpp" to the name of this .hpp
 * 
 * Cut the text below into the makefile
        --------------
        under process:
        --------------
$(OBJ_DIR)/useItemProcess.o: src/agent/processes/useItemProcess.hpp src/agent/processes/useItemProcess.cpp
	$(FLAGS) src/agent/processes/useItemProcess.hpp
	$(FLAGS) src/agent/processes/useItemProcess.cpp
        ---------------------------
        To horizon (after process):
        ---------------------------
$(OBJ_DIR)/useItemProcess.o
        -------------------------
        To link (after process):
        -------------------------
useItemProcess.o

 *
 * 
 * DELETE THIS COMMENT*/

/* --UseItemProcess--
 * TODO: Document this process
 * */
struct Agent;


class UseItemProcess:public Process
{
    private:
        int counter;
    public:
        virtual ~UseItemProcess();
        
        virtual int getDifficulty(Agent* agent, Need* need);
        virtual unsigned char getValue(Agent* agent, Need* need);
        //Update should return 0 if process didn't complete (run next frame)
        //1 if process is ready to go to the next process in chain
        //-1 if process chain should end immediately
        virtual int update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime);
};
#endif
