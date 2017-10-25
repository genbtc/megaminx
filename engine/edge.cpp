#include <GL/glut.h>
#include <math.h>
#include "edge.h"
#include "utils.h"

Edge::Edge()
{
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            _color[i][j] = 0.5;
        }
    }

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

	_vertex[4][0] = 0.99 * (INS_CIRCLE_RAD * cos(3 * PI / 10) / 5 * 2);
	_vertex[4][1] = 0.99 * (INS_CIRCLE_RAD * sin(3 * PI / 10) / 5 * 2);

	_vertex[5][0] = 0.99 * (INS_CIRCLE_RAD * cos(PI * 2 / 5 + 3 * PI / 10) / 5 * 2);
	_vertex[5][1] = 0.99 * (INS_CIRCLE_RAD * sin(PI * 2 / 5 + 3 * PI / 10) / 5 * 2);

	rotateVertex(_vertex[4], 'z', PI);
	rotateVertex(_vertex[4], 'x', PI - SIDE_ANGLE);

	rotateVertex(_vertex[5], 'z', PI);
	rotateVertex(_vertex[5], 'x', PI - SIDE_ANGLE);
}

void Edge::init(int n)
{
	for (int i = 0; i < 6; ++i)
	{
		switch (n + 1)
		{
		case 1:
			initColor(1, 2);
			break;
		case 2:
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			initColor(1, 3);
			break;
		case 3:
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			initColor(1, 4);
			break;
		case 4:
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			initColor(1, 5);
			break;
		case 5:
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			initColor(1, 6);
			break;
		case 6:
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			initColor(2, 3);
			break;
		case 7:
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			initColor(3, 4);
			break;
		case 8:
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			initColor(4, 5);
			break;
		case 9:
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);  //<-------Next
			initColor(5, 6);
			break;
		case 10:
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			initColor(6, 2);
			break;
        //
		case 11:
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			initColor(2, 10);
			break;
		case 12:
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			initColor(3, 11);
			break;
		case 13:
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			initColor(4, 12);
			break;
		case 14:
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			initColor(5, 8);
			break;
		case 15:
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);  //<--Next
			initColor(6, 9);
			break;
        //
		case 16:
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			initColor(2, 11);
			break;
		case 17:
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);  //<-Next
			initColor(3, 12);
			break;
		case 18:
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			initColor(4, 8);
			break;
		case 19:
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			initColor(5, 9);
			break;
		case 20:
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			rotateVertex(_vertex[i], 'z', 6 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			initColor(6, 10);
			break;
        //
		case 21:
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			rotateVertex(_vertex[i], 'x', SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 1 * PI / 5);
			initColor(11, 12);
			break;
		case 22:
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			rotateVertex(_vertex[i], 'x', SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 3 * PI / 5);
			initColor(12, 8);
			break;
		case 23:
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			rotateVertex(_vertex[i], 'x', SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 5 * PI / 5);
			initColor(8, 9);
			break;
		case 24:
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			rotateVertex(_vertex[i], 'x', SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 7 * PI / 5);
			initColor(9, 10);
			break;
		case 25:
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			rotateVertex(_vertex[i], 'x', SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 9 * PI / 5);
			initColor(10, 11);
			break;
        //
		case 26:
			rotateVertex(_vertex[i], 'x', PI);
			initColor(7, 8);
			break;
		case 27:
			rotateVertex(_vertex[i], 'x', PI);
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);//<---Next
			initColor(7, 9);
			break;
		case 28:
			rotateVertex(_vertex[i], 'x', PI);
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			initColor(7, 10);
			break;
		case 29:
			rotateVertex(_vertex[i], 'x', PI);
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			initColor(7, 11);
			break;
		case 30:
			rotateVertex(_vertex[i], 'x', PI);
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			initColor(7, 12);
			break;
        default:
		    break;
		}
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
	glLineWidth(3);
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
