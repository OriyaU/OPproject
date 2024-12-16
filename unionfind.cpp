#include "unionfind.hpp"

UnionFind::UnionFind(int size) : parent(size), rank(size, 0) {
    for (int i = 0; i < size; ++i) {
        parent[i] = i;
    }
}

int UnionFind::find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]); // Path compression
    }
    return parent[x];
}

void UnionFind::unite(int x, int y) {
    int root_x = find(x);
    int root_y = find(y);

    if (root_x != root_y) {
        if (rank[root_x] > rank[root_y]) {
            parent[root_y] = root_x;
        } else if (rank[root_x] < rank[root_y]) {
            parent[root_x] = root_y;
        } else {
            parent[root_y] = root_x;
            rank[root_x]++;
        }
    }
}
