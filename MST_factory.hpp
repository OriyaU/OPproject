#ifndef MST_FACTORY_HPP
#define MST_FACTORY_HPP

#include "MST.hpp"
#include "MST_strategy.hpp"
#include <vector>
#include <string>

class MST_factory {
public:
    MST_factory() {}
    MST* createMST(const std::vector<std::vector<int>>& _adj_matrix, int n, const std::string& strategy);
};

#endif // MST_FACTORY_HPP
