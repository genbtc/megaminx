#include "center.hpp"

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
    glLineWidth(3);
    glColor3d(0, 0, 0);
    makeGLpentagon(_vertex, 1.005, GL_LINE_LOOP);
    //label the piece with a string, floating as a tag
    if (textGLCenterLabels)
        utDrawText3D(_vertex[4][0]*1.1,_vertex[4][1]*1.1,_vertex[4][2]*1.1,GLUT_BITMAP_HELVETICA_18, data._colorName[0]);
    //TODO: Crude coords, aesthetics of text string suboptimal... Option: configure Disable text shown
}
