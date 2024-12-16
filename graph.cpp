#include "graph.hpp"
#include <iostream>
#include <iomanip>

Graph::Graph() : _num_vertices(0), _adj_matrix() {} // Default constructor

Graph::Graph(unsigned int n) : _num_vertices(n), _adj_matrix(n, std::vector<int>(n, NO_EDGE)) {}

Graph::~Graph() = default;

unsigned int Graph::num_vertices() const {
    return _num_vertices;
}

void Graph::add_edge(unsigned int u, unsigned int v, int weight) {
    if (u < _num_vertices && v < _num_vertices) {
        _adj_matrix[u][v] = weight;
        _adj_matrix[v][u] = weight; // Undirected graph
    }
}

// void Graph::add_edge_from_edge(const std::shared_ptr<Edge>& e) {
//     add_edge(e->u(), e->v(), e->weight());
// }

void Graph::remove_edge(unsigned int u, unsigned int v) {
    if (u < _num_vertices && v < _num_vertices) {
        _adj_matrix[u][v] = NO_EDGE;
        _adj_matrix[v][u] = NO_EDGE;
    }
}

bool Graph::has_edge(unsigned int u, unsigned int v) const {
    return u < _num_vertices && v < _num_vertices && _adj_matrix[u][v] != NO_EDGE;
}

// int Graph::get_weight(unsigned int u, unsigned int v) const {
//     if (u < _num_vertices && v < _num_vertices) {
//         return _adj_matrix[u][v];
//     }
//     return NO_EDGE;
// }

std::vector<std::vector<int>>& Graph::get_adj_matrix() {
    return _adj_matrix;
}

// std::string Graph::print() const {
//     std::ostringstream oss;

//     oss << "   ";
//     for (unsigned int i = 0; i < _num_vertices; i++) {
//         oss << std::setw(3) << i;
//     }
//     oss << "\n   ";
//     for (unsigned int i = 0; i < _num_vertices; i++) {
//         oss << "---";
//     }
//     oss << "\n";

//     for (unsigned int i = 0; i < _num_vertices; i++) {
//         oss << std::setw(2) << i << "|";
//         for (unsigned int j = 0; j < _num_vertices; j++) {
//             if (_adj_matrix[i][j] == NO_EDGE) {
//                 oss << std::setw(3) << 0; // Represent NO_EDGE as 0
//             } else {
//                 oss << std::setw(3) << _adj_matrix[i][j];
//             }
//         }
//         oss << "\n";
//     }

//     return oss.str();
// }
