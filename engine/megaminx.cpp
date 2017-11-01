#include "megaminx.h"
#include <cstdlib>
#include <vector>

void Megaminx::solve()
{
    n = 0; 
    k = 0;
    _rotate = false;
    rSide = 0;

	for (int i = 0; i < 12; ++i)
	{
		center[i].init(i);
		face[i].initNum(i);
		face[i].initCenter(center + i);
		face[i].initAxis(i);
	}
    for (int i = 0; i < 30; ++i)
    {
        edge[i].init(i);
    }
    for (int i = 0; i < 20; ++i)
    {
        corner[i].init(i);
    }
	for (int i = 0; i < 12; ++i)
	{
		initFacePieces(i);
	}	
}

void Megaminx::initFacePieces(int i)
{
	switch (i + 1)
	{
	case WHITE:
		face[i].initEdge(edge, 0, 1, 2, 3, 4);
		face[i].initCorner(corner, 0, 1, 2, 3, 4);
		break;
	case BLUE:
		face[i].initEdge(edge, 0, 5, 9, 10, 15);
		face[i].initCorner(corner, 0, 4, 5, 9, 17);
		break;
	case RED:
		face[i].initEdge(edge, 1, 5, 6, 11, 16);
		face[i].initCorner(corner, 0, 1, 5, 6, 16);
		break;
	case GREEN:
		face[i].initEdge(edge, 2, 6, 7, 12, 17);
		face[i].initCorner(corner, 1, 2, 6, 7, 15);
		break;
	case PURPLE:
		face[i].initEdge(edge, 3, 7, 8, 13, 18);
		face[i].initCorner(corner, 2, 3, 7, 8, 19);
		break;
	case YELLOW:
		face[i].initEdge(edge, 4, 8, 9, 14, 19);
		face[i].initCorner(corner, 3, 4, 8, 9, 18);
		break;
	case GRAY:
		face[i].initEdge(edge, 25, 26, 27, 28, 29);
		face[i].initCorner(corner, 10, 11, 12, 13, 14);
		break;
	case LIGHT_BLUE:
		face[i].initEdge(edge, 13, 17, 21, 22, 25);
		face[i].initCorner(corner, 7, 10, 11, 15, 19);
		break;
	case ORANGE:
		face[i].initEdge(edge, 14, 18, 22, 23, 26);
		face[i].initCorner(corner, 8, 11, 12, 18, 19);
		break;
	case LIGHT_GREEN:
		face[i].initEdge(edge, 10, 19, 23, 24, 27);
		face[i].initCorner(corner, 9, 12, 13, 17, 18);
		break;
	case PINK:
		face[i].initEdge(edge, 11, 15, 20, 24, 28);
		face[i].initCorner(corner, 5, 13, 14, 16, 17);
		break;
	case BONE:
		face[i].initEdge(edge, 12, 16, 20, 21, 29);
		face[i].initCorner(corner, 6, 10, 14, 15, 16);
		break;
	default:
		break;
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
	std::vector<int> edgelist;
	for (int i = 0; i < 30; ++i) {
		if ((edge[i].data._colorNum[0] == n) || 
			(edge[i].data._colorNum[1] == n))
			edgelist.push_back(i);
	}
	return edgelist.size();
}

void Megaminx::grayStar()
{
//	face[GRAY-1].edge
}
