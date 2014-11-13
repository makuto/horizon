#ifndef PROCESSMAP_HPP
#define PROCESSMAP_HPP
#include <map>
#include <string>
#include "process.hpp"
class ProcessMap
{
    private:
        std::map<std::string, Process*> processMap;
    public:
        ~ProcessMap(); //Free processes
        Process* getProcess(std::string& searchString);
        void addProcess(std::string name, Process* processToAdd);
};

#endif
