#ifndef MST_HPP
#define MST_HPP

#include "graph.hpp"
#include <memory>
#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <utility>
#include <vector>


class MST {
public:
    MST(); // Default constructor

    MST(std::vector<std::vector<int>> _adj_matrix, std::string strategy);

    const Graph& get_graph() const;
    int get_total_weight() const ;
    std::vector<std::vector<int>> get_adj_matrix();

     //Function to calculate the weight matrix between all pairs of vertices
    std::vector<std::vector<int>> calculateWeightMatrix();

    // Function to print the MST graph
    //void print_graph() const;
     int calculate_total_weight(); // Helper to calculate total MST weight
     void setStrategy(std::string s);
     

private:
    unsigned int numVertices;
    std::vector<std::vector<int>> _adj_matrix;
    std::vector<std::vector<int>> alldistances;  // All distances between vertices
    int totalWeight;
    std::string strategy;

    void dfs_helper(int current, int target, int currentWeight, int& resultWeight, std::vector<bool>& visited);
   
};

#endif // MST_HPP
