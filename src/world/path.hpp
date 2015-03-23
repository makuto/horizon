#ifndef PATH_HPP
#define PATH_HPP
#include <vector>
#include <map>
#include "../utilities/pool.hpp"
#include "coord.hpp"
#include "../utilities/micropather.h"

/* --Path--
 * Path stores paths from point A to point B. It is designed to
 * work with pooling. It is specialized for the Cell-tile system.
 *
 * There are two stages in calculating the Path: macro and micro. In
 * the macro stage, the cells with the lowest difficulty are selected
 * over high-difficulty cells. This is done via a simplex estimation.
 * Macro stage makes objects avoid trying to pathfind over large areas
 * of water or through mountains.
 * Micro stage consists of pathfinding from the object's current cell
 * to the next cell. Due to the nature of the cell caching system, the most
 * that is stored is only this distance - the path in its entirety is
 * never stored. This allows the cell caching system to work most efficiently.
 * Broken down, this is how Path brings an object from point A to point B:
 * 1. Object makes the request by passing in the world pointer, its own location
 *    (point A), and the goal location (point B)
 * 2. Path remains in standby while waiting to have a formal calculation request
 *    (by a path manager; this makes paths only calculate for x ms)
 * 3. Path first calculates macro by estimating the difficulty of cells
 *    (via simplex estimation) or by getting their true difficulty if they
 *    exist in the pool. Once macro is calculated, the path once again returns
 *    to standby
 * 4. Once calculate has been called again, Path chooses an arbitrary point
 *    in a tile one into the next cell (as specified by macro). It then calculates
 *    the path from the object's current position to that point. If the
 *    path is successful, the status of the Path becomes ready.
 * 5. The object, detecting that the Path is ready, calls advance(), which
 *    returns a vector the object can use to move along the path.
 * 6. Once the object reaches the next cell, Path turns back to standby,
 *    removes the micro path (tile to tile) and calculates a path to the
 *    next cell.
 * 7. If the object is in point B's cell, a path from the object to the
 *    destination is calculated rather than a path to the next cell.
 * 8. If the object is at the goal point (within a specified range),
 *    Path's status becomes point reached.
 *
 * Note that "object" is used liberally in this class - it doesn't actually
 * have to be an Object.
 * */
class World;
class Path : public micropather::Graph
{
    private:
        World* world;
        Coord startPosition;
        Coord endPosition;
        Coord currentPosition;
        CellIndex cellCoordOrigin;
        int status; //-1 = failed; 0 = standby (calculating), 1 = ready,
                    //2 = goal reached, 3 = Empty
                    
        micropather::MicroPather* pather;
        std::vector<void*> cellPath;
        std::vector<void*> currentTilePath;
        //The next cell the object is moving into (not necessarily the goal point's cell)
        CellIndex currentGoalCell;
        unsigned int currentGoalCellIndex; //The index of the current cell in cellPath
        unsigned int currentTileIndex; //Where the follower is in the path
        bool isMacro;   //Keep track of whether we are processing macro or not
        bool isGoalInCurrentCell;   //If we are in the the same cell as goal,
                                    //This will let us do that
        bool isTransferringCells;

        float estimatedDifficulty;

        //Helper functions for converting coordinate pairs to/from states
        void stateToPosition(void* state, unsigned int& x, unsigned int& y);
        void* positionToState(unsigned int x, unsigned int y);
    public:
        Path();
        Path(World* newWorld);
        ~Path();
        
        //Call this function to initialize this path (e.g., when
        //allocating from a pool)
        void init(World* newWorld, Coord newStartPosition, Coord newEndPosition);

        //Calculate paths at macro or micro depending on isMacro
        void calculate();
        void calculateTilePath();
        void calculateCellPath();
        //Pass in the object's current position. Its next goal position is returned
        Coord advance(Coord& currentPosition);
        
        //Returns the status of the path
        int getStatus();
        //Returns the expected difficulty (at the cell level); This is scaled
        //by CELL_DIFFICULTY_SCALE by MicroPather
        float getEstimatedDifficulty();

        //Functions for micropather
        virtual float LeastCostEstimate(void* startNode, void* endNode);
        virtual void AdjacentCost(void* currentNode, std::vector<micropather::StateCost>* neighbors);
        virtual void PrintStateInfo(void* currentNode);
};
#endif
