#ifndef OBJECTPROCESSORDIR_CPP
#define OBJECTPROCESSORDIR_CPP
#include "objectProcessorDir.hpp"

ObjectProcessorDir::~ObjectProcessorDir()
{
    for (std::map<int, ObjectProcessor*>::iterator it = processors.begin();
    it != processors.end(); ++it)
    {
        delete it->second;
    }
}
void ObjectProcessorDir::addObjProcessor(ObjectProcessor* newProcessor)
{
    processors[newProcessor->getType()] = newProcessor;
}
//Returns NULL if the processor wasn't found
ObjectProcessor* ObjectProcessorDir::getObjProcessor(int type)
{
    std::map<int, ObjectProcessor*>::iterator search = processors.find(type);
    if (search != processors.end())
    {
        return search->second;
    }
    return NULL;
}
#endif
