#ifndef __CORNER_H__
#define __CORNER_H__
#include "piece.h"

class Corner : public Piece
{
public:
    Corner();
    void createAxis(int n, double* target);
    ~Corner() {}

    void init(int n);
    void render();

    private:
	bool inited;
	bool turn;
};

#endif