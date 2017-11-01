#include "megaminx.h"
#include <cstdlib>

void Megaminx::solve()
{
    n = 0; 
    k = 0;
	_rSide = 0;
    rotating = false;    
	//store the value of the base start vertexes
    double* edgeVertexBase = edge[0].edgeInit();
    for (int i = 0; i < numEdges; ++i)
    {
        edge[i].init(i, edgeVertexBase);
    }
    double* cornerVertexBase = corner[0].cornerInit();
    for (int i = 0; i < numCorners; ++i)
    {
	    corner[i].init(i, cornerVertexBase);
    }
	initFacePieces();
}

void Megaminx::initFacePieces()
{
	double* centerVertexBase = face[0].faceInit();
	for (int i = 0; i < numFaces; ++i)
	{
		center[i].init(i);
		face[i].initCenter(center + i, centerVertexBase);
		face[i].initAxis(i);
		face[i].initEdge(edge[0], numEdges);
		face[i].initCorner(corner[0], numCorners);
	}
}

Megaminx::Megaminx()
{
	numFaces = sizeof(face) / sizeof(Face);
	numEdges = sizeof(edge) / sizeof(Edge);
	numCorners = sizeof(corner) / sizeof(Corner);
    solve();
}

void Megaminx::render()
{
	if (!rotating)
	{
		for (int i=0; i < numFaces; ++i)
			center[i].render();
		for (int i=0; i < numEdges; ++i)
			edge[i].render();
		for (int i=0; i < numCorners; ++i)
			corner[i].render();
	}
	else
	{
		for (int i=0, k=0; i < numFaces; ++i) {			
			if (&center[i] != face[_rSide].center)
				center[i].render();
		}
		for (int i=0, k=0; i < numEdges; ++i) {
			if (&edge[i] == face[_rSide].edge[k])
				k++;
			else
				edge[i].render();
		}
		for (int i=0, k=0; i < numCorners; ++i) {
			if (&corner[i] == face[_rSide].corner[k])
				k++;
			else
				corner[i].render();
		}
		if (face[_rSide].render()) {
			rotating = false;
		}
	}
}

void Megaminx::rotate(int num, int dir)
{
	if (!rotating) {
		rotating = true;
		_rSide = num;
		face[num].rotate(dir);
	}
}

void Megaminx::scramble()
{
    for (int i = 0; i < numFaces; i++) {
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
