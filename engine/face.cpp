#include <GL/glut.h>
#include <math.h>
#include "face.h"
#include "utils.h"
#include <tuple>
#include <iostream>

Face::Face()
{
    center = nullptr;
    turnDir = 0;
    thisNum = 0;
    _rotate = false;
    angle = 0;
    axis[0] = 0;
    axis[1] = 0.001;
    axis[2] = -1;
}

void Face::initEdge(Edge *a, Edge *b, Edge *c, Edge *d, Edge *e)
{
    edge[0] = a;
    edge[1] = b;
    edge[2] = c;
    edge[3] = d;
    edge[4] = e;
}

void Face::initCorner(Corner *a, Corner *b, Corner *c, Corner *d, Corner *e)
{
    corner[0] = a;
    corner[1] = b;
    corner[2] = c;
    corner[3] = d;
    corner[4] = e;
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
        _vertex[i][0] *= 1.02;
        _vertex[i][1] *= 1.02;
    }
}

void Face::createAxis(int n,double *target)
{
    char axis1 = 0, axis2 = 0;
    int multi = 0;
    switch (n + 1)
    {
    case 2:
        axis1 = 'z';  axis2 = 'x';
        break;
    case 3:
        axis1 = 'z';  axis2 = 'x';  multi = 2;
        break;
    case 4:
        axis1 = 'z';  axis2 = 'x';  multi = 4;
        break;
    case 5:
        axis1 = 'z';  axis2 = 'x';  multi = 6;
        break;
    case 6:
        axis1 = 'z';  axis2 = 'x';  multi = 8;
        break;
    case 7:
        axis1 = 'x';
        break;
    case 8:
        axis1 = 'y';  axis2 = 'x';
        break;
    case 9:
        axis1 = 'y';  axis2 = 'x';  multi = 2;
        break;
    case 10:
        axis1 = 'y';  axis2 = 'x';  multi = 4;
        break;
    case 11:
        axis1 = 'y';  axis2 = 'x';  multi = 6;
        break;
    case 12:
        axis1 = 'y';  axis2 = 'x';  multi = 8;
        break;
    default:
        break;
    }
    switch (n + 1)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        rotateVertex(target, axis1, 2 * PI / 10);
        rotateVertex(target, axis2, PI - SIDE_ANGLE);
        rotateVertex(target, 'z', multi * PI / 5);
        break;
    case 7:
        rotateVertex(target, axis1, PI);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        rotateVertex(target, axis1, PI);
        rotateVertex(target, axis2, PI - SIDE_ANGLE);
        rotateVertex(target, 'z', multi * PI / 5);
        break;
    default:
        break;
    }
}
void Face::initAxis(int n)
{
    createAxis(n,axis);
    for (int i = 0; i < 5; ++i)
    {
        createAxis(n,_vertex[i]);
    }
}

Face::~Face()
{

}

void Face::edgeflip(int x,int y)
{
    edge[x]->flip();
    edge[y]->flip();
}

void Face::cornerInsideOutFlip(int a, int b, int c, int d)
{
    corner[a]->flipBack();
    corner[b]->flip();
    corner[c]->flip();
    corner[d]->flipBack();
}
void Face::cornerBackwardsFlip(int a, int b, int c, int d)
{
    corner[a]->flipBack();
    corner[b]->flipBack();
    corner[c]->flip();
    corner[d]->flip();
}
void Face::cornerForwardsFlip(int a, int b, int c, int d)
{
    corner[a]->flip();
    corner[b]->flip();
    corner[c]->flipBack();
    corner[d]->flipBack();
}
void Face::alternatingReverseFlip(int a, int b, int c, int d)
{
    corner[a]->flipBack();
    corner[b]->flip();
    corner[c]->flipBack();
    corner[d]->flip();
}
void Face::alternatingForwardFlip(int a, int b, int c, int d)
{
    corner[a]->flip();
    corner[b]->flipBack();
    corner[c]->flip();
    corner[d]->flipBack();
}
void Face::InwardsFlip(int a, int b, int c, int d)
{
    corner[a]->flip();
    corner[b]->flipBack();
    corner[c]->flipBack();
    corner[d]->flip();
}

void Face::QuadSwapCorners(std::tuple<int, int, int, int, int, int, int, int> pack)
{
    swapCorners(std::get<0>(pack), std::get<1>(pack));
    swapCorners(std::get<2>(pack), std::get<3>(pack));
    swapCorners(std::get<4>(pack), std::get<5>(pack));
    swapCorners(std::get<6>(pack), std::get<7>(pack));
}

