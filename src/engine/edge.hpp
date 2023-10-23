#ifndef __EDGE_H__
#define __EDGE_H__
#include "piece.hpp"

class Edge : public Piece {
public:
    Edge() = default;
    ~Edge() = default;
    void createAxis(int n, double* target);
    void init(int n, double* edgeVertexBase);
    void init(int n, bool doAxes = true);
    void render() const;
};

#endif
