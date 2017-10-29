#include <GL/glut.h>
#include <math.h>
#include "corner.h"
#include "utils.h"

Corner::Corner()
{
    inited = false;
    turn = false;
    for (int i = 0; i < 7; ++i)
    {
        _vertex[i][2] = -INS_SPHERE_RAD;
    }

    _vertex[0][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) / 5 * 2;
    _vertex[0][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10) / 5 * 2;

    _vertex[1][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) + 100 / sin(2 * PI / 5) * 2 / 5;
    _vertex[1][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10);

    _vertex[2][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10);
    _vertex[2][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10);

    _vertex[3][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) - 100 / sin(2 * PI / 5) * 2 / 5;
    _vertex[3][1] = INS_CIRCLE_RAD * sin(3 * PI / 10);
    rotateVertex(_vertex[3], 'z', PI * 2 / 5);

    _vertex[4][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2;
    _vertex[4][1] = INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2;
    rotateVertex(_vertex[4], 'z', -PI * 3 / 5);
    rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);
    rotateVertex(_vertex[4], 'z', PI * 2 / 5);

    _vertex[5][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) - 100 / sin(2 * PI / 5) * 2 / 5;
    _vertex[5][1] = INS_CIRCLE_RAD * sin(3 * PI / 10);
    rotateVertex(_vertex[5], 'z', -PI * 3 / 5);
    rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
    rotateVertex(_vertex[5], 'z', PI * 2 / 5);

    _vertex[6][0] = INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2;
    _vertex[6][1] = INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2;
    rotateVertex(_vertex[6], 'z', -PI * 5 / 5);
    rotateVertex(_vertex[6], 'x', PI - SIDE_ANGLE);
}

void Corner::createAxis(int n, double* target)
{
	piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1)
    {
    case 1:
	    initColor(1, 3, 2);
	    break;
    case 2:
	    initColor(1, 4, 3);
	    break;
    case 3:
	    initColor(1, 5, 4);
	    break;
    case 4:
	    initColor(1, 6, 5);
	    break;
    case 5:
	    initColor(1, 2, 6);
	    break;
		//
    case 6:
	    initColor(2, 3, 11);
	    break;
    case 7:
	    initColor(3, 4, 12);
	    break;
    case 8:
	    initColor(4, 5, 8);
	    break;
    case 9:
	    initColor(5, 6, 9);
	    break;
    case 10:
	    initColor(6, 2, 10);
	    break;
		//
    case 11:
	    initColor(7, 12, 8);
	    break;
    case 12:
	    initColor(7, 8, 9);
	    break;
    case 13:
	    initColor(7, 9, 10);
	    break;
    case 14:
	    initColor(7, 10, 11);
	    break;
    case 15:
	    initColor(7, 11, 12);
	    break;
		//
    case 16:
	    initColor(8, 12, 4);
	    break;
    case 17:
	    initColor(12, 11, 3);
	    break;
    case 18:
	    initColor(11, 10, 2);
	    break;
    case 19:
	    initColor(10, 9, 6);
	    break;
    case 20:
	    initColor(9, 8, 5);
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

void Corner::init(int n)
{
	for (int i = 0; i < 7; ++i)
	{
        createAxis(n, _vertex[i]);
	}
	inited = true;
}



void Corner::render()
{
	glColor3dv(_color[0]);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; ++i)
	{
		glVertex3dv(_vertex[i]);
	}
	glEnd();
	glColor3dv(_color[1]);
	glBegin(GL_POLYGON);
	for (int i = 2; i < 6; ++i)
	{
		glVertex3dv(_vertex[i]);
	}
	glEnd();
	glColor3dv(_color[2]);
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


/**
 * \brief public
 */
void Corner::flip()
{
	double buf[3];
	for (int i = 0; i < 3; ++i) buf[i] = _color[0][i];
	for (int i = 0; i < 3; ++i) _color[0][i] = _color[1][i];
	for (int i = 0; i < 3; ++i) _color[1][i] = buf[i];
	for (int i = 0; i < 3; ++i) buf[i] = _color[1][i];
	for (int i = 0; i < 3; ++i) _color[1][i] = _color[2][i];
	for (int i = 0; i < 3; ++i) _color[2][i] = buf[i];
}


/**
 * \brief Does two flips
 */
void Corner::flipBack()
{
	flip();
	flip();
}