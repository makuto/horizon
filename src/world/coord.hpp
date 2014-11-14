#ifndef COORD_HPP
#define COORD_HPP
/* --Coord--
 * Coord is a universal position coordinate.
 * */
class Coord
{
    protected:
        float x, y;
    public:
        //Returns screen space coordinate relative to the view
        float getScreenX(Coord* viewPosition);
        float getScreenY(Coord* viewPosition);
        void addVector(float dX, float dY);
        void setPosition(float newX, float newY);
};
#endif
