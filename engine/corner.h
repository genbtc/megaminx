#ifndef __CORNER_H__
#define __CORNER_H__
#include "piece.h"

class Corner : public Piece
{
public:
    Corner();
    ~Corner(){}

    void init(int n);    
    void render();

	void flip();
	void flipBack();

private:
	bool inited;
	bool turn;
	double angle;
	double axis[3];
};

#endif