#ifndef __EDGE_H__
#define __EDGE_H__
#include "piece.h"

class Edge : public Piece
{
public:
    Edge();
    ~Edge() {}

    void init(int n);
    void render();

	void flip();
    void swapEdges(int n, int k);
};

#endif 	