#ifndef __CORNER_H__
#define __CORNER_H__
#include "piece.hpp"

class Corner : public Piece {
public:
    Corner() = default;
    ~Corner() {}
    void createAxis(int n, double* target);
    void init(int n, const double *cornerVertexBase);
    void init(int n, bool doAxes = true);
    void render() const;
};

#endif
