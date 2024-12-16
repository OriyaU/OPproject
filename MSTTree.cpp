#include "MSTTree.hpp"
#include <limits>
#include <vector>
#include <queue>
#include <numeric>
#include <algorithm>
#include <functional>

// Calculate the total weight of the MST
int MSTTree::getTotalWeight( MST& mst) {
    int x=mst.calculate_total_weight();
    return x;
}

// Find the longest distance in the MST using a double DFS
int MSTTree::getLongestDistance(MST& mst) {
    auto adjMatrix = mst.get_adj_matrix();
    unsigned int n = adjMatrix.size();

    if (n == 0) {
        return 0;
    }

    // Helper function for DFS to calculate distances
    auto dfs = [&](unsigned int start, int& farthestNode, std::vector<int>& distances) {
        std::vector<bool> visited(n, false);
        std::function<void(int, int)> dfsRecursive = [&](int node, int currentDistance) {
            visited[node] = true;
            distances[node] = currentDistance;
            for (unsigned int i = 0; i < n; ++i) {
                if (adjMatrix[node][i] != NO_EDGE && !visited[i]) {
                    dfsRecursive(i, currentDistance + adjMatrix[node][i]);
                }
            }
        };

        dfsRecursive(start, 0);

        // Find the farthest node
        farthestNode = std::distance(distances.begin(), std::max_element(distances.begin(), distances.end()));
    };

    int farthestNode = 0;
    std::vector<int> distances(n, 0);

    // First DFS to find the farthest node from an arbitrary start
    dfs(0, farthestNode, distances);

    // Second DFS from the farthest node to calculate the longest path
    std::fill(distances.begin(), distances.end(), 0);
    dfs(farthestNode, farthestNode, distances);

    return *std::max_element(distances.begin(), distances.end());
}

// Calculate the average distance between all pairs of vertices in the MST
double MSTTree::getAverageDistance(MST& mst) {
    auto weightMatrix = mst.calculateWeightMatrix();
    unsigned int n = weightMatrix.size();

    if (n == 0) {
        return 0.0;
    }

    unsigned long long totalDistance = 0;
    unsigned int count = 0;

    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            if (weightMatrix[i][j] != -1) {
                totalDistance += weightMatrix[i][j];
                ++count;
            }
        }
    }

    return static_cast<double>(totalDistance) / count;
}

// Find the shortest distance between two nodes in the MST
int MSTTree::getShortestDistance(MST& mst) {
    auto weightMatrix = mst.calculateWeightMatrix();
    unsigned int n = weightMatrix.size();

    if (n == 0) {
        return 0;
    }

    int shortestDistance = std::numeric_limits<int>::max();
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = i + 1; j < n; ++j) {
            if (weightMatrix[i][j] != -1) {
                shortestDistance = std::min(shortestDistance, weightMatrix[i][j]);
            }
        }
    }

    return shortestDistance == std::numeric_limits<int>::max() ? 0 : shortestDistance;
}
