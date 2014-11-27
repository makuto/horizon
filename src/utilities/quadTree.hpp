#ifndef QUADTREE_HPP
#define QUADTREE_HPP
#include <vector>
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <base2.0/collision/collision.hpp>
#include <iostream>

const unsigned int MAX_TREE_DEPTH = 10;
//QuadPoints store the coordinate of the data along with the data itself
template<class R>
class QuadPoint
{
    public:
        float x;
        float y;
        R data;
};
/* --QuadTree--
 * Template spacial quadtree
 * I referred to http://en.wikipedia.org/wiki/Quadtree while coding this
 * */
template<class T>
class QuadTree
{
    protected:
        QuadTree* tL;
        QuadTree* tR;
        QuadTree* bL;
        QuadTree* bR;
        aabb bounds;

        int maxCapacity;
        std::vector<QuadPoint<T> > data;
        
        QuadTree(unsigned int newMaxCapacity, float newX, float newY, float newWidth, float newHeight, unsigned int newDepth)
        {
            depth = newDepth + 1;
            std::cout << "["<< depth <<"]node created\n";
            maxCapacity = newMaxCapacity;
            tL=NULL;
            tR=NULL;
            bL=NULL;
            bR=NULL;
            bounds.setPosition(newX, newY);
            bounds.resize(newWidth, newHeight);
        }
    private:
        //If a point cannot be handled when inserted from a subdivision,
        //don't try to subdivide (problem if [maxCapacity + 1] points are placed
        //in the exact same place). Node will contain more than maxCapacity if
        //this is the case. Not the most elegant, but it segfaults otherwise :)
        bool subdivideInsert(T newData, float x, float y)
        {
            std::cout << this << "\n";
            std::cout << "subdiv insert bounds: " << bounds.x << " , " << bounds.y << " w " << bounds.w << " h " << bounds.h;
            std::cout << "; is in range: ";
            //Make sure point is within the bounds of this node
            if (!isPointInRange(x, y, bounds))
            {
                std::cout << "rejected b/c bounds\n";
                //Point is not within bounds
                return false;
            }
            
            //Node isn't full yet, so add data to this node
            if (data.size() < maxCapacity)
            {
                QuadPoint<T> newPoint;
                newPoint.x = x;
                newPoint.y = y;
                newPoint.data = newData;
                data.push_back(newPoint);
                std::cout << "data added\n";
                return true;
            }

            //Node is already full; don't try to add any more
            std::cout << "rejected b/c full\n";
            return false;
        }
        void subdivide()
        {
            float halfWidth = bounds.w/2;
            float halfHeight = bounds.h/2;
            tL = new QuadTree<T>(maxCapacity, bounds.x, bounds.y, halfWidth, halfHeight, depth); 
            tR = new QuadTree<T>(maxCapacity, bounds.x + halfWidth, bounds.y, halfWidth, halfHeight, depth); 
            bL = new QuadTree<T>(maxCapacity, bounds.x, bounds.y + halfHeight, halfWidth, halfHeight, depth); 
            bR = new QuadTree<T>(maxCapacity, bounds.x + halfWidth, bounds.y + halfHeight, halfWidth, halfHeight, depth);

            //Redistribute points into child nodes
            while(!data.empty())
            {
                std::cout << "REDIS\n";
                QuadPoint<T> newDataPoint = data.back();
                T newData = newDataPoint.data;
                float x = newDataPoint.x;
                float y = newDataPoint.y;
                std::cout << "redis tl\n";
                if (tL->subdivideInsert(newData, x, y))
                {
                    std::cout << "success\n";
                    data.pop_back();
                    continue;
                }
                std::cout << "redis tr\n";
                if (tR->subdivideInsert(newData, x, y))
                {
                    std::cout << "success\n";
                    data.pop_back();
                    continue;
                }
                std::cout << "redis bl\n";
                if (bL->subdivideInsert(newData, x, y))
                {
                    std::cout << "success\n";
                    data.pop_back();
                    continue;
                }
                std::cout << "redis br\n";
                if (bR->subdivideInsert(newData, x, y))
                {
                    std::cout << "success\n";
                    data.pop_back();
                    continue;
                }
                //For some reason a point will not be accepted, so
                //stop redistributing
                std::cout << "[!] point not accepted\n";
                break;
            } 
        }
        bool isPointInRange(float x, float y, aabb& range)
        {
            std::cout << "Is " << x << " , " << y << " in range " << range.x << " , " << range.y << " w " << range.w << " h " << range.h << "?";
            //See if point is within range
            if (x <= range.x + range.w && x >= range.x && y <= range.y + range.h && y >= range.y)
            {
                std::cout << " yes\n";
                return true;
            }
            std::cout << " nope\n";
            return false;
        }
    public:
        unsigned int depth;
        QuadTree(unsigned int newMaxCapacity, float newX, float newY, float newWidth, float newHeight)
        {
            depth = 1;
            std::cout << "["<< depth <<"]node created\n";
            maxCapacity = newMaxCapacity;
            tL=NULL;
            tR=NULL;
            bL=NULL;
            bR=NULL;
            bounds.setPosition(newX, newY);
            bounds.resize(newWidth, newHeight);
        }
        bool insert(T newData, float x, float y)
        {
            std::cout << "["<< depth <<"] " << newData << " at " << x << " , " << y << " \n";
            //Make sure point is within the bounds of this node
            if (!isPointInRange(x, y, bounds))
            {
                //Point is not within bounds
                return false;
            }
            
            //Node isn't full yet, so add data to this node
            if (!tL && data.size() < maxCapacity)
            {
                QuadPoint<T> newPoint;
                newPoint.x = x;
                newPoint.y = y;
                newPoint.data = newData;
                data.push_back(newPoint);
                return true;
            }

            //Safety max depth
            if (depth >= MAX_TREE_DEPTH)
            {
                std::cout << "\033[31mWARNING: Max tree depth (" << MAX_TREE_DEPTH << ") reached!\033[0m\n";
                //return false;
            }
            
            //Node is full; subdivide (if not already subdivided)
            if (!tL)
            {
                subdivide();
                if (tL->subdivideInsert(newData, x, y)) return true;
                if (tR->subdivideInsert(newData, x, y)) return true;
                if (bL->subdivideInsert(newData, x, y)) return true;
                if (bR->subdivideInsert(newData, x, y)) return true;
                //Point wouldn't be accepted by any nodes
                std::cout <<"point rejected; keeping\n";
                QuadPoint<T> newPoint;
                newPoint.x = x;
                newPoint.y = y;
                newPoint.data = newData;
                data.push_back(newPoint);
                return true;
            }

            //If already subdivided, try inserting into child nodes
            if (tL->insert(newData, x, y)) return true;
            if (tR->insert(newData, x, y)) return true;
            if (bL->insert(newData, x, y)) return true;
            if (bR->insert(newData, x, y)) return true;
            //Shouldn't ever happen
            std::cout << "This shouldn't happen\n";
            return false;
        }
        //Fills the provided vector with resultant points (points
        //contained in the specified range). Returns total results
        unsigned int queryRange(aabb& range, std::vector<T>& results)
        {
            std::cout << "querying\n";
            unsigned int totalResults = 0;
            //Make sure range is touching this node
            if (!isColliding(&range, &bounds))
            {
                std::cout << "range not touching\n";
                return 0;
            }
            std::cout << "range colliding\n";
            //Add points in this node to results if contained in range
            for (typename std::vector<QuadPoint<T> >::iterator it = data.begin(); it!=data.end(); ++it)
            {
                std::cout << "query:";
                if (isPointInRange((*it).x, (*it).y, range))
                {
                    results.push_back((*it).data);
                    totalResults++;
                }
            }
            std::cout << "Done with points in this node (size " << data.size() << "), checking children\n";

            //Let all child nodes (if any) add points
            if (!tL)
            {
                std::cout << " no children\n";
                return totalResults;
            }
            std::cout << "Children exist\n";
            totalResults += tL->queryRange(range, results);
            totalResults += tR->queryRange(range, results);
            totalResults += bL->queryRange(range, results);
            totalResults += bR->queryRange(range, results);
            std::cout << "Done checking children\n";
            
            return totalResults;
        }
        
};
#endif
