#ifndef COORD_HPP
#define COORD_HPP
/* --Coord--
 * Coord is a universal position coordinate.
 * */
extern const int CELL_WIDTH;
extern const int CELL_HEIGHT;
extern const int TILE_WIDTH;
extern const int TILE_HEIGHT;
//Note that you want this value to always be larger than screen size
//UPDATE: No longer required to be larger than screen
extern const int CELL_WIDTH_PIXELS;
extern const int CELL_HEIGHT_PIXELS;
//Simple x,y index to a cell
struct CellIndex
{
    int x;
    int y;
};
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
