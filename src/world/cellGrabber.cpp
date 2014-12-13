#include "cellGrabber.hpp"
#include "world.hpp"
#include "cell.hpp"

Cell* GetCell(CellIndex index, World* world)
{
    std::cout << "CellGrabber\n";
    return world->getCell(index);
}
