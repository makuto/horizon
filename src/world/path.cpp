#ifndef PATH_CPP
#define PATH_CPP
#include "path.hpp"
#include "world.hpp"
#include "cell.hpp"

#define uintPtr uintptr_t

//See micropather docs
const unsigned int PATHER_STATE_CACHE_SIZE = 2048;
const unsigned int PATHER_ADJACENT_CACHE = 4;
const int MICROPATHER_FAILED = micropather::MicroPather::NO_SOLUTION;
const int MICROPATHER_SAME_CELL = micropather::MicroPather::START_END_SAME;

//Because all positions must be unsigned, the coords must be made
//relative to some point that will make all cells positive. This
//constant determines how far to the left and up the system can go
const int MACRO_CELL_ORIGIN_ALLOWANCE = 5;
//How far in the pather should plan when moving from one cell to the next
//(greater values will make it more likely that the path will not go through
//unwalkable spaces)
const int PATH_CELL_TILE_INDENT = 20;
//Difficulty returned by cell.getDifficulty and world.estimateDifficulty
//is between 0 and 1. Micropather will mostly disregard differences in
//difficulty that are this small. CELL_DIFFICULTY_SCALE scales these
//difficulties to give more effect. Low CELL_DIFFICULTY_SCALE values will
//take more direct routes, regardless of difficulty (e.g. 1) while higher
//values result in less difficult routes (e.g. 100). At 100, paths will
//go around mountains and oceans. At 1, these features are disregarded.
const float CELL_DIFFICULTY_SCALE = 100;
//Multiplied by manhattan distance estimate heuristic
const float MANHATTAN_MULTIPLIER = 100;


