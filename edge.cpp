#include "edge.hpp"

Edge::Edge(unsigned int u, unsigned int v, int weight)
    : _u(u), _v(v), _weight(weight) {}

unsigned int Edge::u() const {
    return _u;
}

unsigned int Edge::v() const {
    return _v;
}

int Edge::weight() const {
    return _weight;
}

unsigned int Edge::get_other(unsigned int vertex) const {
    if (vertex == _u) {
        return _v;
    } else if (vertex == _v) {
        return _u;
    } else {
        return static_cast<unsigned int>(-1);
    }
}

bool Edge::compare(const std::shared_ptr<Edge>& a, const std::shared_ptr<Edge>& b) {
    return a->_weight < b->_weight;
}
