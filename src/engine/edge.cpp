#include "edge.hpp"
#include <cstring>  //memcpy
#include <string>   //std::string

/**
 * \brief Inits the piece with a pre-existing Vertex Array
 * \param n the number of the Edge piece
 * \param edgeVertexBase the starting points to be memcpy'ed in
 */
void Edge::init(int n, double* edgeVertexBase)
{
    memcpy(&_vertex, edgeVertexBase, sizeof(_vertex));
    init(n); //doAxes = true
}

/**
 * \brief Inits the piece with a pre-existing Vertex Array
 * \param n the number of the Edge piece
 * \param doAxes True by default. First Time Initialization Only
 */
void Edge::init(int n, bool doAxes)
{
    if (doAxes)
        for (int i = 0; i < 6; ++i)
            createAxis(n, _vertex[i]);
    initColor(g_edgePiecesColors[n], false);
    data.pieceNum = n;
    _defaultPieceNum = n;
}

/**
 * \brief createAxis sets up the x & z Axes that the Edge pieces ride on
 * \note (called by init on startup)
 * \param n - the number of the piece
 * \param *target - the pre-existing Vertex Array
 */
void Edge::createAxis(int n, double* target)
{
    piecepack pack = { 'z', 'x', (n * 2 % 10) };
    switch (n + 1) {
    case 1 ... 5:
        axis1multi(target, pack);
        break;
    case 6 ... 10:
        EdgeGrp2(target, pack);
        break;
    case 11 ... 15:
        EdgeGrp3(target, pack);
        break;
    case 16 ... 20:
        EdgeGrp4(target, pack);
        break;
    case 21 ... 25:
        EdgeGrp5(target, pack);
        break;
    case 26 ... 30:
        EdgeGrp6(target, pack);
        break;
    default:
        break;
    }
}

/**
 * \brief Render Edge Node
 */
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

    glColor3d(0, 0, 0); //Black

    //Edge Side One - Black Border Line 0-4
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 4; ++i) {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] *1.005 , _vertex[i][2] *1.005);
    }
    glEnd();
    //(Intersection Line is at 2/3)
    //Edge Side Two - Black Border Line 2-6
    glBegin(GL_LINE_LOOP);
    for (int i = 2; i < 6; ++i) {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] *1.005 , _vertex[i][2] *1.005);
    }
    glEnd();

    //Piece Numbering Test:
    if (openGLGlobalState.textGLCenterLabels) {
        std::string c = std::to_string(data.pieceNum);
        int x = (data.pieceNum < 15) ? -10 : 10;   //positional offset
        int z = (data.pieceNum > 9 && data.pieceNum < 20) ? -10 : 10;   //positional offset
        glColor3f(0.9, 0.2, 0.2); //red numbers
        utDrawText3DFont(_vertex[2][0], _vertex[2][1], _vertex[2][2], GLUT_BITMAP_HELVETICA_18, c.c_str());
    }
    //Vertex Numbering Test:
    if (openGLGlobalState.textGLVertexLabels) {
        for (int i = 2; i < 4; ++i) {
            std::string c = std::to_string(i);
            int z = (data.pieceNum > 9 && data.pieceNum < 20) ? -10 : 10;   //positional offset.  tiny font
            utDrawText3DFont(_vertex[i][0] + z, _vertex[i][1], _vertex[i][2] + z, GLUT_BITMAP_HELVETICA_12, c.c_str());
        }
    }
}
