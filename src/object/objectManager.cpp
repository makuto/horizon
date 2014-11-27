#ifndef OBJECTMANAGER_CPP
#define OBJECTMANAGER_CPP
#include "objectManager.hpp"
#include <vector>
#include <iostream>
#include <base2.0/collision/collision.hpp>
ObjectManager::ObjectManager()
{
    head = new QuadTree<int>(2, 0, 0, 100, 100);
    std::cout << "gen\n";
    for (int i = 0; i < 30; ++i)
    {
        std::cout << "inserting\n";
        //head->insert(rand() % 100, rand() % 100, rand() % 100);
        if (!head->insert(i, 30, 50)) break;
    }
    std::cout << "done\n";
    std::vector<int> results;
    aabb range(4, 4, 100, 100);
    std::cout << "\nQuerying...\n";
    unsigned int resultCount = head->queryRange(range, results);
    std::cout << "Query returned " << resultCount << " points:\n";
    if (resultCount)
    {
        for (unsigned int i = 0; i < resultCount; ++i)
        {
            std::cout << results[i] << "\n";
        }
    }
    delete head;
}
//TESTING
int main()
{
    ObjectManager test;
}

#endif
