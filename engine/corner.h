#ifndef __CORNER_H__
#define __CORNER_H__
#include "piece.h"

class Corner : Piece
{
public:
    Corner();
    ~Corner(){}

    void init(int n);    
    void render();

	double* color();
	void flip();
	void flipBack();

private:

	double _color[3][3];
	double _vertex[7][3];
	bool inited;
	bool turn;
	double angle;
	double axis[3];
};

#endif