/** @file opengl.h
 *  @brief useful utilities for opengl based apps
 *  @author Bartlomiej Filipek
 *  @date March 2011
  * @author genBTC Edited 2017*
 */

#pragma once
#ifdef _WINDOWS
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// 2D and TEXT MODE
void utSetOrthographicProjection(float scrW, float scrH);
void utResetPerspectiveProjection();
void utDrawText2D(float x, float y, void *font, const char *string);
void utDrawText3D(float x, float y, float z, void *font, const char *string);

// simplier version for default font...
inline void utDrawText2D(float x, float y, const char *string)
{
    utDrawText2D(x, y, GLUT_BITMAP_8_BY_13, string);
}
// simplier version for default font...
inline void utDrawText3D(float x, float y, float z, const char *string)
{
    utDrawText3D(x, y, z, GLUT_BITMAP_8_BY_13, string);
}

void utCalculateAndPrintFps(float x, float y);
void utCalculateAndPrintAngles(float x, float y, double x1, double y1);

extern void makeGLpentagon(const double(&_vertex)[7][3], double scale, int shape);
