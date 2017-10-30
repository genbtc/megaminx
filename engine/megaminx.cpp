#include "megaminx.h"
#include <cstdlib>

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
	switch (i+1)
	{
	case 1:
		face[i].initEdge(edge, edge + 1, edge + 2, edge + 3, edge + 4);
		face[i].initCorner(corner, corner + 1, corner + 2, corner + 3, corner + 4);
		break;
	case 2:
		face[i].initEdge(edge + 0, edge + 5, edge + 9, edge + 10, edge + 15);
		face[i].initCorner(corner, corner + 4, corner + 5, corner + 9, corner + 17);
		break;
	case 3:
		face[i].initEdge(edge + 1, edge + 5, edge + 6, edge + 11, edge + 16);
		face[i].initCorner(corner, corner + 1, corner + 5, corner + 6, corner + 16);
		break;
	case 4:
		face[i].initEdge(edge + 2, edge + 6, edge + 7, edge + 12, edge + 17);
		face[i].initCorner(corner + 1, corner + 2, corner + 6, corner + 7, corner + 15);
		break;
	case 5:
		face[i].initEdge(edge + 3, edge + 7, edge + 8, edge + 13, edge + 18);
		face[i].initCorner(corner + 2, corner + 3, corner + 7, corner + 8, corner + 19);
		break;
	case 6:
		face[i].initEdge(edge + 4, edge + 8, edge + 9, edge + 14, edge + 19);
		face[i].initCorner(corner + 3, corner + 4, corner + 8, corner + 9, corner + 18);
		break;
	case 7:
		face[i].initEdge(edge + 25, edge + 26, edge + 27, edge + 28, edge + 29);
		face[i].initCorner(corner + 10, corner + 11, corner + 12, corner + 13, corner + 14);
		break;
	case 8:
		face[i].initEdge(edge + 13, edge + 17, edge + 21, edge + 22, edge + 25);
		face[i].initCorner(corner + 7, corner + 10, corner + 11, corner + 15, corner + 19);
		break;
	case 9:
		face[i].initEdge(edge + 14, edge + 18, edge + 22, edge + 23, edge + 26);
		face[i].initCorner(corner + 8, corner + 11, corner + 12, corner + 18, corner + 19);
		break;
	case 10:
		face[i].initEdge(edge + 10, edge + 19, edge + 23, edge + 24, edge + 27);
		face[i].initCorner(corner + 9, corner + 12, corner + 13, corner + 17, corner + 18);
		break;
	case 11:
		face[i].initEdge(edge + 11, edge + 15, edge + 20, edge + 24, edge + 28);
		face[i].initCorner(corner + 5, corner + 13, corner + 14, corner + 16, corner + 17);
		break;
	case 12:
		face[i].initEdge(edge + 12, edge + 16, edge + 20, edge + 21, edge + 29);
		face[i].initCorner(corner + 6, corner + 10, corner + 14, corner + 15, corner + 16);
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
        int r = rand() % 2 * 2 - 1;
        this->face[i].placeParts(1);
    }
}

//works good.
void Megaminx::swapOneCorner(int i, int x)
{
    this->face[i].corner[x]->flip();
}
//Front Face is 7 (Blue)
//works good.
void Megaminx::swapOneEdge(int i,int x)
{    
    this->face[i].edge[x]->flip();
}

void Megaminx::setCurrentFace(int i)
{
	g_currentFace = &this->face[i];
}

void Megaminx::resetFace(int i)
{
	auto corner0 = face[i].corner[0]->_colorNum;
	auto corner1 = face[i].corner[1]->_colorNum;
	auto corner2 = face[i].corner[2]->_colorNum;
	auto corner3 = face[i].corner[3]->_colorNum;
	auto corner4 = face[i].corner[4]->_colorNum;
	auto edge0 = face[i].edge[0]->_colorNum;
	auto edge1 = face[i].edge[1]->_colorNum;
	auto edge2 = face[i].edge[2]->_colorNum;
	auto edge3 = face[i].edge[3]->_colorNum;
	auto edge4 = face[i].edge[4]->_colorNum;
}
