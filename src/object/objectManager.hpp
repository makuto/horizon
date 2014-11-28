#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP
#include <map>
#include <vector>
#include "../utilities/quadTree.hpp"
#include "object.hpp"
#include "objectProcessor.hpp"
/* --ObjectManager--
 * ObjectManagers control the creation, rendering, interaction, and deletion
 * of Objects. ObjectManagers don't care what type the objects are.
 * */
struct ObjectPool
{
    Object* firstEmptyObj;
    std::vector<Object> pool;
};
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
    public:
        ObjectManager();
        ~ObjectManager();
        
        //Returns a pointer to a vector with all objects in this
        //manager of the requested type or NULL if there are no objects of
        //the requested type
        std::vector<Object>* getObjectsOfType(int type);
        //Returns a pointer to an uninitialized (nor constructed) object
        //or NULL if there are no free pool spaces
        Object* getNewObject(int type, float x, float y);
        //Removes an object from the pool (must call this to work with pool)
        //Type comes from object, so don't wipe the memory
        //Bad things will happen if you try to remove an object from the wrong manager
        //(malformed uninitialized object)
        void removeObject(Object* objectToRemove);
};
#endif
