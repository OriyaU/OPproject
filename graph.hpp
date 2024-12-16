#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "edge.hpp"
#include <vector>
#include <limits>
#include <memory>
#include <string>
#include <sstream>

// Use -1 to represent NO_EDGE consistently
#define NO_EDGE -1

class Graph {
public:
    Graph(); // Default constructor
    explicit Graph(unsigned int n);
    ~Graph();

    unsigned int num_vertices() const;
    void add_edge(unsigned int u, unsigned int v, int weight);
  //  void add_edge_from_edge(const std::shared_ptr<Edge>& e);
    void remove_edge(unsigned int u, unsigned int v);
    bool has_edge(unsigned int u, unsigned int v) const;
    //int get_weight(unsigned int u, unsigned int v) const;
    std::vector<std::vector<int>>& get_adj_matrix();

  //  std::string print() const;

private:
    unsigned int _num_vertices;
    std::vector<std::vector<int>> _adj_matrix;
};

#endif // GRAPH_HPP
