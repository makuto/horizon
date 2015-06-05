#ifndef POOL_HPP
#define POOL_HPP
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <vector>
#include <iostream>
//IMPORTANT: You must compile with -std=c++11 if you want constructors to be called!
/* --Pool--
 * Pool holds any data. It initializes it on construction, then manages the
 * data without new or delete, so it is faster.
 *
 * It is a doubly linked list for faster looping
 * */
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
        std::vector<PoolData<R> > pool;
        PoolData<R>* firstFreeData;
        PoolData<R>* firstUsedData;
        unsigned int size;
        unsigned int totalActiveData;
        void resetPool()
        {
            firstFreeData = &pool[0];
            for (unsigned int i = 0; i < size - 1; ++i)
            {
                PoolData<R>* currentData = &pool[i];
                currentData->nextFreeData = &pool[i + 1];
                currentData->nextUsedData = NULL;
                currentData->prevUsedData = NULL;
                currentData->isActive = false;
            }
            //Last datum needs null
            PoolData<R>* lastData = &pool[size - 1];
            lastData->nextFreeData = NULL;
            lastData->nextUsedData = NULL;
            lastData->prevUsedData = NULL;
            lastData->isActive = false;
        }
    public:
        Pool(unsigned int newSize)
        {
            size = newSize;
            totalActiveData = 0;
            firstFreeData = NULL;
            firstUsedData = NULL;
            pool.resize(size);
            resetPool();
        }
        ~Pool()
        {
            
        }
        void clear()
        {
            totalActiveData = 0;
            firstFreeData = NULL;
            firstUsedData = NULL;
            resetPool();
        }
        PoolData<R>* getNewData()
        {
            if (firstFreeData != NULL)
            {
                PoolData<R>* freeData = firstFreeData;
                firstFreeData = firstFreeData->nextFreeData;
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
        void removeData(PoolData<R>* dataToRemove)
        {
            dataToRemove->nextFreeData = firstFreeData;
            firstFreeData = dataToRemove;
            if (firstFreeData->prevUsedData)
                firstFreeData->prevUsedData->nextUsedData = firstFreeData->nextUsedData;
            totalActiveData--;
            dataToRemove->isActive = false;
        }
        //Returns NULL if index isn't within range or data is not active
        PoolData<R>* getActiveDataAtIndex(unsigned int index)
        {
            if (index > size) return NULL;
            PoolData<R>* data = &pool[index];
            if (data->isActive) return data;
            else return NULL;
        }
        //Uses prevUsedData to skip over inactive data. Break on NULL
        PoolData<R>* getNextActiveData(PoolData<R>* currentData)
        {
            return currentData->prevUsedData;
        }
        //Returns the first active data (use in conjunction with getNextActiveData
        //to traverse the pool. Returns NULL if the pool is empty
        PoolData<R>* getFirstActiveData()
        {
            return firstUsedData;
        }
        
        unsigned int getTotalActiveData()
        {
            return totalActiveData;
        }
};
#endif

