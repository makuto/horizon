#include <iostream>
#include "agent/species.hpp"

int main()
{
    Species testSpecies;
    testSpecies.updateAgent(testSpecies.createAgent());
    std::cout << "Hello world\n";
    return 1;
}
