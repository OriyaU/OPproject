#ifndef MSTTREE_HPP
#define MSTTREE_HPP

#include "graph.hpp"
#include "MST.hpp"
#include <memory>

class MSTTree {
public:
    int getTotalWeight( MST& mst);
    int getLongestDistance(MST& mst);
    double getAverageDistance( MST& mst);
    int getShortestDistance(MST& mst);
};
#endif // MSTTREE_HPP
