#ifndef __EDGE_H__
#define __EDGE_H__
#include "piece.h"

class Edge : public Piece
{
public:
    Edge();
    ~Edge() {}

    void createAxis(int n, double* target);

    void init(int n);
    void render();

	void flip();
};

#endif 	