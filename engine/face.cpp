#include <GL/glut.h>
#include <math.h>
#include "face.h"
#include "utils.h"
#include <iostream>
#include <vector>

Face::Face(): m_pos(0), m_radius(0)
{
    center = nullptr;
    turnDir = 0;
    thisNum = 0;
    _rotate = false;
    angle = 0;
    axis[0] = 0;
    axis[1] = 0.0001;
    axis[2] = -1;
}

std::vector<int> Face::initEdge(Edge& n)
{
	std::vector<int> edgelist;
    const auto color = center->data._colorNum;
    const auto edgeref = &n;
	int count = 0;
	for (int i = 0; i < 30; ++i)
	{
		if ((edgeref[i].data._colorNum[0] == color[0]) || 
			(edgeref[i].data._colorNum[1] == color[0]))
		{
			edge[count] = edgeref + i;
			count++;
			edgelist.push_back(i);
		} 
	}
	return edgelist;
}
void Face::initEdge(Edge *n, int a, int b, int c, int d, int e)
{
	edge[0] = n + a;
	edge[1] = n + b;
	edge[2] = n + c;
	edge[3] = n + d;
	edge[4] = n + e;
}

std::vector<int> Face::initCorner(Corner& n)
{
	std::vector<int> cornerlist;
	const auto color = center->data._colorNum;
	const auto cornerref = &n;
	int count = 0;
	for (int i = 0; i < 20; ++i)
	{
		if ((cornerref[i].data._colorNum[0] == color[0]) || 
			(cornerref[i].data._colorNum[1] == color[0]) || 
			(cornerref[i].data._colorNum[2] == color[0]))
		{
			corner[count] = cornerref + i;
			count++;
			cornerlist.push_back(i);
		} 
	}
	return cornerlist;
}
void Face::initCorner(Corner *n, int a, int b, int c, int d, int e)
{
	corner[0] = n + a;
	corner[1] = n + b;
	corner[2] = n + c;
	corner[3] = n + d;
	corner[4] = n + e;
}

void Face::initNum(int num)
{
    thisNum = num;
}

void Face::initCenter(Center *a)
{
    center = a;
    for (int i = 0; i < 5; ++i)
    {
        _vertex[i][0] = -INS_CIRCLE_RAD * cos(3 * PI / 10) + 100 / sin(2 * PI / 5) * 2 / 5;
        _vertex[i][1] = -INS_CIRCLE_RAD * sin(3 * PI / 10);
        _vertex[i][2] = -INS_SPHERE_RAD;
        rotateVertex(_vertex[i], 'z', 2 * PI / 5);
        rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
        rotateVertex(_vertex[i], 'z', 2 * i * PI / 5);
    }
}

/**
 * \brief create our Face
 * \param n We can re-use the Center axis creation
 */
void Face::initAxis(int n)
{
    center->createAxis(n,axis);
    for (int i = 0; i < 5; ++i)
    {
        center->createAxis(n,_vertex[i]);
    }
}

/**
 * \brief Simple-Flips (inverts) one Edge-piece 
 * and then the other, individually.
 */
void Face::twoEdgesFlip(int a,int b)
{
    edge[a]->flip(false);
	edge[b]->flip(false);
}

void Face::genericFlip(int a, int b, int c, int d, fourPack pack)
{
	pack.four[0] ? corner[a]->flip(true) : corner[a]->flipTwice(true);
	pack.four[1] ? corner[b]->flip(true) : corner[b]->flipTwice(true);
	pack.four[2] ? corner[c]->flip(true) : corner[c]->flipTwice(true);
	pack.four[3] ? corner[d]->flip(true) : corner[d]->flipTwice(true);
}
void Face::inwardsFlip(int a, int b, int c, int d)
{
    genericFlip(a, b, c, d, {0, 1, 1, 0});
}
void Face::outwardsFlip(int a, int b, int c, int d)
{
    genericFlip(a, b, c, d, {1, 0, 0, 1});
}
void Face::backwardsFlip(int a, int b, int c, int d)
{
    genericFlip(a, b, c, d, {0, 0, 1, 1});
}
void Face::forwardsFlip(int a, int b, int c, int d)
{
    genericFlip(a, b, c, d, {1, 1, 0, 0});
}
void Face::alternatingBackwardsFlip(int a, int b, int c, int d)
{
    genericFlip(a, b, c, d, {0, 1, 0, 1});
}
void Face::alternatingForwardsFlip(int a, int b, int c, int d)
{
    genericFlip(a, b, c, d, {1, 0, 1, 0});
}

