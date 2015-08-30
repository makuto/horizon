#ifndef POOL_HPP
#define POOL_HPP
#include <stdlib.h>
#include <vector>
#include <iostream>

/* --Pool--
 * Pool holds any data. It initializes the requested amount of data on construction.
 * Any subsequent calls to getNewData() and removeData() cost very little
 * because no more memory is allocated or freed. This provides significant
 * speed improvements.
 *
 * It uses a doubly linked list for faster iteration through the active data
 * and a singly linked list for iteration through free/inactive data.
 *
 * Vector is used to store the data due to its resize function, which allows
 * the pool's size to be set on initialization. The pool cannot be resized
 * after initialization because vector must move all data to get one complete
 * block.
 *
 * Note: You must compile with -std=c++11 if you want your data type's
 * constructors to be called.
 *
 * TODO: Need to construct inUse list to be in order (better locality)!
 * */

//Generic data container. Holds the requested data as well as pointers to the next
//free data (if inactive), and the previous and next active/used data (if active)
template<class T>
class PoolData
{
    public:
        T data;
        bool isActive;
        PoolData<T>* nextFreeData;
        PoolData<T>* nextUsedData;
        PoolData<T>* prevUsedData;
};

template<class R>
class Pool
{
    private:
        std::vector<PoolData<R> > pool;     //The pool data, stored in a STL vector
        PoolData<R>* firstFreeData;         //The head of the free data linked list
        PoolData<R>* firstUsedData;         //The head of the used data linked list
        unsigned int size;                  //The size of the pool
        unsigned int totalActiveData;       //The number of active data in the pool
        
        //Prepare the data pointers as a linked list
        void resetPool()
        {
            //Iterate through all data, resetting pointers and isActive status
            firstFreeData = &pool[0];
            for (unsigned int i = 0; i < size - 1; ++i)
            {
                PoolData<R>* currentData = &pool[i];
                currentData->nextFreeData = &pool[i + 1];
                currentData->nextUsedData = NULL;
                currentData->prevUsedData = NULL;
                currentData->isActive = false;
            }
            //Last datum needs NULL status to signal end of linked list
            PoolData<R>* lastData = &pool[size - 1];
            lastData->nextFreeData = NULL;
            lastData->nextUsedData = NULL;
            lastData->prevUsedData = NULL;
            lastData->isActive = false;
        }
    public:
        //Initializes the Pool with newSize elements. Once this constructor
        //is called, no more memory is allocated. 
        Pool(unsigned int newSize)
        {
            size = newSize;
            totalActiveData = 0;
            firstFreeData = NULL;
            firstUsedData = NULL;
            pool.resize(size);
            resetPool();
        }
        //Resets the pool. Note that this isn't cheap because it must fix up the linked list
        //that underlies the Pool structure
        void clear()
        {
            totalActiveData = 0;
            firstFreeData = NULL;
            firstUsedData = NULL;
            resetPool();
        }
        //Returns the first free data in the pool, or NULL if the pool is
        //full.
        PoolData<R>* getNewData()
        {
            if (firstFreeData != NULL) //Make sure the pool isn't full
            {
                PoolData<R>* freeData = firstFreeData;
                firstFreeData = firstFreeData->nextFreeData;
                //Link this data into the usedData linked list
                if (firstUsedData != NULL) firstUsedData->nextUsedData = freeData;
                freeData->nextUsedData = NULL;
                freeData->prevUsedData = firstUsedData;
                firstUsedData = freeData;
                freeData->isActive = true;
                totalActiveData++;
                return freeData;
            }
            return NULL; //Pool is full
        }
        
        //Sets the data to inactive so that it can be used again.
        void removeData(PoolData<R>* dataToRemove)
        {
            dataToRemove->nextFreeData = firstFreeData;
            firstFreeData = dataToRemove;
            //Unlinks this data from the usedData linked list
            if (firstFreeData->prevUsedData)
                firstFreeData->prevUsedData->nextUsedData = firstFreeData->nextUsedData;
            totalActiveData--;
            dataToRemove->isActive = false;
        }
        
        //Gets data at the requested index.
        //Returns NULL if index isn't within range or data is not active
        //Use getNextActiveData if you want to iterate through the pool
        //(it uses nextUsedData to skip over inactive data)
        PoolData<R>* getActiveDataAtIndex(unsigned int index)
        {
            //Index out of bounds
            if (index > size) return NULL;
            PoolData<R>* data = &pool[index];
            if (data->isActive) return data;
            else return NULL; //Data isn't in use
        }
        
        //Uses prevUsedData to skip over inactive data. This function will
        //all fast iteration through active data in the pool. Break on NULL
        PoolData<R>* getNextActiveData(PoolData<R>* currentData)
        {
            return currentData->prevUsedData;
        }
        
        //Returns the first active data (use in conjunction with getNextActiveData)
        //to traverse the pool. Returns NULL if the pool is empty
        PoolData<R>* getFirstActiveData()
        {
            return firstUsedData;
        }
        
        //Returns the total number of active data in the pool
        unsigned int getTotalActiveData()
        {
            return totalActiveData;
        }
};
#endif

