#include <GL/glut.h>
#include <math.h>
#include "edge.h"
#include "utils.h"

Edge::Edge()
{
	this->edgeInit();
}

void Edge::createAxis(int n, double* target)
{
	piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        axis1multi(target, pack);
        break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        EdgeGrp2(target, pack);
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        EdgeGrp3(target, pack);
        break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        EdgeGrp4(target, pack);
        break;
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
	    pack.multi += 1;
        EdgeGrp5(target, pack);
        break;
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
        EdgeGrp6(target, pack);
        break;
    default:
        break;
    }
}

void Edge::init(int n, double* edgeVertexBase)
{
	_vertex[0][0] = *edgeVertexBase;
	init(n);
}

void Edge::init(int n)
{
	switch (n + 1)
	{
	case 1:
		initColor(WHITE, BLUE);
		break;
	case 2:
		initColor(WHITE, RED);
		break;
	case 3:
		initColor(WHITE, GREEN);
		break;
	case 4:
		initColor(WHITE, PURPLE);
		break;
	case 5:
		initColor(WHITE, YELLOW);
		break;
		//
	case 6:
	    initColor(BLUE, RED);
		break;
	case 7:
		initColor(RED, GREEN);
		break;
	case 8:
		initColor(GREEN, PURPLE);
		break;
	case 9:
		initColor(PURPLE, YELLOW);
		break;
	case 10:
		initColor(YELLOW, BLUE);
		break;
		//
	case 11:
	    initColor(BLUE, LIGHT_GREEN);
		break;
	case 12:
		initColor(RED, PINK);
		break;
	case 13:
		initColor(GREEN, BONE);
		break;
	case 14:
		initColor(PURPLE, LIGHT_BLUE);
		break;
	case 15:
		initColor(YELLOW, ORANGE);
		break;
		//
	case 16:
	    initColor(BLUE, PINK);
		break;
	case 17:
		initColor(RED, BONE);
		break;
	case 18:
		initColor(GREEN, LIGHT_BLUE);
		break;
	case 19:
		initColor(PURPLE, ORANGE);
		break;
	case 20:
		initColor(YELLOW, LIGHT_GREEN);
		break;
		//
	case 21:
	    initColor(PINK, BONE);
		break;
	case 22:
		initColor(BONE, LIGHT_BLUE);
		break;
	case 23:
		initColor(LIGHT_BLUE, ORANGE);
		break;
	case 24:
		initColor(ORANGE, LIGHT_GREEN);
		break;
	case 25:
		initColor(LIGHT_GREEN, PINK);
		break;
		//
	case 26:
	    initColor(GRAY, LIGHT_BLUE);
		break;
	case 27:
		initColor(GRAY, ORANGE);
		break;
	case 28:
		initColor(GRAY, LIGHT_GREEN);
		break;
	case 29:
		initColor(GRAY, PINK);
		break;
	case 30:
		initColor(GRAY, BONE);
		break;
	default:
		break;
	}
	for (int i = 0; i < 6; ++i)
	{
        createAxis(n, _vertex[i]);
	}
}

void Edge::render()
{
	glColor3dv(data._color[0]);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; ++i)
	{
		glVertex3dv(_vertex[i]);
	}
	glEnd();
	glColor3dv(data._color[1]);
	glBegin(GL_POLYGON);
	for (int i = 2; i < 6; ++i)
	{
		glVertex3dv(_vertex[i]);
	}
	glEnd();
	glLineWidth(4);
	glColor3d(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; ++i)
	{
		// glVertex3dv(_vertex[i]);
		glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (int i = 2; i < 6; ++i)
	{
		// glVertex3dv(_vertex[i]);
		glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
	}
	glEnd();
}