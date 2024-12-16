#include "MST.hpp"

// Default constructor
MST::MST() : numVertices(0), totalWeight(0), strategy("") {}

// Constructor with adjacency matrix and strategy
MST::MST(std::vector<std::vector<int>> _adj_matrix, std::string strategy)
{
    this->_adj_matrix = _adj_matrix;
    numVertices = _adj_matrix.size();
    alldistances = calculateWeightMatrix();
    this->strategy = strategy;
}

// Get the underlying graph (as Graph object)
const Graph &MST::get_graph() const
{
    static Graph graph(_adj_matrix.size());
    for (unsigned int i = 0; i < _adj_matrix.size(); ++i)
    {
        for (unsigned int j = 0; j < _adj_matrix[i].size(); ++j)
        {
            if (_adj_matrix[i][j] != NO_EDGE)
            {
                graph.add_edge(i, j, _adj_matrix[i][j]);
            }
        }
    }
    return graph;
}

//Get the total weight of the MST
int MST::get_total_weight() const {
   return totalWeight;
}

// Get the adjacency matrix of the MST
std::vector<std::vector<int>> MST::get_adj_matrix()
{

    return _adj_matrix;
}

// Helper to calculate the total weight of the MST
int MST::calculate_total_weight()
{

    totalWeight = 0;
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        for (unsigned int j = i + 1; j < numVertices; ++j)
        { // Avoid double-counting edges
            if (_adj_matrix[i][j] != NO_EDGE)
            {
                totalWeight += _adj_matrix[i][j];
                //  std::cout << "updates"<<totalWeight<<"/n";
            }
        }
    }
    MST::totalWeight = totalWeight;
    return totalWeight;
}

void MST::setStrategy(std::string s)
{
    this->strategy = s;
}

// DFS helper function to calculate path weight between two vertices
void MST::dfs_helper(int current, int target, int currentWeight, int &resultWeight, std::vector<bool> &visited)
{
    if (current == target)
    {
        resultWeight = currentWeight;
        return;
    }

    visited[current] = true;

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        if (_adj_matrix[current][i] != NO_EDGE && !visited[i])
        {
            dfs_helper(i, target, currentWeight + _adj_matrix[current][i], resultWeight, visited);
            if (resultWeight != -1)
            { // Stop further DFS if target is found
                return;
            }
        }
    }
}

// Calculate the weight matrix for all pairs of vertices
std::vector<std::vector<int>> MST::calculateWeightMatrix()
{
    alldistances = std::vector<std::vector<int>>(numVertices, std::vector<int>(numVertices, -1));

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        for (unsigned int j = 0; j < numVertices; ++j)
        {
            if (i != j)
            {
                std::vector<bool> visited(numVertices, false);
                int pathWeight = -1;
                dfs_helper(i, j, 0, pathWeight, visited);
                alldistances[i][j] = pathWeight;
            }
            else
            {
                alldistances[i][j] = 0; // Distance to self is zero
            }
        }
    }
    return alldistances;
}

// Print the MST graph
// void MST::print_graph() const
// {
//     std::cout << "Minimum Spanning Tree (" << strategy << "):\n";
//     for (unsigned int i = 0; i < numVertices; ++i)
//     {
//         for (unsigned int j = i + 1; j < numVertices; ++j)
//         { // Avoid double-printing edges
//             if (_adj_matrix[i][j] != NO_EDGE)
//             {
//                 std::cout << "Edge (" << i << " - " << j << ") Weight: " << _adj_matrix[i][j] << "\n";
//             }
//         }
//     }
//     //  std::cout << "Total Weight: " << totalWeight << "\n";
// }
