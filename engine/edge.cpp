#include <GL/glut.h>
#include "edge.h"

void Edge::createAxis(int n, double* target)
{
    piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1) {
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

/**
 * \brief Inits the piece with a pre-existing Vertex Array
 * \param n the number of the piece
 * \param edgeVertexBase the starting points to be memcpy'ed in
 */
void Edge::init(int n, double* edgeVertexBase)
{
    memcpy(&_vertex, edgeVertexBase, sizeof(_vertex));
    init(n);
}

void Edge::init(int n, bool doAxes)
{
    if (doAxes)
        for (int i = 0; i < 6; ++i)
            createAxis(n, _vertex[i]);
    initColor(g_edgePiecesColors[n], false);
}

void Edge::render()
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
    glLineWidth(4);
    glColor3d(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; ++i) {
        // glVertex3dv(_vertex[i]); * 1.005
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int i = 2; i < 6; ++i) {
        // glVertex3dv(_vertex[i]);
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
    }
    glEnd();
}