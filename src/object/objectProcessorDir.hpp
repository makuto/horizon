#ifndef OBJECTPROCESSORDIR_HPP
#define OBJECTPROCESSORDIR_HPP
#include "objectProcessor.hpp"
/* --ObjectProcessorDir--
 * Holds ObjectProcessors
 * */
class ObjectProcessorDir
{
    private:
        std::map<int, ObjectProcessor*> processors;
    public:
        ~ObjectProcessorDir();
        void addObjProcessor(ObjectProcessor* newProcessor);
        //Returns NULL if the processor wasn't found
        ObjectProcessor* getObjProcessor(int type);
};
#endif
