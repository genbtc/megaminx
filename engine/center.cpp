#include <GL/glut.h>
#include "center.h"

Center::Center()
{
	this->centerInit();
}

void Center::createAxis(int n, double* target)
{
	piecepack pack;
    switch (n + 1)
    {
	//no case1.
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
	    pack = { 'z', 'x', ((n-1) * 2 % 10) };
        CenterSide1(target, pack);
        break;
    case 7:
	    pack = { 'x', 0, 0 };
        CenterCenter(target,pack);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
	    pack = { 'y', 'x', ((n-2) * 2 % 10) };
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
    }
	initColor(n + 1);
}

void Center::render()
{
	glColor3dv(data._color[0]);
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