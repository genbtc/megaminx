#ifndef __EDGE_H__
#define __EDGE_H__
#include "piece.h"

class Edge : Piece
{
public:
    Edge();
    ~Edge() {}

    void init(int n);
    void render();
	double* color();

	void flip();
};

#endif 	