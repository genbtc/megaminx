/** @file opengl.cpp
 *  @brief implementation of useful utilities for opengl based apps
 *  @author originally Bartlomiej Filipek = March 2011
 *  @author edited by genBTC = 2017-2023,2024
 */
#include <cstdio>
#include "opengl.h"

void utSetOrthographicProjection(float screenWidth, float screenHeight)
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    // reset matrix
    glLoadIdentity();
    // set a 2D orthographic projection
    gluOrtho2D(0, screenWidth, 0, screenHeight);
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // also need to move the origin
    // from the bottom left corner
    // to the upper left corner
    glTranslatef(0, -screenHeight, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

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

void utDrawText2DFont(float x, float y, void *font, const char *theString)
{
    //Set 3D Depth Position = Clamp Z value for 2D draw
    utDrawText3DFont(x, y, 1, font, theString);
}
//choose default automatic 8x13 font
void utDrawText2D(float x, float y, const char *theString)
{
    utDrawText3DFont(x, y + 13, 1, GLUT_BITMAP_8_BY_13, theString);
}
void utDrawText3DFont(float x, float y, float z, void *font, const char *theString)
{
    // set position to start drawing fonts 3D
    glRasterPos3f(x, y, z);
    // loop all the characters in the string
    for (char *c=(char*)theString; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}
//choose default automatic 8x13 font
void utDrawText3D(float x, float y, float z, const char *theString)
{
    utDrawText3DFont(x, y, z, GLUT_BITMAP_8_BY_13, theString);
}

void utCalculateAndPrintAngles(float x, float y, double x1, double y1)
{
    snprintf(g_angleXstring, 16, "X,%5.0f,", x1);
    utDrawText2D(x, y, g_angleXstring);
    snprintf(g_angleYstring, 16, "Y:  %5.0f", y1);
    utDrawText2D(x + 16, y, g_angleYstring);
}

void utCalculateAndPrintFps(float x, float y)
{
    g_elapsed = glutGet(GLUT_ELAPSED_TIME);
    g_framecounter++;
    if (g_elapsed - g_timeBase > 1000) {
        snprintf(g_FPS, 16, "FPS: %4.2f", g_framecounter * 1000.0 / (g_elapsed - g_timeBase));
        g_timeBase = g_elapsed;
        g_framecounter = 0;
    }
    utDrawText2D(x, y, g_FPS);
}

void makeGLpolygon(const double _vertex[][3], double scaleFactor, int nsided)
{
    glBegin(GL_POLYGON);
    {
        for (int i = 0; i < nsided; ++i)
            glVertex3d(_vertex[i][0] * scaleFactor, _vertex[i][1] * scaleFactor, _vertex[i][2] * scaleFactor);
    }
    glEnd();
}
