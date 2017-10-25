#include <GL/glut.h>
#include <math.h>
#include "center.h"
#include "utils.h"

const double FI = (1 + sqrt(5)) / 2;
const double PI = acos(-1);
const double SIDE_ANGLE = 2 * atan(FI);
const double INS_SPHERE_RAD = 100 * sqrt(10 + 22 / sqrt(5)) / 4;
const double INS_CIRCLE_RAD = 100 / sqrt((5 - sqrt(5)) / 2);

Center::Center()
{
	_color[0][0] = 1;
	_color[0][1] = 1;
	_color[0][2] = 1;

    for (int i = 0; i < 5; ++i)
    {
        _vertex[i][0] = INS_CIRCLE_RAD * cos(PI * 2 / 5 * i + 3 * PI / 10) / 5 * 2;
        _vertex[i][1] = INS_CIRCLE_RAD * sin(PI * 2 / 5 * i + 3 * PI / 10) / 5 * 2;
        _vertex[i][2] = -INS_SPHERE_RAD;
    }

}

void Center::init(int n)
{
	for (int i = 0; i < 5; ++i)
	{
		switch (n + 1)
		{
		case 1:
			break;
		case 2:
			rotateVertex(_vertex[i], 'z', 2 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			break;
		case 3:
			rotateVertex(_vertex[i], 'z', 2 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			break;
		case 4:
			rotateVertex(_vertex[i], 'z', 2 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			break;
		case 5:
			rotateVertex(_vertex[i], 'z', 2 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			break;
		case 6:
			rotateVertex(_vertex[i], 'z', 2 * PI / 10);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);
			break;
		case 7:
			rotateVertex(_vertex[i], 'x', PI);
			break;
		case 8:
			rotateVertex(_vertex[i], 'y', PI);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			break;
		case 9:
			rotateVertex(_vertex[i], 'y', PI);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 2 * PI / 5);
			break;
		case 10:
			rotateVertex(_vertex[i], 'y', PI);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 4 * PI / 5);
			break;
		case 11:
			rotateVertex(_vertex[i], 'y', PI);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 6 * PI / 5);
			break;
		case 12:
			rotateVertex(_vertex[i], 'y', PI);
			rotateVertex(_vertex[i], 'x', PI - SIDE_ANGLE);
			rotateVertex(_vertex[i], 'z', 8 * PI / 5);			
			break;
        default:
            break;
		}
        initColor(n+1);
	}
}



void Center::render()
{
	glColor3dv(_color[0]);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 5; ++i)
	{
		glVertex3dv(_vertex[i]);
	}
	glEnd();
	glLineWidth(4);
	glColor3d(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 5; ++i)
	{
		// glVertex3dv(_vertex[i]);
		glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
	}
	glEnd();
}