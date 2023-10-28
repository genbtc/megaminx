/** @file opengl.cpp
 *  @brief implementation of useful utilities for opengl based apps
 *  @author Bartlomiej Filipek = March 2011
 *  @author genBTC = 2017
 *  @date 2023
 */
#include <cstdio>
#include "opengl.h"

///////////////////////////////////////////////////////////////////////////////
void utSetOrthographicProjection(float scrW, float scrH)
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    // reset matrix
    glLoadIdentity();
    // set a 2D orthographic projection
    gluOrtho2D(0, scrW, 0, scrH);
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // mover the origin from the bottom left corner
    // to the upper left corner
    glTranslatef(0, -(float)scrH, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
void utResetPerspectiveProjection()
{
    // set the current matrix to GL_PROJECTION
    glMatrixMode(GL_PROJECTION);
    // restore previous settings
    glPopMatrix();
    // get back to GL_MODELVIEW matrix
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////
void utDrawText2DFont(float x, float y, void *font, const char *anglesStr)
{
    // set position to start drawing fonts 2D
    glRasterPos2f(x, y);
    // loop all the characters in the string
    for (char *c=(char*)anglesStr; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}
//choose default automatic 8x13 font
void utDrawText2D(float x, float y, const char *anglesStr)
{
    utDrawText2DFont(x, y + 13, GLUT_BITMAP_8_BY_13, anglesStr);
}
///////////////////////////////////////////////////////////////////////////////
void utDrawText3DFont(float x, float y, float z, void *font, const char *anglesStr)
{
    // set position to start drawing fonts 3D
    glRasterPos3f(x, y, z);
    // loop all the characters in the string
    for (char *c=(char*)anglesStr; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}
//choose default automatic 8x13 font
void utDrawText3D(float x, float y, float z, const char *anglesStr)
{
    utDrawText3DFont(x, y, z, GLUT_BITMAP_8_BY_13, anglesStr);
}

///////////////////////////////////////////////////////////////////////////////
void utCalculateAndPrintAngles(float x, float y, double x1, double y1)
{
    static char anglesStr[16];
    snprintf(anglesStr, 16, "X: %5.0f", x1);
    utDrawText2D(x, y, anglesStr);
    snprintf(anglesStr, 16, "Y: %5.0f", y1);
    utDrawText2D(x, y + 13, anglesStr);
}

///////////////////////////////////////////////////////////////////////////////
void utCalculateAndPrintFps(float x, float y)
{
    static char fpsStr[16];
    static unsigned int frame = 0;
    static int timeBase = 0;
    frame++;
    const int t = glutGet(GLUT_ELAPSED_TIME);
    if (t - timeBase > 1000) {
        snprintf(fpsStr, 16, "FPS: %4.2f", frame * 1000.0 / (t - timeBase));
        timeBase = t;
        frame = 0;
    }
    utDrawText2D(x, y, fpsStr);
}

///////////////////////////////////////////////////////////////////////////////
void makeGLpentagon(const double _vertex[][3], double scale, int shape)
{
    glBegin(shape);
    for (int i = 0; i < 5; ++i) {
        glVertex3d(_vertex[i][0] * scale, _vertex[i][1] * scale, _vertex[i][2] * scale);
    }
    glEnd();
}
