#include "megaminx.h"
#include <cstdlib>
#include <vector>

void Megaminx::solve()
{
    n = 0; 
    k = 0;
    _rotate = false;
    rSide = 0;
	double* edgeVertexBase = edge[0].edgeInit();
    for (int i = 0; i < 30; ++i)
    {
        edge[i].init(i, edgeVertexBase);
    }
	double* cornerVertexBase = corner[0].cornerInit();
    for (int i = 0; i < 20; ++i)
    {
	    corner[i].init(i, cornerVertexBase);
    }
	initFacePieces();
}

void Megaminx::initFacePieces()
{
	for (int i = 0; i < 12; ++i)
	{
		center[i].init(i);
		face[i].initNum(i);
		face[i].initCenter(center + i);
		face[i].initAxis(i);
		face[i].initEdge(edge[0],sizeof(edge) / sizeof(Edge));
		face[i].initCorner(corner[0], sizeof(corner) / sizeof(Corner));
	}
}

Megaminx::Megaminx()
{
    solve();
}

Megaminx::~Megaminx()
{
}

void Megaminx::render()
{
	if (!_rotate)
	{
		for (int i=0; i < 12; ++i)
			center[i].render();
		for (int i=0; i < 30; ++i)
			edge[i].render();
		for (int i=0; i < 20; ++i)
			corner[i].render();
	}
	else
	{
		for (int i=0, k=0; i < 12; ++i) {			
			if (&center[i] != face[rSide].center)
				center[i].render();
		}
		for (int i=0, k=0; i < 30; ++i) {
			if (&edge[i] == face[rSide].edge[k])
				k++;
			else
				edge[i].render();
		}
		for (int i=0, k=0; i < 20; ++i) {
			if (&corner[i] == face[rSide].corner[k])
				k++;
			else
				corner[i].render();
		}
		if (face[rSide].render()) {
			_rotate = false;
		}
	}
}

void Megaminx::rotate(int num, int dir)
{
	if (!_rotate) {
		_rotate = true;
		rSide = num;
		face[num].rotate(dir);
	}
}

void Megaminx::scramble()
{
    for (int i = 0; i < 12; i++) {
        const int r = rand() % 2 * 2 - 1;
        face[i].placeParts(r);
    }
}

void Megaminx::swapOneCorner(int i, int x)
{
	face[i].corner[x]->flip(true);
}

void Megaminx::swapOneEdge(int i,int x)
{    
	face[i].edge[x]->flip(false);
}

void Megaminx::setCurrentFace(int i)
{
	g_currentFace = &face[i];
}

//sample temp. no good.
int Megaminx::resetFace(int n)
{

}

void Megaminx::grayStar()
{
//	face[GRAY-1].edge
}
