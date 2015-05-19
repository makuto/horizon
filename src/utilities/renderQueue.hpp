#ifndef RENDERQUEUE_HPP
#define RENDERQUEUE_HPP
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/ept/eptParser.hpp>
#include "../world/coord.hpp"
#include "imageManager.hpp"

//The sprite's origin
enum RENDER_ORIGIN_MODE
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CENTER
};

//The layer to render on
enum RENDER_QUEUE_LAYER:int
{
    ONGROUND = 0,
    ABOVEGROUND
};
struct RenderInstance
{
    //META: I'm trying the John Carmack style indentation here
    std::string         imageId;        //The ID of the desired image
    std::string         subRectId;      //The ID of the desired subrect
    Coord               position;       //The position of the image (I use a
                                        //Coord here so that view no longer has
                                        //to be so global)
    float               velX;           //Stores the velocity of the image.
    float               velY;           //Used to extrapolate for fixed timesteps
                                        //Should be set if using renderLayerExtrapolate
    float               rotation;       //The rotation of the image
    RENDER_ORIGIN_MODE  originMode;     //What the origin should be in regards to
                                        //the provided position (e.g. CENTER would
                                        //center the image on the position)
    int                 requestorId;    //The ID of the instance requestor (for debug)
};
/**--RenderStage--
 * RenderStage contains an array of RenderInstances treated like a stack.
 * */
class RenderStage
{
    private:
        std::vector<RenderInstance> instances;
        unsigned int nextEmptyIndex;
    public:
        RenderStage();
        //Resizes the instances vector to hte provided value
        void setMaxInstances(int newMaxInstances);
        //Returns an unused instance, or NULL if MAXIMUM_RENDER_INSTANCES
        //has been reached. Note that the instance is not reset from the last
        //time that it was used
        RenderInstance* getEmptyInstance();
        //Returns the instance at the top of the stack, or NULL if the stack
        //is empty. Once popped, it is treated as an empty instance but still
        //retains its data.
        RenderInstance* popInstance();
        //Resets nextEmptyIndex to 0, effectively wiping the stage
        void reset();
};
/* --RenderQueue--
 * RenderQueue takes rendering requests in the form of RenderInstances and
 * holds them until renderLayer is called. This is useful because it allows
 * precise control of layering, which is impossible with Base/SFML. It also
 * gives more precise control over when rendering actually happens, and
 * decouples window and view from render clients.
 *
 * RenderQueue should probably not be used with particles etc.*/
typedef std::map<int, std::vector<RenderStage>* > renderLayerMap;
class RenderQueue
{
    private:
        renderLayerMap layers;
        window* win;
        ImageManager* imageManager;
    public:
        //Sets up layers as specified in the EPT specification (see end of file)
        RenderQueue(eptFile* spec, window* newWin, ImageManager* newImageManager);
        //Destroys layers
        ~RenderQueue();
        //Returns a RenderInstance on the desired layer and stage. Simply set
        //the instance to the image, position, etc. that you want and it
        //will be rendered in the RenderQueue.render() function when called.
        //Returns NULL if the stage doesn't exist or is full
        RenderInstance* getInstance(RENDER_QUEUE_LAYER layer, unsigned int stage);
        //Renders a single instance.
        void renderInstance(Coord& viewPosition, RenderInstance* instance);
        //Renders all used instances in each stage of the specified layer
        void renderLayer(Coord& viewPosition, RENDER_QUEUE_LAYER layer);
        //Renders all used instances in each stage, extrapolating on their next
        //position by predicting how far they will be at their velocity.
        //extrapolateAmount is how far we should extrapolate - between 0 and 1.0
        void renderLayerExtrapolate(Coord& viewPosition, RENDER_QUEUE_LAYER layer, float extrapolateAmount);
        //Resets all RenderStages on all layers
        void reset();
};
/* --EPT Specification--
 * The EPT format is the following:
 *  //Group name is layer name
 *  1:
 *      //Attribute name is ignored, but they should be in order
 *      1=1000;
 *      //Attribute value is how many instances in a stage
 *      2=100;
 *      3=100;
 *  2:
 *      1=10;
 *      2=10;
 * */
#endif
