#ifndef COORD_HPP
#define COORD_HPP
#include "cell.hpp"
/* --Coord--
 * Coord is a universal position coordinate.
 * */
class Coord
{
    protected:
        CellIndex cell;
        float x, y;
    public:
        //Returns the coordinate inside the cell
        float getCellOffsetX();
        float getCellOffsetY();
        CellIndex getCell();
        float getTrueX();
        float getTrueY();
        //Returns screen space coordinate relative to the view
        float getScreenX(Coord* viewPosition);
        float getScreenY(Coord* viewPosition);
        void print();
        //Add the desired delta (in floating pixels);
        //overrun will be applied to CellIndex automatically
        void addVector(float dX, float dY);
        void setPosition(CellIndex& newCell, float newX, float newY);
};
#endif
