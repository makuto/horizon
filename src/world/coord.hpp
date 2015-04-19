#ifndef COORD_HPP
#define COORD_HPP
/* --Coord--
 * Coord is a universal position coordinate.
 *
 * The entire coordinate system is defined in coord.cpp. Use these
 * constants whenever you can in order to make it flexible
 * */
extern const int CELL_WIDTH;
extern const int CELL_HEIGHT;
extern const int TILE_WIDTH;
extern const int TILE_HEIGHT;
extern const int TILE_MANHATTAN_RADIUS;
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
//Used to be able to use CellIndex as a key in a std::map. See 
//http://stackoverflow.com/questions/6973406/c-stl-map-container-with-class-key-and-class-value
struct CellIndexComparer
{
    bool operator()(const CellIndex& first, const CellIndex& second) const;
};
//TODO: fix small hitch at cell borders
class Time;
class Coord
{
    protected:
        CellIndex cell;
        float x, y;
    public:
        bool operator==(const Coord& a);
        //Returns the coordinate inside the cell
        float getCellOffsetX();
        float getCellOffsetY();
        CellIndex getCell();
        float getTrueX();
        float getTrueY();
        //Returns screen space coordinate relative to the view
        float getScreenX(Coord* viewPosition);
        float getScreenY(Coord* viewPosition);
        //Return the position relative to the passed in index
        float getRelativeCellX(CellIndex& index);
        float getRelativeCellY(CellIndex& index);
        //Returns the distance between the two
        float getDistanceTo(Coord& point);
        //Returns the manhattan distance between the two
        float getManhattanTo(Coord& point);

        void print();
        //Add the desired delta (in floating points);
        //overrun will be applied to CellIndex automatically
        void addVector(float dX, float dY);
        //Moves towards the given point at the specified speed
        void moveTowards(Coord& point, float speed, Time* deltaTime);
        void setPosition(CellIndex& newCell, float newX, float newY);
        void setPosition(int newCellX, int newCellY, float newX, float newY);
};
#endif
