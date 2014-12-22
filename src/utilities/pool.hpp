#ifndef POOL_HPP
#define POOL_HPP
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <vector>
#include <iostream>
//IMPORTANT: You must compile with -std=c++11 if you want constructors to be called!
/* --Pool--
 * Pool holds any data. It initializes it on construction, then manages the
 * data without new or delete, so it is faster.
 * */
template<class T>
class PoolData
{
    public:
        T data;
        bool isActive;
        PoolData<T>* nextFreeData;
};

template<class R>
class Pool
{
    private:
        std::vector<PoolData<R> > pool;
        PoolData<R>* firstFreeData;
        unsigned int size;
        void resetPool()
        {
            firstFreeData = &pool[0];
            for (unsigned int i = 0; i < size - 1; ++i)
            {
                PoolData<R>* currentData = &pool[i];
                currentData->nextFreeData = &pool[i + 1];
                currentData->isActive = false;
            }
            //Last datum needs null
            PoolData<R>* lastData = &pool[size - 1];
            lastData->nextFreeData = NULL;
            lastData->isActive = false;
        }
    public:
        Pool(unsigned int newSize)
        {
            size = newSize;
            pool.resize(size);
            resetPool();
        }
        ~Pool()
        {
            
        }
        PoolData<R>* getNewData()
        {
            if (firstFreeData != NULL)
            {
                PoolData<R>* freeData = firstFreeData;
                firstFreeData = firstFreeData->nextFreeData;
                freeData->isActive = true;
                return freeData;
            }
            return NULL; //Pool is full
        }
        void removeData(PoolData<R>* dataToRemove)
        {
            dataToRemove->nextFreeData = firstFreeData;
            firstFreeData = dataToRemove;
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
};
#endif

