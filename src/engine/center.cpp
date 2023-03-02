#include "center.h"

void Center::createAxis(int n, double* target)
{
    piecepack pack;
    switch (n + 1) {
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
        CenterCenter(target, pack);
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
        createAxis(n, _vertex[i]);
    initColor(n + 1);
}

void Center::render() const
{
    glColor3dv(data._color[0]);
    makeGLpentagon(_vertex, 1.0, GL_POLYGON);
    glLineWidth(4);
    glColor3d(0, 0, 0);
    makeGLpentagon(_vertex, 1.005, GL_LINE_LOOP);
}
