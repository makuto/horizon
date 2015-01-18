#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP
#include <map>
#include <vector>
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/collision/collision.hpp>
//#include "../world/world.hpp"
#include "../world/coord.hpp"
//#include "../world/cell.hpp"
#include "../utilities/quadTree.hpp"
#include "objectProcessor.hpp"
#include "objectProcessorDir.hpp"
#include "object.hpp"
//#include "../world/world.hpp"
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
class ObjectProcessor;
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
        CellIndex parentCellID;
        ObjectProcessorDir* processorDir;
        //Holds query points returned from getObjectsInRangeCache
        //numQueryPoints holds how many were returned
        Object** queryArray;
        int numQueryPoints;
        void getObjectsInRangeCache(aabb& range);

    public:
        ObjectManager(World* newWorld, ObjectProcessorDir* newProcessorDir,
        CellIndex newParentCellID, Cell* newParent);
        ObjectManager();
        void init(World* newWorld, ObjectProcessorDir* newProcessorDir,
        CellIndex newParentCellID, Cell* newParent);
        ~ObjectManager();
        
        //Returns a pointer to a vector with all objects in this
        //manager of the requested type or NULL if there are no objects of
        //the requested type. All objects in the array are initialized (in use)
        //Delete the array once you are done.
        //TODO: Add has any of type function
        //TODO: Add cached version of this function, if necessary
        std::vector<Object*>* getObjectsOfType(int type);
        //Returns a pointer to a vector with all objects in the area in this
        //cell or NULL if there are no objects in the range.
        //Delete the array once you are done.
        std::vector<Object*>* getObjectsInRange(aabb& range);
        //Returns a pointer to an uninitialized (nor constructed) object
        //or NULL if there are no free pool spaces. The object will be added
        //to the quadtree at the specified position. position will be set to
        //This current cell and x, y; type will also be set
        Object* getNewRawObject(int type, float x, float y);
        //Uses getNewRawObject and calls initObject() for the object's type
        Object* getNewInitializedObject(int type, int subType, float x, float y, float rotation);
        //Removes an object from the pool (must call this to work with pool)
        //Type comes from object, so don't wipe the memory
        //Bad things will happen if you try to remove an object from the wrong manager
        //(malformed uninitialized object)
        void removeObject(Object* objectToRemove);
        //Objects must call this function to change position if they are going
        //to work with collisions and quadtree. Pass the new desired position
        //and ObjectManager handles the rest. Do not try to set position yourself
        //(FIXED; objects now require all positions to go through an obj man first)
        //Collisions can only be prevented; overlapping objects will stay that
        //way until one object or the other resolves/moves away.
        //Collisions are determined by manhattan distances (set in obj.manhattanRadius);
        //this means bounds might detect collisions differently (this is because
        //preventCollision() does not provide information on if the bounds are colliding
        void moveObject(Object* objectToMove, Coord& newPosition);
        //Gets all objects onscreen and calls ObjProcessor.renderObject()
        void renderObjects(float viewX, float viewY, window* win);
        //Updates all objects
        //Returns false if there are no active objects
        bool updateObjects(Time* globalTime);
};
#endif
