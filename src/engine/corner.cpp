#include "corner.hpp"
#include "../ui/opengl.h"
#include <cstring>  //memcpy
#include <string>   //std::string

/**
 * \brief Inits the piece with a pre-existing Vertex Array
 * \param n the number of the Corner piece
 * \param cornerVertexBase the starting points to be memcpy'ed in
 */
void Corner::init(int n, double* cornerVertexBase)
{
    memcpy(&_vertex, cornerVertexBase, sizeof(_vertex));
    init(n);
}

/**
 * \brief Inits the piece with a pre-existing Vertex Array
 * \param n the number of the Corner piece
 * \param doAxes True by default. First Time Initialization Only
 */
void Corner::init(int n, bool doAxes)
{
    if (doAxes)
        for (int i = 0; i < 7; ++i)
            createAxis(n, _vertex[i]);
    initColor(g_cornerPiecesColors[n], true);
    data.pieceNum = n;
    _defaultPieceNum = n;
}

/**
 * \brief createAxis sets up the x,y,z Axes that the Corner pieces ride on
 * \note (called by init on startup)
 * \param n - the number of the piece
 * \param *target - the pre-existing Vertex Array
 */
void Corner::createAxis(int n, double* target)
{
    piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1) {
    case 2 ... 5 :
        axis1multi(target, pack);
        break;
    case 6 ... 10:
        CenterSide1(target, pack);
        break;
    case 11 ... 15:
        CornerGrp3(target, pack);
        break;
    case 16 ... 20:
        pack.axis1 = 'x';
        pack.axis2 = 'z';
        CornerGrp4(target, pack);
        break;
    default:
        break;
    }
}

/**
 * \brief Render Corner Node
 */
void Corner::render() const
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

    glLineWidth(3);
    (data.hotPieceMoving) ?  glColor3d(.4, 1, 0) : glColor3d(0, 0, 0);

//    makeGLpentagon(_vertex, 1.005, GL_LINE_LOOP);
    //Pentagon can be made manually in three chunks
    glBegin(GL_LINE_LOOP);
    glVertex3d(_vertex[2][0] * 1.005, _vertex[2][1] * 1.005, _vertex[2][2] * 1.005);
    glVertex3d(_vertex[1][0] * 1.005, _vertex[1][1] * 1.005, _vertex[1][2] * 1.005);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3d(_vertex[2][0] * 1.005, _vertex[2][1] * 1.005, _vertex[2][2] * 1.005);
    glVertex3d(_vertex[3][0] * 1.005, _vertex[3][1] * 1.005, _vertex[3][2] * 1.005);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3d(_vertex[2][0] * 1.005, _vertex[2][1] * 1.005, _vertex[2][2] * 1.005);
    glVertex3d(_vertex[5][0] * 1.005, _vertex[5][1] * 1.005, _vertex[5][2] * 1.005);
    glEnd();

    //Piece Numbering Test:
    if (openGLGlobalState.textGLCenterLabels) {
        std::string c = std::to_string(data.pieceNum);
        int z = (data.pieceNum < 10) ? -8 : 8;   //positional offset
        utDrawText3DFont(_vertex[2][0] + z, _vertex[2][1], _vertex[2][2] + z, GLUT_BITMAP_HELVETICA_18, c.c_str());
    }

    //Vertex Draw Test:
    //for (int i = 0; i < 7; ++i) {
    //    std::string c = std::to_string(i);
    //    utDrawText3D(_vertex[i][0] +5, _vertex[i][1] +5, _vertex[i][2], c.c_str());
    //}
}
