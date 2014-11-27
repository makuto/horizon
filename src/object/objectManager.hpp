#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP
#include "../utilities/quadTree.hpp"
#include "object.hpp"
#include "objectProcessor.hpp"
/* --ObjectManager--
 * ObjectManagers control the creation, rendering, interaction, and deletion
 * of Objects. ObjectManagers don't care what type the objects are.
 * */
class ObjectManager
{
    private:
        //Note that the objects themselves should NOT be stored in this tree
        QuadTree<int>* head;
    public:
        ObjectManager();
};
#endif
