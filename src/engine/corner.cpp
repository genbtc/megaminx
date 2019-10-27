#include "corner.h"

void Corner::createAxis(int n, double* target)
{
    piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1) {
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
        CornerGrp3(target, pack);
        break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        pack.axis1 = 'x';
        pack.axis2 = 'z';
        CornerGrp4(target, pack);
        break;
    default:
        break;
    }
}

void Corner::init(int n, double* cornerVertexBase)
{
    memcpy(&_vertex, cornerVertexBase, sizeof(_vertex));
    init(n);
}

void Corner::init(int n, bool doAxes)
{
    if (doAxes)
        for (int i = 0; i < 7; ++i)
            createAxis(n, _vertex[i]);
    initColor(g_cornerPiecesColors[n], true);
    data.pieceNum = n;
}

void Corner::render()
{
    glColor3dv(data._color[0]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 4; ++i) {
        glVertex3dv(_vertex[i]);
    }
    glEnd();
    glColor3dv(data._color[1]);
    glBegin(GL_POLYGON);
    for (int i = 2; i < 6; ++i) {
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

    glLineWidth(4);
    glColor3d(0, 0, 0);

    makeGLpentagon(_vertex, 1.005, GL_LINE_LOOP);

    glBegin(GL_LINE_LOOP);
    glVertex3d(_vertex[2][0] * 1.005, _vertex[2][1] * 1.005, _vertex[2][2] * 1.005);
    glVertex3d(_vertex[5][0] * 1.005, _vertex[5][1] * 1.005, _vertex[5][2] * 1.005);
    glVertex3d(_vertex[6][0] * 1.005, _vertex[6][1] * 1.005, _vertex[6][2] * 1.005);
    glVertex3d(_vertex[1][0] * 1.005, _vertex[1][1] * 1.005, _vertex[1][2] * 1.005);
    glEnd();
    //Piece Numbering Test:
    //std::string c = std::to_string(data.pieceNum);
    //utDrawText3D(_vertex[2][0], _vertex[2][1], _vertex[2][2] + 5, c.c_str());
    //Vertex Draw Test:
    //for (int i = 0; i < 7; ++i) {
    //    std::string c = std::to_string(i);
    //    utDrawText3D(_vertex[i][0] +5, _vertex[i][1] +5, _vertex[i][2], c.c_str());
    //}
}
