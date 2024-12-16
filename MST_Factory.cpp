#include "MST_factory.hpp"

MST* MST_factory::createMST(const std::vector<std::vector<int>>& _adj_matrix, int n, const std::string& strategy) {
    MST_strategy mst;
    //std::cout << "making a factory\n";
    if (strategy == "prim") {
        return new MST(mst.prim(_adj_matrix, n).get_adj_matrix(), "prim");
    } else if (strategy == "kruskal") {
        return new MST(mst.kruskal(_adj_matrix, n).get_adj_matrix(), "kruskal");
    } else {
        return nullptr;
    }
}
