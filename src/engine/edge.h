#ifndef __EDGE_H__
#define __EDGE_H__
#include "piece.h"

class Edge : public Piece {
public:
    Edge() = default;
    ~Edge() {}
    void createAxis(int n, double* target);
    void init(int n, double* edgeVertexBase);
    void init(int n, bool doAxes = true);
    void render() const;
};

#endif