void Face::QuadSwapEdges(std::tuple<int, int, int, int, int, int, int, int> pack)
{
    swapEdges(std::get<0>(pack), std::get<1>(pack));
    swapEdges(std::get<2>(pack), std::get<3>(pack));
    swapEdges(std::get<4>(pack), std::get<5>(pack));
    swapEdges(std::get<6>(pack), std::get<7>(pack));
}

void Face::placeParts(bool right)
{
    if (right == true)
    {
        switch (thisNum)
        {
        case 0:
            QuadSwapCorners(std::make_tuple(0, 1, 1, 2, 2, 3, 3, 4));
            QuadSwapEdges(std::make_tuple(0, 1, 1, 2, 2, 3, 3, 4));
            break;
        case 1:
            cornerBackwardsFlip(0, 1, 2, 4);
            QuadSwapCorners(std::make_tuple(4, 0, 4, 2, 0, 3, 0, 1));
            QuadSwapEdges(std::make_tuple(4, 1, 1, 3, 0, 1, 0, 2));
            edgeflip(1, 2);
            break;
        case 2:
            edgeflip(0, 3);
            QuadSwapEdges(std::make_tuple(1, 0, 1, 2, 1, 3, 3, 4));
            cornerBackwardsFlip(0, 1, 3, 4);
            QuadSwapCorners(std::make_tuple(0, 1, 0, 2, 2, 3, 2, 4));
            break;
        case 3:
            QuadSwapEdges(std::make_tuple(3, 2, 4, 3, 0, 1, 1, 2));
            edgeflip(1, 2);
            QuadSwapCorners(std::make_tuple(3, 4, 1, 3, 1, 2, 0, 1));
            alternatingReverseFlip(1, 2, 3, 4);
            break;
        case 4:
            QuadSwapEdges(std::make_tuple(0, 1, 1, 2, 1, 3, 3, 4));
            edgeflip(1, 2);
            QuadSwapCorners(std::make_tuple(0, 1, 0, 3, 0, 4, 0, 2));
            alternatingReverseFlip(1, 2, 3, 4);
            break;
        case 5:
            QuadSwapEdges(std::make_tuple(2, 4, 2, 3, 0, 2, 0, 1));
            edgeflip(1, 2);
            QuadSwapCorners(std::make_tuple(1, 3, 1, 4, 1, 2, 0, 1));
            alternatingReverseFlip(1, 2, 3, 4);
            break;
        case 6:
            QuadSwapCorners(std::make_tuple(0, 1, 4, 0, 3, 4, 2, 3));
            QuadSwapEdges(std::make_tuple(0, 1, 4, 0, 3, 4, 2, 3));
            break;
        case 7:
            QuadSwapEdges(std::make_tuple(0, 3, 0, 4, 0, 2, 0, 1));
            edgeflip(3, 4);
            QuadSwapCorners(std::make_tuple(0, 4, 0, 2, 0, 1, 0, 3));
            InwardsFlip(0, 1, 3, 4);
            break;
        case 8:
            QuadSwapEdges(std::make_tuple(0, 1, 1, 2, 2, 4, 3, 4));
            edgeflip(3, 4);
            QuadSwapCorners(std::make_tuple(0, 4, 1, 4, 1, 2, 2, 3));
            alternatingForwardFlip(0, 1, 3, 4);
            break;
        case 9:
            QuadSwapEdges(std::make_tuple(0, 1, 1, 2, 2, 4, 3, 4));
            edgeflip(3, 4);
            QuadSwapCorners(std::make_tuple(0, 4, 1, 4, 1, 2, 2, 3));
            alternatingForwardFlip(0, 1, 3, 4);
            break;
        case 10:
            QuadSwapEdges(std::make_tuple(0, 1, 1, 3, 3, 4, 2, 4));
            edgeflip(2, 4);
            QuadSwapCorners(std::make_tuple(0, 4, 1, 4, 1, 2, 2, 3));
            alternatingForwardFlip(0, 1, 3, 4);
            break;
        case 11:
            QuadSwapEdges(std::make_tuple(0, 3, 0, 4, 0, 2, 0, 1));
            edgeflip(2, 4);
            QuadSwapCorners(std::make_tuple(0, 3, 0, 1, 0, 2, 0, 4));
            alternatingForwardFlip(0, 2, 3, 4);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (thisNum)
        {
        case 0:
            QuadSwapCorners(std::make_tuple(0, 1, 4, 0, 3, 4, 2, 3));
            QuadSwapEdges(std::make_tuple(0, 1, 4, 0, 3, 4, 2, 3));
            break;
        case 1:
            cornerForwardsFlip(0, 2, 3, 4);
            QuadSwapCorners(std::make_tuple(0, 1, 0, 3, 4, 2, 4, 0));
            QuadSwapEdges(std::make_tuple(0, 2, 0, 1, 1, 3, 4, 1));
            edgeflip(0, 3);
            break;
        case 2:
            edgeflip(1, 2);
            QuadSwapEdges(std::make_tuple(3, 4, 1, 3, 1, 2, 1, 0));
            alternatingForwardFlip(1, 2, 3, 4);
            QuadSwapCorners(std::make_tuple(2, 4, 2, 3, 0, 2, 0, 1));
            break;
        case 3:
            QuadSwapEdges(std::make_tuple(1, 2, 0, 1, 4, 3, 3, 2));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(0, 1, 1, 2, 1, 3, 3, 4));
            cornerForwardsFlip(0, 1, 3, 4);
            break;
        case 4:
            QuadSwapEdges(std::make_tuple(3, 4, 1, 3, 1, 2, 0, 1));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(0, 2, 0, 4, 0, 3, 0, 1));
            cornerForwardsFlip(0, 1, 3, 4);
            break;
        case 5:
            QuadSwapEdges(std::make_tuple(0, 1, 0, 2, 2, 3, 2, 4));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(0, 1, 1, 2, 1, 4, 1, 3));
            cornerForwardsFlip(0, 1, 3, 4);
            break;
        case 6:
            QuadSwapCorners(std::make_tuple(0, 1, 1, 2, 2, 3, 3, 4));
            QuadSwapEdges(std::make_tuple(0, 1, 1, 2, 2, 3, 3, 4));
            break;
        case 7: //front clockwise;
            QuadSwapEdges(std::make_tuple(0, 1, 0, 2, 0, 4, 0, 3));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(0, 3, 0, 1, 0, 2, 0, 4));
            cornerInsideOutFlip(0, 1, 2, 3);
            break;
        case 8:
            QuadSwapEdges(std::make_tuple(3, 4, 2, 4, 1, 2, 0, 1));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(2, 3, 1, 2, 1, 4, 0, 4));
            cornerInsideOutFlip(0, 1, 2, 4);
            break;
        case 9:
            QuadSwapEdges(std::make_tuple(3, 4, 2, 4, 1, 2, 0, 1));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(2, 3, 1, 2, 1, 4, 0, 4));
            cornerInsideOutFlip(0, 1, 2, 4);
            break;
        case 10:
            QuadSwapEdges(std::make_tuple(2, 4, 3, 4, 1, 3, 0, 1));
            edgeflip(0, 2);
            QuadSwapCorners(std::make_tuple(2, 3, 1, 2, 1, 4, 0, 4));
            cornerInsideOutFlip(0, 1, 2, 4);
            break;
        case 11:
            QuadSwapEdges(std::make_tuple(0, 1, 0, 2, 0, 4, 0, 3));
            edgeflip(0, 3);
            QuadSwapCorners(std::make_tuple(0, 4, 0, 2, 0, 1, 0, 3));
            cornerInsideOutFlip(0, 1, 2, 4);
            break;
        default:
            break;
        }
    }
}


