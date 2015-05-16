#ifndef INPUTSTATE_HPP
#define INPUTSTATE_HPP
#include <base2.0/input/input.hpp>
#include <base2.0/ept/eptParser.hpp>
#include "../world/time.hpp"
//Input holds a named input with universal code and state variables
struct Input
{
    int code;           //Universal key and mouse code
    std::string name;   //Name of this specific input (e.g. 'pause')
    int state;          //Universal key state (0 or 1 for binary inputs
                        //(e.g. space); -x to +x for variable inputs (e.g.
                        //mouse))
};
/* --InputState--
 * InputState stores the state of all inputs of interest (as defined by
 * an EPT spec) as abstract handles. This allows defining of inputs by
 * function rather than by input.
 *
 * For example, instead of querying a mouse button, you would query "attack",
 * which was defined in a EPT spec as SPACE. This makes very simple configurable
 * input!
 *
 * Additionally, InputStates can be stored for input recording, which allows
 * replays and makes testing easier. (TODO)
 * */
class InputState
{
    //Where the inputs will be collected from
    enum Source
    {
        INPUT_MANAGER,      //Get the input from a standard inputManager
        FILE,               //Get the input from a file      (TODO)
        NETWORK             //Get the input from the network (TODO)
    };
    private:
        std::map<std::string, Input> inputs;
        Source inputSource;
        inputManager* inputMan;
        Time lastUpdate;
    public:
        InputState();
        //Populates inputs map with the inputs specified in the spec;
        //sets Source depending on the second parameter
        void setup(eptGroup* spec, inputManager* newInputManager);
        //TODO (possibly)
        //void setup(eptFile* spec, InputRecording* recording);
        //void setup(eptFile* spec, InputNetworked* networkInput);

        //Updates the states of all inputs in the inputs map
        //Will eventually be able to read these from a file or network
        void update(Time* globalTime);

        //Returns the current state of the input (since last update()) or
        //0 if the input does not exist (which also causes an error to be printed)
        int getInputState(const std::string& inputName); 
};
/* --EPT Specification--
 * The EPT spec should be defined as follows:
 * <myInput.1>
 * //Group names do not matter as you will be passing in a single
 * //group (use them to pick the right input mode); this functions
 * // as a useful mechanism for having different input modes and
 * //configurations in the same .ept file
 * menuControls:
 *      //Provide the name of the input as well as the universal code of
 *      //the input; check base2.0/input/input.hpp for input codes
 *      back=43;
 *      accept=63;
 * gameplayControls:
 *      forward=55;
 *      backward=54;
 *      pause=100;
 * //etc.
 * 
 * You would then create an InputState like so:
 * InputState menuControlInputs;
 * menuControlInputs.setup(parser.getGroup("myInput.menuControls"), inputMan);
 * //And
 * InputState gameplayControlInputs;
 * gameplayControlInputs.setup(parser.getGroup("myInput.gameplayControls"), inputMan);
 * */
#endif
