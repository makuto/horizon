#ifndef IMAGEMANAGER_HPP
#define IMAGEMANAGER_HPP
#include <base2.0/graphics/graphics.hpp>
#include <base2.0/ept/eptParser.hpp>

struct SubRect
{
    int x;
    int y;
    int width;
    int height;
};
/**--ImageManager--
 * ImageManager holds all sprites and subRects. It loads the images automatically
 * and provides a convenient interface for requesting them. The integration
 * of subRects encourages use of spritesheets.
 * */
class ImageManager
{
    private:
        std::map<std::string, SubRect> subRects;
        std::map<std::string, sprite*> sprites;
    public:
        ImageManager();
        ~ImageManager();

        bool load(eptFile* spec);
        //Returns the sprite with the specified handle, or NULL if it doesn't exist
        //Note that this function also resets the subrect and rotation of the sprite
        sprite* getSprite(const std::string& spriteName);
        //Returns the sprite with the desired subRect applied, or NULL if it
        //doesn't exist
        sprite* getSubRectSprite(const std::string& spriteName, const std::string& subRectName);
        //Returns the sprite with the desired subRect applied, or NULL if it
        //doesn't exist. Also returns a reference to the subRect used on the image
        sprite* getSubRectSprite(const std::string& spriteName, const std::string& subRectName, SubRect& subRect);
        
};
/* EPT specification
 * <imageManager.1>
 *
 * //image handle
 * hud:
 *     //Filename, preceded by _
 *     _file=data/images/hud.png
 *     //All other attributes not preceded by '_' are treated as subrects
 *     healthBackground={0, 0, 400, 100};
 *     healthForeground={100, 100, 400, 100};
 *     //(in the format {x, y, width, height}; )
 *     //In the future, animations will be defined via #, e.g.
 *     #anim1={0, 0, 32, 32, 8, 24};
 *     //defined like so: {x, y, singleFrameW, h, totalFrames, FPS}
 *
 * You can then get the set up sprite by calling this (e.g.):
 *      sprite* hudBackground = imageManager.getSubRectSprite("hud", "healthBackground");
 * and it's ready to render!
 * */
#endif
