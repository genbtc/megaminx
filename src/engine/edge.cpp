#include "edge.hpp"

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
    data.pieceNum = n;
    _defaultPieceNum = n;
}

void Edge::render() const
{
    //Edge Side One - Color Fill
    glColor3dv(data._color[0]);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 4; ++i) {
        glVertex3dv(_vertex[i]);
    }
    glEnd();
    //Edge Side Two - Color Fill
    glColor3dv(data._color[1]);
    glBegin(GL_POLYGON);
    for (int i = 2; i < 6; ++i) {
        glVertex3dv(_vertex[i]);
    }
    glEnd();
   
    if (data.hotPieceMoving)
        glColor3d(.4, 1, 0);
    else
        glColor3d(0, 0, 0);

    //Edge Side One - Border
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; ++i) {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] *1.005 , _vertex[i][2] *1.005);
    }
    glEnd();
    //(Intersection Line is at 2/3)
    //Edge Side Two - Border
    glBegin(GL_LINE_LOOP);
    for (int i = 2; i < 6; ++i) {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] *1.005 , _vertex[i][2] *1.005);
    }
    glEnd();

    // for (int i = 0; i < 6; ++i) {
    //     //Vertex Numbering Test:
    //     std::string c = std::to_string(i);
    //     utDrawText3D(_vertex[i][0], _vertex[i][1], _vertex[i][2], GLUT_BITMAP_HELVETICA_12, c.c_str());
    // }
}
