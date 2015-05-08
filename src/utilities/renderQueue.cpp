#ifndef RENDERQUEUE_CPP
#define RENDERQUEUE_CPP
#include "renderQueue.hpp"
#include <iostream>

RenderStage::RenderStage()
{
    nextEmptyIndex = 0;
}
void RenderStage::setMaxInstances(int newMaxInstances)
{
    instances.resize(newMaxInstances);
    reset();
}
//Returns an unused instance, or NULL if MAXIMUM_RENDER_INSTANCES
//has been reached. Note that the instance is not reset from the last
//time that it was used
RenderInstance* RenderStage::getEmptyInstance()
{
    if (nextEmptyIndex >= instances.size())
    {
        return NULL;
    }
    RenderInstance* newInstance = &instances[nextEmptyIndex];
    nextEmptyIndex++;
    return newInstance;
}
//Returns the instance at the top of the stack, or NULL if the stack
//is empty. Once popped, it is treated as an empty instance but still
//retains its data.
RenderInstance* RenderStage::popInstance()
{
    if (nextEmptyIndex == 0) return NULL;
    nextEmptyIndex--;
    RenderInstance* poppedInstance = &instances[nextEmptyIndex];
    return poppedInstance;
}
//Resets nextEmptyIndex to 0, effectively wiping the stage
void RenderStage::reset()
{
    nextEmptyIndex = 0;
}
RenderQueue::RenderQueue(eptFile* spec)
{
    eptGroup* group;
    std::string groupName;
    group = spec->getGroupFromIndex(0, groupName);
    for (int i = 1; group != NULL; ++i)
    {
        std::vector<RenderStage>* newLayer = new std::vector<RenderStage>;
        //Resize the vector to hold all specified stages
        newLayer->resize(group->getTotalAttributes());
        std::string attrName;
        std::string attrValue = group->getAttributeFromIndex(0, attrName);
        for (int n = 1; attrValue != ""; n++)
        {
            int numInstances = attrToInt(attrValue);
            (*newLayer)[n-1].setMaxInstances(numInstances);
            //Get next value
            attrValue = group->getAttributeFromIndex(n, attrName);
        }
        //Add the new layer to the layersMap
        layers[attrToInt(groupName)] = newLayer;
        //Get next layer
        group = spec->getGroupFromIndex(i, groupName);
    }
    
}
RenderQueue::~RenderQueue()
{
    for (renderLayerMap::iterator it = layers.begin(); it != layers.end(); ++it)
    {
        delete it->second;
    }
}
//Returns a RenderInstance on the desired layer and stage. Simply set
//the instance to the image, position, etc. that you want and it
//will be rendered in the RenderQueue.render() function when called.
//Returns NULL if the stage doesn't exist or is full
RenderInstance* RenderQueue::getInstance(RENDER_QUEUE_LAYER layer, unsigned int stage)
{
    //Find the layer
    renderLayerMap::iterator findIt = layers.find(layer);
    if (findIt == layers.end())
    {
        std::cout << "ERROR: RenderQueue.getInstance(): Layer " << layer << " doesn't exist!\n";
        return NULL;
    }
    
    std::vector<RenderStage>* currentLayer = findIt->second;
    //Make sure stage is in bounds
    if (stage >= currentLayer->size())
    {
        std::cout << "ERROR: RenderQueue.getInstance(): Stage " << stage << " on layer " << layer << " doesn't exist!\n";
        return NULL;
    }
    return (*currentLayer)[stage].getEmptyInstance();
}
//Renders a single instance.
void RenderQueue::renderInstance(window* win, ImageManager* imageManager, Coord& viewPosition, RenderInstance* instance)
{
    //Get instance sprite
    sprite* spr = NULL;
    SubRect subRect;
    subRect.x = -1;
    if (instance->subRectId == "") spr = imageManager->getSprite(instance->imageId);
    else spr = imageManager->getSubRectSprite(instance->imageId, instance->subRectId, subRect);
    if (!spr) return;

    //Prepare sprite
    CellIndex viewCell = viewPosition.getCell();
    float x = instance->position.getRelativeCellX(viewCell);
    float y = instance->position.getRelativeCellY(viewCell);
    spr->setPosition(x, y);
    spr->setRotation(instance->rotation);

    //Get true width and height (subrect could make it hard to set origin)
    int width = spr->getWidth();
    int height = spr->getHeight();
    if (subRect.x != -1)
    {
        width = subRect.width;
        height = subRect.height;
    }
    //Set the origin as specified by originMode
    switch(instance->originMode)
    {
        case RENDER_ORIGIN_MODE::CENTER:
            spr->setOrigin(width / 2, height / 2);
            break;
        case RENDER_ORIGIN_MODE::TOP_LEFT:
            spr->setOrigin(0, 0);
            break;
        case RENDER_ORIGIN_MODE::TOP_RIGHT:
            spr->setOrigin(width, 0);
            break;
        case RENDER_ORIGIN_MODE::BOTTOM_LEFT:
            spr->setOrigin(0, height);
            break;
        case RENDER_ORIGIN_MODE::BOTTOM_RIGHT:
            spr->setOrigin(width, height);
            break;
        default:
            spr->setOrigin(0, 0);
            break;
    }

    //Render the instance
    win->draw(spr);
}
//Renders all used instances in each stage of the specified layer
void RenderQueue::renderLayer(window* win, ImageManager* imageManager, Coord& viewPosition, RENDER_QUEUE_LAYER layer)
{
    //Find the layer
    renderLayerMap::iterator findIt = layers.find(layer);
    if (findIt == layers.end())
    {
        std::cout << "ERROR: RenderQueue.renderLayer(): Layer " << layer << " doesn't exist!\n";
        return;
    }
    
    std::vector<RenderStage>* currentLayer = findIt->second;
    for (std::vector<RenderStage>::iterator it = currentLayer->begin(); it != currentLayer->end(); ++it)
    {
        //Render all instances in this stage
        RenderInstance* instance = (*it).popInstance();
        while (instance != NULL)
        {
            renderInstance(win, imageManager, viewPosition, instance);
            instance = (*it).popInstance();
        }
    }
}
//Resets all RenderStages on all layers
void RenderQueue::reset()
{
    for (renderLayerMap::iterator it = layers.begin(); it != layers.end(); ++it)
    {
        std::vector<RenderStage>* currentLayer = it->second;
        for (std::vector<RenderStage>::iterator sIt = currentLayer->begin(); sIt != currentLayer->end(); ++sIt)
        {
            sIt->reset();
        }
    }
}
#endif
