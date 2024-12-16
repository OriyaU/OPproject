#include "MST_strategy.hpp"
#include <stdexcept>
#include <string>

// Prim's Algorithm
MST MST_strategy::prim(const std::vector<std::vector<int>>& _adj_matrix, int n) {

   // std::cout << "starting calculating mst prim"<<_adj_matrix[0][1]<<"\n";
    std::vector<bool> inMST(n, false);           // Track nodes included in the MST
    std::vector<int> key(n, std::numeric_limits<int>::max()); // Minimum weights to include each vertex
    std::vector<int> parent(n, -1);             // To store the MST structure
    key[0] = 0;                                 // Start from node 0
    int totalWeight = 0;

    for (int count = 0; count < n; ++count) {
        // Find the vertex with the smallest key value not yet in MST
        int minKey = std::numeric_limits<int>::max();
        int u = -1;
        for (int i = 0; i < n; ++i) {
            if (!inMST[i] && key[i] < minKey) {
                minKey = key[i];
                u = i;
            }
        }

        if (u == -1) {
            throw std::runtime_error("Graph is disconnected; MST cannot be created");
        }

        // Add the selected vertex to the MST
        inMST[u] = true;
        totalWeight += key[u];

        // Update key values for adjacent vertices
        for (int v = 0; v < n; ++v) {
            if (v == u) continue; // Skip self-loops
            int weight = _adj_matrix[u][v];
            if (weight != NO_EDGE && !inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
            }
        }
    }

    // Build the MST adjacency matrix
    std::vector<std::vector<int>> mstAdjMatrix(n, std::vector<int>(n, NO_EDGE));
    for (int i = 1; i < n; ++i) {
        if (parent[i] != -1) {
            mstAdjMatrix[i][parent[i]] = _adj_matrix[i][parent[i]];
            mstAdjMatrix[parent[i]][i] = _adj_matrix[parent[i]][i];
        }
    }
    //std::cout << "finish calculating mst prim"<<"\n";


    return MST(mstAdjMatrix, "Prim");
}

// Kruskal's Algorithm
MST MST_strategy::kruskal(const std::vector<std::vector<int>>& _adj_matrix, int n) {
    std::vector<std::tuple<int, int, int>> edges; // (weight, u, v)

    // Extract all edges from the adjacency matrix
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) { // Avoid duplicate edges for undirected graph
            int weight = _adj_matrix[u][v];
            if (weight != NO_EDGE && u != v) { // Skip NO_EDGE and self-loops
                edges.emplace_back(weight, u, v);
            }
        }
    }

    // Sort edges by weight in non-decreasing order
    std::sort(edges.begin(), edges.end());

    UnionFind uf(n);                           // Union-Find structure for cycle detection
    std::vector<std::vector<int>> mstAdjMatrix(n, std::vector<int>(n, NO_EDGE));
    int totalWeight = 0;

    for (const auto& [weight, u, v] : edges) {
        if (uf.find(u) != uf.find(v)) {       // If adding this edge doesn't form a cycle
            uf.unite(u, v);                   // Merge the two sets
            mstAdjMatrix[u][v] = weight;      // Add the edge to the MST
            mstAdjMatrix[v][u] = weight;      // Undirected graph
            totalWeight += weight;
        }
    }

    return MST(mstAdjMatrix, "Kruskal");
}
