#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP
#include <map>
#include <vector>
//#include "../world/world.hpp"
#include "../world/coord.hpp"
//#include "../world/cell.hpp"
#include "../utilities/quadTree.hpp"
#include "objectProcessor.hpp"
#include "object.hpp"
/* --ObjectManager--
 * ObjectManagers control the creation, rendering, interaction, and deletion
 * of Objects. ObjectManagers don't care what type the objects are.
 * */
struct ObjectPool
{
    Object* firstEmptyObj;
    std::vector<Object> pool;
};
class Cell;
class World;
class ObjectManager
{
    private:
        //Note that the objects themselves are NOT stored in this tree,
        //they are stored in 'objects.' The quadtree simply searches
        //pointers to objects in the map
        QuadTree<Object*>* indexQuadTree;
        //Objects are stored in a map with the key as their type
        //TODO: Should objects be stored or pointers?
        std::map<int, ObjectPool> objectPools;
        //Abstracts map.find; returns NULL if no pool exists
        ObjectPool* getObjectPool(int type);
        //Abstracts map insertion (objectPools[type])
        //Returns NULL if something went wrong
        ObjectPool* createPool(int type, unsigned int size);
        //ObjectManager needs the world in case objects go out of its bounds
        //and a different ObjectManager should take over
       World* world;
        Cell* parentCell;
    public:
        ObjectManager();
     ObjectManager(World* newWorld, Cell* newParent);
        ~ObjectManager();
        
        //Returns a pointer to a vector with all objects in this
        //manager of the requested type or NULL if there are no objects of
        //the requested type. All objects in the array are initialized (in use)
        //Delete the array once you are done.
        std::vector<Object*>* getObjectsOfType(int type);
        //Returns a pointer to an uninitialized (nor constructed) object
        //or NULL if there are no free pool spaces
        Object* getNewObject(int type, float x, float y);
        //Removes an object from the pool (must call this to work with pool)
        //Type comes from object, so don't wipe the memory
        //Bad things will happen if you try to remove an object from the wrong manager
        //(malformed uninitialized object)
        void removeObject(Object* objectToRemove);
        //Objects must call this function to change position if they are going
        //to work with collisions and quadtree. Pass the new desired position
        //and ObjectManager handles the rest. Do not try to set position yourself
        void moveObject(Object* objectToMove, Coord& newPosition);
};
#endif
