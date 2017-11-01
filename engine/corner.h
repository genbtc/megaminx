#ifndef __CORNER_H__
#define __CORNER_H__
#include "piece.h"

class Corner : public Piece
{
public:

    Corner();
    static void createAxis(int n, double* target);
    void init(int n, double* cornerVertexBase);
    ~Corner() {}

    void init(int n);
    void render();
};

#endif