#ifndef PROCESSMAP_CPP
#define PROCESSMAP_CPP
#include "processMap.hpp"

ProcessMap::~ProcessMap() //Free processes
{
    for (std::map<std::string, Process*>::iterator it = processMap.begin(); it!=processMap.end(); ++it)
    {
        delete it->second;
    }
}
Process* ProcessMap::getProcess(std::string& searchString)
{
    std::map<std::string, Process*>::iterator search = processMap.find(searchString);
    if (search!=processMap.end())
    {
        return search->second;
    }
    return NULL;
}
void ProcessMap::addProcess(std::string name, Process* processToAdd)
{
    processMap[name] = processToAdd;
}

#endif
