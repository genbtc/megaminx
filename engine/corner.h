#ifndef __CORNER_H__
#define __CORNER_H__
#include "piece.h"

class Corner : public Piece
{
public:
    Corner() = default;
    ~Corner() {}
	static void createAxis(int n, double* target);
	void init(int n, double* cornerVertexBase);
    void init(int n);
    void render();
};

#endif