Path::Path()
{
    pather = new micropather::MicroPather(this, PATHER_STATE_CACHE_SIZE, PATHER_ADJACENT_CACHE);
    world = NULL;
    isMacro = true;
    isGoalInCurrentCell = false;
    status = 3; //Empty
}
Path::Path(World* newWorld)
{
    pather = new micropather::MicroPather(this, PATHER_STATE_CACHE_SIZE, PATHER_ADJACENT_CACHE);
    world = newWorld;
    isMacro = true;
    isGoalInCurrentCell = false;
    status = 3; //Empty
}
Path::~Path()
{
    delete pather;
}
void Path::init(World* newWorld, Coord newStartPosition, Coord newEndPosition)
{
    pather->Reset();
    world = newWorld;
    startPosition = newStartPosition;
    endPosition = newEndPosition;
    currentPosition = startPosition;
    status = 0;
    isMacro = true;
    isGoalInCurrentCell = false;

    CellIndex startCell = startPosition.getCell();
    CellIndex endCell = endPosition.getCell();
    currentGoalCell = startCell;
    if (startCell.x == endCell.x && startCell.y == endCell.y)
    {
        isMacro = false; //Goal is in current cell; done calculating macro
        isGoalInCurrentCell = true;
        status = 0;
        cellCoordOrigin.x = startCell.x - MACRO_CELL_ORIGIN_ALLOWANCE;
        cellCoordOrigin.y = startCell.y - MACRO_CELL_ORIGIN_ALLOWANCE;
        currentTileIndex = 0;
        return;
    }
    //Get the range for the relative coordinates (all state positions must be positive)
    int minX = 0;
    int minY = 0;
    //Find the smallest value for relative range origin
    if (startCell.x < endCell.x) minX = startCell.x;
    else minX = endCell.x;
    if (startCell.y < endCell.y) minY = startCell.y;
    else minY = endCell.y;
    minX -= MACRO_CELL_ORIGIN_ALLOWANCE;
    minY -= MACRO_CELL_ORIGIN_ALLOWANCE;
    //Set the origin to the new range
    cellCoordOrigin.x = minX;
    cellCoordOrigin.y = minY;
    currentTileIndex = 0;
}
int Path::getStatus()
{
    return status;
}
//Returns the expected difficulty (at the cell level); This is scaled
//by CELL_DIFFICULTY_SCALE
float Path::getEstimatedDifficulty()
{
    return estimatedDifficulty;
}
void Path::calculateCellPath()
{
    if (!isMacro) return;
    pather->Reset();
    isMacro = true;
    CellIndex startCellIndex = startPosition.getCell();
    CellIndex endCellIndex = endPosition.getCell();
    void* startState = positionToState(startCellIndex.x - cellCoordOrigin.x,
    startCellIndex.y - cellCoordOrigin.y);
    void* endState = positionToState(endCellIndex.x - cellCoordOrigin.x,
    endCellIndex.y - cellCoordOrigin.y);

    //Find the most optimal macro path
    float totalCost = 0;
    int result = pather->Solve(startState, endState, &cellPath, &totalCost);
    if (result==MICROPATHER_FAILED)
    {
        std::cout << "Micropather failed\n";
        status = -1;
        return;
    }
    estimatedDifficulty = totalCost;
    
    //std::cout << "Total cost: " << totalCost << "\n";
    if (cellPath.empty()) //No cells to transfer; go straight to goal
    {
        currentGoalCell = endCellIndex;
        currentGoalCellIndex = 0;
        isGoalInCurrentCell = true;
        isTransferringCells = false;
        isMacro = false;
        return;
    }
    //Set the goal cell to the next cell in the path
    unsigned int x = 0;
    unsigned int y = 0;
    currentGoalCellIndex = 1;
    stateToPosition(cellPath[currentGoalCellIndex], x, y);
    //std::cout << "  State: " << x << " , " << y << "\n";
    //std::cout << "  Actual cell: " << (int)x + cellCoordOrigin.x << " , " << (int)y + cellCoordOrigin.y << "\n";
    currentGoalCell.x = (int)x + cellCoordOrigin.x;
    currentGoalCell.y = (int)y + cellCoordOrigin.y;
    isMacro = false;
    status = 0;
}
void Path::calculateTilePath()
{
    pather->Reset();
    isMacro = false;
    CellIndex currentCellIndex = currentPosition.getCell();
    CellIndex endCellIndex = currentGoalCell;
    
    //std::cout << "current" << currentCellIndex.x << " , " << currentCellIndex.y << " goal " << endCellIndex.x << " , " << endCellIndex.y << " origin " << cellCoordOrigin.x << " , " << cellCoordOrigin.y << "\n";
    //Get the relative start and end positions
    int currentTileOffsetX = currentPosition.getCellOffsetX() / TILE_WIDTH;
    int currentTileOffsetY = currentPosition.getCellOffsetY() / TILE_HEIGHT;
    int goalTileOffsetX = 0;
    int goalTileOffsetY = 0;
    isTransferringCells = false;
    if (isGoalInCurrentCell || (currentGoalCell.x == endPosition.getCell().x && currentGoalCell.y == endPosition.getCell().y))
    {
        //Set the tile offset to the true goal point in the cell
        goalTileOffsetX = endPosition.getCellOffsetX() / TILE_WIDTH;
        goalTileOffsetY = endPosition.getCellOffsetY() / TILE_HEIGHT;
    }
    else //Moving from one cell to the next; compute tile to target
    {
        int xDiff = endCellIndex.x - currentCellIndex.x;
        int yDiff = endCellIndex.y - currentCellIndex.y;

        //Select a tile that is closest to the current cell and centered
        //on the equal axis (and indent it)
        switch(xDiff)
        {
            case -1:
                goalTileOffsetX = CELL_WIDTH - PATH_CELL_TILE_INDENT;
                goalTileOffsetY = currentTileOffsetY;
                break;
            case 1:
                goalTileOffsetX = PATH_CELL_TILE_INDENT;
                goalTileOffsetY = currentTileOffsetY;
                break;
            default:
                break;
        }
        switch(yDiff)
        {
            case -1:
                goalTileOffsetX = currentTileOffsetX;
                goalTileOffsetY = CELL_HEIGHT - PATH_CELL_TILE_INDENT;
                break;
            case 1:
                goalTileOffsetX = currentTileOffsetX;
                goalTileOffsetY = PATH_CELL_TILE_INDENT;
                break;
            default:
                break;
        }
        isTransferringCells = true;
    }
    
    int currentTileX = ((currentCellIndex.x - cellCoordOrigin.x)
    * CELL_WIDTH) + currentTileOffsetX;
    int currentTileY = ((currentCellIndex.y - cellCoordOrigin.y)
    * CELL_HEIGHT) + currentTileOffsetY;
    
    int goalTileX = ((endCellIndex.x - cellCoordOrigin.x)
    * CELL_WIDTH) + goalTileOffsetX;
    int goalTileY = ((endCellIndex.y - cellCoordOrigin.y)
    * CELL_HEIGHT) + goalTileOffsetY;
    
    //std::cout << currentTileOffsetY << "\n";
    //std::cout << "current " << currentTileX << " , " << currentTileY << " goal " << goalTileX << " , " << goalTileY << "\n";
    void* startTileState = positionToState(currentTileX, currentTileY);
    void* endTileState = positionToState(goalTileX, goalTileY);
    //Find the most optimal micro path
    float totalCost = 0;
    int result = pather->Solve(startTileState, endTileState, &currentTilePath, &totalCost);
    if (result==MICROPATHER_FAILED)
    {
        std::cout << "Micropather failed\n";
        status = -1;
        return;
    }
    CellIndex currentIndex = currentPosition.getCell();
    Cell* currentCell = world->getCell(currentIndex);
    Cell* currentGoalCellPtr = world->getCell(currentGoalCell);
    if (!currentCell || !currentGoalCellPtr)
    {
        std::cout << "ERROR: calculateInGoal(): Cell " << currentIndex.x << " , " << currentIndex.y << " cannot be retrieved (or goal cell)\n";
        return;
    }
    status = 1;
}
void Path::calculate()
{
    if (status == 0)
    {
        if (isMacro) calculateCellPath();
        else calculateTilePath();
    }
}
Coord Path::advance(Coord& currentAdvPosition)
{
    if (status != 1) return currentAdvPosition; //If calculating etc, not ready to advance
    //Convert position to relative tile value
    int posTileX = ((currentAdvPosition.getCell().x - cellCoordOrigin.x) * CELL_WIDTH) + (currentAdvPosition.getCellOffsetX() / TILE_WIDTH);
    int posTileY = ((currentAdvPosition.getCell().y - cellCoordOrigin.y) * CELL_HEIGHT) + (currentAdvPosition.getCellOffsetY() / TILE_HEIGHT);
    //std::cout << "Object's current position (in rel tiles): " << posTileX << " , " << posTileY << "\n";
    //Get last tile in current path (to see if follower has reached the end)
    void* goalTile = currentTilePath[currentTilePath.size() - 1];
    unsigned int lastTileX = 0;
    unsigned int lastTileY = 0;
    stateToPosition(goalTile, lastTileX, lastTileY);
    if (posTileX == (int)lastTileX && posTileY == (int)lastTileY) //Reached end of current tile path
    {
        currentPosition = currentAdvPosition;
        currentTileIndex = 0;
        currentGoalCellIndex++;
        if (currentGoalCellIndex >= cellPath.size())
        {
            status = 2; //Goal reached (at end of cell path)
            return currentAdvPosition;
        }
        else
        {
            //Get next cell to transfer into
            unsigned int nextCellX = 0;
            unsigned int nextCellY = 0;
            stateToPosition(cellPath[currentGoalCellIndex], nextCellX, nextCellY);
            currentGoalCell.x = (int) nextCellX + cellCoordOrigin.x;
            currentGoalCell.y = (int) nextCellY + cellCoordOrigin.y;
            std::cout << currentGoalCellIndex << "\n";
        }
        status = 0; //Wait to calculate next tile path
    }
    else //Keep following current path
    {
        //Retrieve current goal tile
        unsigned int x = 0;
        unsigned int y = 0;
        stateToPosition(currentTilePath[currentTileIndex], x, y);
        //Follower reached goal tile; get next goal tile
        if (posTileX==(int)x && posTileY==(int)y) currentTileIndex++;
        //currentTileIndex++;
        if (currentTileIndex >= currentTilePath.size()) //TODO: Does this run? (it shouldn't)
        {
            status = 0;
            return currentAdvPosition;
        }
        unsigned int goalX = 0;
        unsigned int goalY = 0;
        stateToPosition(currentTilePath[currentTileIndex], goalX, goalY);
        //Determine coord of goal tile
        Coord nextGoalPosition;
        nextGoalPosition.setPosition((goalX / CELL_WIDTH) + cellCoordOrigin.x,
        (goalY / CELL_HEIGHT) + cellCoordOrigin.y, ((goalX % CELL_WIDTH) * TILE_WIDTH) + (TILE_WIDTH / 2), ((goalY % CELL_HEIGHT) * TILE_HEIGHT) + (TILE_HEIGHT / 2));
        //std::cout << "Next goal position:\n";
        //nextGoalPosition.print();
        return nextGoalPosition;
    }
    return currentAdvPosition;
}
void Path::stateToPosition(void* state, unsigned int& x, unsigned int& y)
{
    uintPtr castState = reinterpret_cast<uintPtr>(state);
    const unsigned int MASK = 0x0000FFFF;
    y = castState & MASK;
    castState= castState>>16;
    x = castState & MASK;
}
void* Path::positionToState(unsigned int x, unsigned int y)
{
    uintPtr positionToCast = 0;
    positionToCast |= x;
    positionToCast <<= 16;
    positionToCast |= y;
    return reinterpret_cast<void*>(positionToCast);
}
float Path::LeastCostEstimate(void* startNode, void* endNode)
{
    unsigned int x1 = 0;
    unsigned int y1 = 0;
    unsigned int x2 = 0;
    unsigned int y2 = 0;

    stateToPosition(startNode, x1, y1);
    stateToPosition(endNode, x2, y2);
    //Use Manhattan distance as estimate
    unsigned int xDiff = x2 - x1;
    unsigned int yDiff = y2 - y1;
    
    return (abs(xDiff) + abs(yDiff)) * MANHATTAN_MULTIPLIER;
}
void Path::AdjacentCost(void* currentNode, std::vector<micropather::StateCost>* neighbors)
{
    unsigned int x = 0;
    unsigned int y = 0;
    stateToPosition(currentNode, x, y);
    if (isMacro)
    {
        CellIndex currentNodeIndex;
        currentNodeIndex.x = x + cellCoordOrigin.x;
        currentNodeIndex.y = y + cellCoordOrigin.y;

        const int dx[] = {0, -1, 0, 1};
        const int dy[] = {-1, 0, 1, 0};
        for (int i = 0; i < 4; i++)
        {
            float difficulty = 1;
            CellIndex adjacentNodeIndex;
            adjacentNodeIndex.x = currentNodeIndex.x + dx[i];
            adjacentNodeIndex.y = currentNodeIndex.y + dy[i];
            Cell* adjacentCell = world->getCellIfExists(adjacentNodeIndex);
            if (adjacentCell) //Cell is cached; get exact difficulty
            {
                difficulty = adjacentCell->getDifficulty();
            }
            else //Cell not cached; estimate difficulty
            {
                difficulty = world->estimateCellDifficulty(adjacentNodeIndex);
            }
            //Multiply difficulty to amplify
            difficulty *= CELL_DIFFICULTY_SCALE;
            //Push this cell into neighbors
            void* adjacentState = positionToState(adjacentNodeIndex.x - cellCoordOrigin.x, adjacentNodeIndex.y - cellCoordOrigin.y);
            micropather::StateCost nodeCost = {adjacentState, difficulty};
            neighbors->push_back(nodeCost);
        }
    }
    else //isMicro
    {
        //TODO: Replace this with cache variable!
        CellIndex currentIndex = currentPosition.getCell();
        Cell* currentCell = world->getCell(currentIndex);
        if (!currentCell)
        {
            std::cout << "ERROR: AdjacentCost(): Cell " << currentIndex.x << " , " << currentIndex.y << " cannot be retrieved!\n";
            return;
        }
        Cell* currentGoalCellPtr = NULL;
        if (!isGoalInCurrentCell)
        {
            currentGoalCellPtr = world->getCell(currentGoalCell);
            if (!currentGoalCellPtr)
            {
                std::cout << "ERROR: AdjacentCost(): Cell " << currentIndex.x << " , " << currentIndex.y << " cannot be retrieved!\n";
                return;
            }
        }
        //Get the adjacent tiles
        const int dx[] = {0, -1, 0, 1};
        const int dy[] = {-1, 0, 1, 0};
        //const int dx[] = {0, -1, 0, 1, -1, -1, 1, 1}; //Allow diagonal
        //const int dy[] = {-1, 0, 1, 0, -1, 1, -1, 1};
        for (int i = 0; i < 4; i++)
        {
            int newX = x + dx[i];
            int newY = y + dy[i];
            //Convert new adjacent to a true Coord
            Coord adjacentPosition;
            adjacentPosition.setPosition((newX / CELL_WIDTH) + cellCoordOrigin.x,
            (newY / CELL_HEIGHT) + cellCoordOrigin.y, newX % CELL_WIDTH, newY % CELL_HEIGHT);
            bool insideCurrentCell = false;
            bool insideCurrentGoalCell = false;
            if (adjacentPosition.getCell().x == currentIndex.x && adjacentPosition.getCell().y == currentIndex.y)
            {
                insideCurrentCell = true;
            }
            if (adjacentPosition.getCell().x == currentGoalCell.x && adjacentPosition.getCell().y == currentGoalCell.y)
            {
                insideCurrentGoalCell = true;
            }
            //Adjacent tile is out of bounds; skip it
            if (!insideCurrentCell && !insideCurrentGoalCell)
            {
                continue;
            }
            Cell* thisTileCell = currentCell;
            if (insideCurrentGoalCell && !isGoalInCurrentCell)
            {
                thisTileCell = currentGoalCellPtr;
            }
            tile* groundTile = thisTileCell->getTileAt(adjacentPosition.getCellOffsetX(), adjacentPosition.getCellOffsetY(), 0);
            tile* onGroundTile = thisTileCell->getTileAt(adjacentPosition.getCellOffsetX(), adjacentPosition.getCellOffsetY(), 1);
            if (groundTile && onGroundTile)
            {
                float difficulty = 1;
                
                //TODO: Add func to determine if Water or solid (or just difficulty?)
                if (groundTile->x < 142 || onGroundTile->x != 255)
                {
                    //TODO: Calculate this 
                    difficulty = 10000;
                    //continue;
                }
    
                void* adjacentState = positionToState(newX, newY);
                micropather::StateCost nodeCost = {adjacentState, difficulty};
                neighbors->push_back(nodeCost);
            }
        }
    }
}
void Path::PrintStateInfo(void* currentNode)
{
    unsigned int x = 0;
    unsigned int y = 0;
    stateToPosition(currentNode, x, y);
    std::cout << "State is " << x << " , " << y << "\n";
}

#endif
