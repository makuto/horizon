#ifndef GOTORESOURCE_PROCESS_HPP
#define GOTORESOURCE_PROCESS_HPP
#include <vector>
#include "../process.hpp"
#include "../../world/time.hpp"
//#include "agent.hpp" //Forward declared
#include "../need.hpp"

#include "../../object/objectManager.hpp"
#include "../../object/object.hpp"

/* --GoToResourceProcess--
 * TODO: Document this process
 * */
struct Agent;

class PathManager;
class ResourceTree;
class Resource;
class GoToResourceProcess:public Process
{
    private:
        PathManager* pathManager;
        ResourceTree* resourceTree;
        Resource** resourceSearchCache;
    public:
        GoToResourceProcess();
        virtual ~GoToResourceProcess();

        void setup(PathManager* newPathManager, ResourceTree* newResourceTree);
        //Return the difficulty of the process, or -1 if impossible. index
        //contains the index of the process in its chain
        virtual int getDifficulty(Agent* agent, Object* obj, Need* need, int index);
        //Update should return 0 if process didn't complete (run next frame)
        //1 if process is ready to go to the next process in chain
        //-1 if process chain should end immediately
        virtual int update(Agent* agent, Object* obj, ObjectManager* objectManager, Need* need, Time* deltaTime);
};
#endif