void Face::QuadSwapCorners(eightPack pack)
{
    swapCorners(pack.eight[0], pack.eight[1]);
    swapCorners(pack.eight[2], pack.eight[3]);
    swapCorners(pack.eight[4], pack.eight[5]);
    swapCorners(pack.eight[6], pack.eight[7]);
}

void Face::QuadSwapEdges(eightPack pack)
{
    swapEdges(pack.eight[0], pack.eight[1]);
    swapEdges(pack.eight[2], pack.eight[3]);
    swapEdges(pack.eight[4], pack.eight[5]);
    swapEdges(pack.eight[6], pack.eight[7]);
}
/**
 * \brief 
 * \param right Each case is for each of the 12 faces, 
 * / in order to get it to switch colors after it rotates.
 */
bool Face::placeParts(int right)
{
    if (right == 1)
    {
        switch (thisNum)
        {
        case 0:
            QuadSwapCorners({ 0, 1, 1, 2, 2, 3, 3, 4 });
            QuadSwapEdges({ 0, 1, 1, 2, 2, 3, 3, 4 });
            break;
        case 1:
            backwardsFlip(0, 1, 2, 4);
            QuadSwapCorners({ 4, 0, 4, 2, 0, 3, 0, 1 });
            QuadSwapEdges({ 4, 1, 1, 3, 0, 1, 0, 2 });
            twoEdgesFlip(1, 2);
            break;
        case 2:
            twoEdgesFlip(0, 3);
            QuadSwapEdges({ 1, 0, 1, 2, 1, 3, 3, 4 });
            backwardsFlip(0, 1, 3, 4);
            QuadSwapCorners({ 0, 1, 0, 2, 2, 3, 2, 4 });
            break;
        case 3:
            QuadSwapEdges({ 3, 2, 4, 3, 0, 1, 1, 2 });
            twoEdgesFlip(1, 2);
            QuadSwapCorners({ 3, 4, 1, 3, 1, 2, 0, 1 });
            alternatingBackwardsFlip(1, 2, 3, 4);
            break;
        case 4:
            QuadSwapEdges({ 0, 1, 1, 2, 1, 3, 3, 4 });
            twoEdgesFlip(1, 2);
            QuadSwapCorners({ 0, 1, 0, 3, 0, 4, 0, 2 });
            alternatingBackwardsFlip(1, 2, 3, 4);
            break;
        case 5:
            QuadSwapEdges({ 2, 4, 2, 3, 0, 2, 0, 1 });
            twoEdgesFlip(1, 2);
            QuadSwapCorners({ 1, 3, 1, 4, 1, 2, 0, 1 });
            alternatingBackwardsFlip(1, 2, 3, 4);
            break;
        case 6:
            QuadSwapCorners({ 0, 1, 4, 0, 3, 4, 2, 3 });
            QuadSwapEdges({ 0, 1, 4, 0, 3, 4, 2, 3 });
            break;
        case 7:
            QuadSwapEdges({ 0, 3, 0, 4, 0, 2, 0, 1 });
            twoEdgesFlip(3, 4);
            QuadSwapCorners({ 0, 4, 0, 2, 0, 1, 0, 3 });
            outwardsFlip(0, 1, 3, 4);
            break;
        case 8:
            QuadSwapEdges({ 0, 1, 1, 2, 2, 4, 3, 4 });
            twoEdgesFlip(3, 4);
            QuadSwapCorners({ 0, 4, 1, 4, 1, 2, 2, 3 });
            alternatingForwardsFlip(0, 1, 3, 4);
            break;
        case 9:
            QuadSwapEdges({ 0, 1, 1, 2, 2, 4, 3, 4 });
            twoEdgesFlip(3, 4);
            QuadSwapCorners({ 0, 4, 1, 4, 1, 2, 2, 3 });
            alternatingForwardsFlip(0, 1, 3, 4);
            break;
        case 10:
            QuadSwapEdges({ 0, 1, 1, 3, 3, 4, 2, 4 });
            twoEdgesFlip(2, 4);
            QuadSwapCorners({ 0, 4, 1, 4, 1, 2, 2, 3 });
            alternatingForwardsFlip(0, 1, 3, 4);
            break;
        case 11:
            QuadSwapEdges({ 0, 3, 0, 4, 0, 2, 0, 1 });
            twoEdgesFlip(3, 4);
            QuadSwapCorners({ 0, 3, 0, 1, 0, 2, 0, 4 });
            alternatingForwardsFlip(0, 2, 3, 4);
            break;
        default:
            break;
        }
    }
    else
    {           //CLOCKWISE.
        switch (thisNum)
        {
        case 0:
            QuadSwapCorners({ 0, 1, 4, 0, 3, 4, 2, 3 });
            QuadSwapEdges({ 0, 1, 4, 0, 3, 4, 2, 3 });
            break;
        case 1:
            forwardsFlip(0, 2, 3, 4);
            QuadSwapCorners({ 0, 1, 0, 3, 4, 2, 4, 0 });
            QuadSwapEdges({ 0, 2, 0, 1, 1, 3, 4, 1 });
            twoEdgesFlip(0, 3);
            break;
        case 2:
            twoEdgesFlip(1, 2);
            QuadSwapEdges({ 3, 4, 1, 3, 1, 2, 1, 0 });
            alternatingForwardsFlip(1, 2, 3, 4);
            QuadSwapCorners({ 2, 4, 2, 3, 0, 2, 0, 1 });
            break;
        case 3:
            QuadSwapEdges({ 1, 2, 0, 1, 4, 3, 3, 2 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 0, 1, 1, 2, 1, 3, 3, 4 });
            forwardsFlip(0, 1, 3, 4);
            break;
        case 4:
            QuadSwapEdges({ 3, 4, 1, 3, 1, 2, 0, 1 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 0, 2, 0, 4, 0, 3, 0, 1 });
            forwardsFlip(0, 1, 3, 4);
            break;
        case 5:
            QuadSwapEdges({ 0, 1, 0, 2, 2, 3, 2, 4 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 0, 1, 1, 2, 1, 4, 1, 3 });
            forwardsFlip(0, 1, 3, 4);
            break;
        case 6:
            QuadSwapCorners({ 0, 1, 1, 2, 2, 3, 3, 4 });
            QuadSwapEdges({ 0, 1, 1, 2, 2, 3, 3, 4 });
            break;
        case 7: //front clockwise;
            QuadSwapEdges({ 0, 1, 0, 2, 0, 4, 0, 3 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 0, 3, 0, 1, 0, 2, 0, 4 });
            inwardsFlip(0, 1, 2, 3);
            break;
        case 8:
            QuadSwapEdges({ 3, 4, 2, 4, 1, 2, 0, 1 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 2, 3, 1, 2, 1, 4, 0, 4 });
            inwardsFlip(0, 1, 2, 4);
            break;
        case 9:
            QuadSwapEdges({ 3, 4, 2, 4, 1, 2, 0, 1 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 2, 3, 1, 2, 1, 4, 0, 4 });
            inwardsFlip(0, 1, 2, 4);
            break;
        case 10:
            QuadSwapEdges({ 2, 4, 3, 4, 1, 3, 0, 1 });
            twoEdgesFlip(0, 2);
            QuadSwapCorners({ 2, 3, 1, 2, 1, 4, 0, 4 });
            inwardsFlip(0, 1, 2, 4);
            break;
        case 11:
            QuadSwapEdges({ 0, 1, 0, 2, 0, 4, 0, 3 });
            twoEdgesFlip(0, 3);
            QuadSwapCorners({ 0, 4, 0, 2, 0, 1, 0, 3 });
            inwardsFlip(0, 1, 2, 4);
            break;
        default:
            break;
        }
    }
    return true;
}


bool Face::render()
{
    //8 is the current speed.
    if (_rotate) angle += turnDir * 8;
	if (angle >= 56 || angle <= -56)
	{
		if (_rotate) angle -= turnDir * 2;
	}
    glPushMatrix();
    glRotated(angle, axis[0], axis[1], axis[2]);

    for (int i = 0; i < 5; ++i)
    {
        corner[i]->render();
        edge[i]->render();
        glVertex3dv(_vertex[i]);
    }
    center->render();
    glLineWidth(4);
    glColor3d(0, 0, 0);
    glPopMatrix();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i)
    {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
    }
    glEnd();
    
    if (angle >= 72 || angle <= -72)
    {
        angle = 0;
        _rotate = false;
        return placeParts(turnDir);
    }
    return false;
}

void Face::rotate(int _turnDir)
{
    _rotate = true;
    turnDir = _turnDir;
}

void Face::swapCorners(int n, int k)
{
	corner[n]->swapdata(corner[k]->data);
}

void Face::swapEdges(int n, int k)
{
	edge[n]->swapdata(edge[k]->data);
}