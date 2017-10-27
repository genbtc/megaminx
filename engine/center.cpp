#include <GL/glut.h>
#include <math.h>
#include "center.h"

Center::Center()
{
    for (int i = 0; i < 5; ++i)
    {
        _vertex[i][0] = (INS_CIRCLE_RAD * cos(PI * 2 / 5 * i + 3 * PI / 10) / 5) * 2;
        _vertex[i][1] = (INS_CIRCLE_RAD * sin(PI * 2 / 5 * i + 3 * PI / 10) / 5) * 2;
        _vertex[i][2] = -INS_SPHERE_RAD;
    }
}


void Center::createAxis(int n, double* target)
{
    piecepack pack{ 0,0,0 };
    switch (n + 1)
    {
    case 2:
        pack = { 'z','x', 0 };
        break;
    case 3:
        pack = { 'z','x', 2 };
        break;
    case 4:
        pack = { 'z','x', 4 };
        break;
    case 5:
        pack = { 'z','x', 6 };
        break;
    case 6:
        pack = { 'z','x', 8 };
        break;
    case 7:
        pack = { 'x',0, 0 };
        break;
    case 8:
        pack = { 'y','x', 0 };
        break;
    case 9:
        pack = { 'y','x', 2 };
        break;
    case 10:
        pack = { 'y','x', 4 };
        break;
    case 11:
        pack = { 'y','x', 6 };
        break;
    case 12:
        pack = { 'y','x', 8 };
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
        CenterSide1(target, pack);
        break;
    case 7:
        CenterCenter(target,pack);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        CenterSide2(target, pack);
        break;
    default:
        break;
    }
}

void Center::init(int n)
{
    for (int i = 0; i < 5; ++i)
    {
        createAxis(n, _vertex[i]);
        initColor(n + 1);
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