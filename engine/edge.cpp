#include <GL/glut.h>
#include <math.h>
#include "edge.h"
#include "utils.h"

Edge::Edge()
{
	for (int i = 0; i < 6; ++i)
	{
		_vertex[i][2] = -INS_SPHERE_RAD;
	}

	_vertex[0][0] = 0.99 * (INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) / 5 * 2);
	_vertex[0][1] = 0.99 * (INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10) / 5 * 2);

	_vertex[1][0] = 0.99 * (INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2);
	_vertex[1][1] = 0.99 * (INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2);

	_vertex[2][0] = 0.99 * (INS_CIRCLE_RAD * cos(3 * PI / 10) - 100 / sin(2 * PI / 5) * 2 / 5);
	_vertex[2][1] = 0.99 * (INS_CIRCLE_RAD * sin(3 * PI / 10));

	_vertex[3][0] = 0.99 * (INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) + 100 / sin(2 * PI / 5) * 2 / 5);
	_vertex[3][1] = 0.99 * (INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10));

	_vertex[4][0] = _vertex[1][0];
	_vertex[4][1] = _vertex[1][1];
    rotateVertex(_vertex[4], 'z', PI);
    rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);

    _vertex[5][0] = _vertex[0][0];
    _vertex[5][1] = _vertex[0][1];
	rotateVertex(_vertex[5], 'z', PI);
	rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
}


void Edge::createAxis(int n, double* target)
{
	piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1)
    {
    case 1:
	    initColor(1, 2);  //white-blue
	    break;
    case 2:
	    initColor(1, 3);  //white-red
	    break;
    case 3:
	    initColor(1, 4);  //white-green
	    break;
    case 4:
	    initColor(1, 5);  //white-purple
	    break;
    case 5:
	    initColor(1, 6);  //white-yellow
	    break;
	    //
	case 6:
	    initColor(2, 3);
	    break;
    case 7:
	    initColor(3, 4);
	    break;
    case 8:
	    initColor(4, 5);
	    break;
    case 9:
	    initColor(5, 6);
	    break;
    case 10:
	    initColor(6, 2);
	    break;
	    //
	case 11:
	    initColor(2, 10);
	    break;
    case 12:
	    initColor(3, 11);
	    break;
    case 13:
	    initColor(4, 12);
	    break;
    case 14:
	    initColor(5, 8);
	    break;
    case 15:
	    initColor(6, 9);
	    break;
	    //
	case 16:
	    initColor(2, 11);
	    break;
    case 17:
	    initColor(3, 12);
	    break;
    case 18:
	    initColor(4, 8);
	    break;
    case 19:
	    initColor(5, 9);
	    break;
    case 20:
	    initColor(6, 10);
	    break;
	    //
	case 21:
	    initColor(11, 12);
	    break;
    case 22:
	    initColor(12, 8);
	    break;
    case 23:
	    initColor(8, 9);
	    break;
    case 24:
	    initColor(9, 10);
	    break;
    case 25:
	    initColor(10, 11);
	    break;
	    //
	case 26:
	    initColor(7, 8);  //gray-blue
	    break;
    case 27:
	    initColor(7, 9);  //gray-orange
	    break;
    case 28:
	    initColor(7, 10);  //gray-green
	    break;
    case 29:
	    initColor(7, 11);  //gray-pink
	    break;
    case 30:
	    initColor(7, 12);  //gray-bone
	    break;
    default:
	    break;
    }
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

void Edge::init(int n)
{    
	for (int i = 0; i < 6; ++i)
	{
        createAxis(n, _vertex[i]);
	}
}

void Edge::render()
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


void Edge::flip()
{
	double buf[3];
	for (int i = 0; i < 3; ++i) buf[i] = _color[0][i];
	for (int i = 0; i < 3; ++i) _color[0][i] = _color[1][i];
	for (int i = 0; i < 3; ++i) _color[1][i] = buf[i];
}
