#include "center.hpp"

/**
 * \brief Inits a Center piece
 * \note  (calls createAxis and initColor)
 * \param n the number of the piece
 */
void Center::init(int n)
{
    for (int i = 0; i < 5; ++i)
        createAxis(n, _vertex[i]);
    initColor(n + 1);
}

/**
 * \brief createAxis sets up the x,y,z Axes that the Center pieces ride on
 * \note (called by init on startup)
 * \param n - the number of the piece
 * \param *target - the pre-existing Vertex Array
 */
void Center::createAxis(int n, double* target)
{
    piecepack pack;
    switch (n + 1) {
    case 2 ... 6:
        pack = { 'z', 'x', ((n-1) * 2 % 10) };
        CenterSide1(target, pack);
        break;
    case 7:
        pack = { 'x', 0, 0 };
        CenterCenter(target, pack);
        break;
    case 8 ... 12:
        pack = { 'y', 'x', ((n-2) * 2 % 10) };
        CenterSide2(target, pack);
        break;
    default:
        break;
    }
}

/**
 * \brief Render Center Node
 */
void Center::render() const
{
    //Make a solid color pentagon
    glColor3dv(data._color[0]);
    makeGLpentagon(&_vertex[0], 1.0, GL_POLYGON);
    //Make a black line border around pentagon
    glLineWidth(3);     //border thickness
    glColor3d(0, 0, 0); //RGB(0,0,0) == Black
    makeGLpentagon(&_vertex[0], 1.005, GL_LINE_LOOP);   //1 + 0.005 to account for the border)
    //label the piece with a number(string), as a floating tag on piece
    if (textGLCenterLabels)
        utDrawText3D(_vertex[4][0]*1.1,_vertex[4][1]*1.1,_vertex[4][2]*1.1,GLUT_BITMAP_HELVETICA_18, data._colorName[0]);
                                                    // 1.1x spaces it out
    //TODO: Crude coords, aesthetics of text numbers is suboptimal... Option: configure Disable text shown
}
