/** @file opengl.h
 *  @brief useful utilities for opengl based apps
 *  @author Bartlomiej Filipek
 *  @date March 2011
  * @author genBTC Edited 2017+2023*
 */

#pragma once
#include <GL/glut.h>

#ifdef __cplusplus
extern "C" {
#endif

// 3D to 2D Orthographic Projection Matrix for Perspective & Zoom
void utSetOrthographicProjection(float screenWidth, float screenHeight);
void utResetPerspectiveProjection();

// Text Fonts
void utDrawText2DFont(float x, float y, void *font, const char *theString);
void utDrawText3DFont(float x, float y, float z, void *font, const char *theString);
void utDrawText2D(float x, float y, const char *theString);
void utDrawText3D(float x, float y, float z, const char *theString);

// Display Stats
void utCalculateAndPrintFps(float x, float y);
void utCalculateAndPrintAngles(float x, float y, double x1, double y1);

// GL_POLYGON Draw Primitive (fill)
void makeGLpolygon (const double vertex[][3], double scaleFactor, int nsided);

// Numeric piece labels (optional)
static struct openGLstate {
  int textGLCenterLabels;
  int textGLVertexLabels;
} openGLGlobalState = { 1, 0 };   //TODO: Add better option.

//Global state: TODO put these inside something more
static char g_angleXstring[16];
static char g_angleYstring[16];
static char g_FPS[16];
static unsigned int g_framecounter = 0;
static int  g_elapsed = 0;
static int  g_timeBase = 0;

#ifdef __cplusplus
}
#endif
