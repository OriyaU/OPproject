#ifndef EDGE_HPP
#define EDGE_HPP

#include <memory>

class Edge {
public:
    Edge(unsigned int u, unsigned int v, int weight);

    unsigned int u() const;
    unsigned int v() const;
    int weight() const;
    unsigned int get_other(unsigned int vertex) const;

    // Comparator for sorting edges based on weight
    static bool compare(const std::shared_ptr<Edge>& a, const std::shared_ptr<Edge>& b);

private:
    unsigned int _u;
    unsigned int _v;
    int _weight;
};

#endif // EDGE_HPP