bool Face::render()
{
    if (_rotate) angle += turnDir * 5;
    glPushMatrix();
    glRotated(angle, axis[0], axis[1], axis[2]);

    for (int i = 0; i < 5; ++i)
    {
        corner[i]->render();
        edge[i]->render();
    }
    center->render();
    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i)
    {
        glVertex3dv(_vertex[i]);
    }
    glEnd();

    glPopMatrix();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i)
    {
        glVertex3dv(_vertex[i]);
    }
    glEnd();

    if (turnDir == 1)
    {
        if (angle >= 69)
        {
            angle = 0;
            _rotate = false;
            placeParts(true);
            return true;
        }
    }
    else
    {
        if (angle <= -69)
        {
            angle = 0;
            _rotate = false;
            placeParts(false);
            return true;
        }
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
    double buf;
    for (int i = 0; i < 9; ++i)
    {
        buf = corner[n]->getcolor()[i];
        corner[n]->getcolor()[i] = corner[k]->getcolor()[i];
        corner[k]->getcolor()[i] = buf;
    }
}

void Face::swapEdges(int n, int k)
{
    double buf;
    for (int i = 0; i < 6; ++i)
    {
        buf = edge[n]->getcolor()[i];
        edge[n]->getcolor()[i] = edge[k]->getcolor()[i];
        edge[k]->getcolor()[i] = buf;
    }
}