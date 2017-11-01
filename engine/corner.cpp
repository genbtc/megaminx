#include <GL/glut.h>
#include <math.h>
#include "corner.h"
#include "utils.h"

Corner::Corner()
{
	this->cornerInit();
}

void Corner::createAxis(int n, double* target)
{
	piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1)
    {
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
        CenterSide1(target, pack);
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
	    pack.axis1 = 'x'; pack.axis2 = 'z';
        CornerGrp3(target, pack);
        break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        CornerGrp4(target, pack);
        break;
    default:
        break;
    }
}

void Corner::init(int n, double* cornerVertexBase)
{
	_vertex[0][0] = *cornerVertexBase;
    init(n);
}
void Corner::init(int n)
{
	switch (n + 1)
	{
	case 1:
		initColor(WHITE, RED, BLUE);
		break;
	case 2:
		initColor(WHITE, GREEN, RED);
		break;
	case 3:
		initColor(WHITE, PURPLE, GREEN);
		break;
	case 4:
		initColor(WHITE, YELLOW, PURPLE);
		break;
	case 5:
		initColor(WHITE, BLUE, YELLOW);
		break;
		//
    case 6:
	    initColor(BLUE, RED, PINK);
		break;
	case 7:
		initColor(RED, GREEN, BONE);
		break;
	case 8:
		initColor(GREEN, PURPLE, LIGHT_BLUE);
		break;
	case 9:
		initColor(PURPLE, YELLOW, ORANGE);
		break;
	case 10:
		initColor(YELLOW, BLUE, LIGHT_GREEN);
		break;
		//
    case 11:
	    initColor(GRAY, BONE, LIGHT_BLUE);
		break;
	case 12:
		initColor(GRAY, LIGHT_BLUE, ORANGE);
		break;
	case 13:
		initColor(GRAY, ORANGE, LIGHT_GREEN);
		break;
	case 14:
		initColor(GRAY, LIGHT_GREEN, PINK);
		break;
	case 15:
		initColor(GRAY, PINK, BONE);
		break;
		//
    case 16:
	    initColor(LIGHT_BLUE, BONE, GREEN);
		break;
	case 17:
		initColor(BONE, PINK, RED);
		break;
	case 18:
		initColor(PINK, LIGHT_GREEN, BLUE);
		break;
	case 19:
		initColor(LIGHT_GREEN, ORANGE, YELLOW);
		break;
	case 20:
		initColor(ORANGE, LIGHT_BLUE, PURPLE);
		break;
	default:
		break;
	}
	for (int i = 0; i < 7; ++i)
	{
        createAxis(n, _vertex[i]);
	}
}

void Corner::render()
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
	glColor3dv(data._color[2]);
	glBegin(GL_POLYGON);
	glVertex3dv(_vertex[2]);
	glVertex3dv(_vertex[5]);
	glVertex3dv(_vertex[6]);
	glVertex3dv(_vertex[1]);
	glEnd();

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
	glBegin(GL_LINE_LOOP);
	glVertex3d(_vertex[2][0] * 1.005, _vertex[2][1] * 1.005, _vertex[2][2] * 1.005);
	glVertex3d(_vertex[5][0] * 1.005, _vertex[5][1] * 1.005, _vertex[5][2] * 1.005);
	glVertex3d(_vertex[6][0] * 1.005, _vertex[6][1] * 1.005, _vertex[6][2] * 1.005);
	glVertex3d(_vertex[1][0] * 1.005, _vertex[1][1] * 1.005, _vertex[1][2] * 1.005);
	glEnd();
}
