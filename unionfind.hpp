#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <vector>

class UnionFind {
public:
    explicit UnionFind(int size);

    int find(int x);
    void unite(int x, int y);

private:
    std::vector<int> parent;
    std::vector<int> rank;
};

#endif // UNIONFIND_HPP
