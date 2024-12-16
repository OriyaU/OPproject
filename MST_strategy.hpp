#ifndef MST_STRATEGY_HPP
#define MST_STRATEGY_HPP

#include <algorithm>
#include <iostream>
#include <tuple>
#include <utility>
#include "unionfind.hpp"
#include <sstream>
#include "MST.hpp"
#include <chrono>
#include <iostream>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

class MST_strategy {
public:
    MST prim(const std::vector<std::vector<int>>& _adj_matrix, int n);
    MST kruskal(const std::vector<std::vector<int>>& _adj_matrix, int n);
};

#endif // MST_STRATEGY_HPP
