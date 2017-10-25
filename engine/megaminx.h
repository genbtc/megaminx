#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"
#include "center.h"

class Megaminx
{
public:
    void solve();
	Megaminx();
	~Megaminx();

	void render();
	void rotate(int, int);
	void scramble();
    void swapOneCorner(int i, int x);
    void swapOneEdge(int i, int x);
    /* y axis */
	double n;
    /* x axis */
	double k;
	bool _rotate;
private:

	int rSide;

	Face face[12];
	Center center[12];
	Edge edge[30];
	Corner corner[20];
};

#endif