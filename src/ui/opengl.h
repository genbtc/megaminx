/** @file opengl.h
 *  @brief useful utilities for opengl based apps
 *  @author Bartlomiej Filipek
 *  @date March 2011
  * @author genBTC Edited 2017+2023*
 */

#pragma once
#include <GL/glut.h>
//#include <GL/gl.h>
//#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif

// optional bool:
static int textGLCenterLabels = 1;

// 2D and TEXT MODE
void utSetOrthographicProjection(float scrW, float scrH);
void utResetPerspectiveProjection();
void utDrawText2D(float x, float y, void *font, const char *string);
void utDrawText3D(float x, float y, float z, void *font, const char *string);

void utCalculateAndPrintFps(float x, float y);
void utCalculateAndPrintAngles(float x, float y, double x1, double y1);

void makeGLpentagon (const double vertex[][3], const double scale, const int shape);

#ifdef __cplusplus
}
#endif
