/** @file opengl.h
 *  @brief useful utilities for opengl based apps
 *
 *  @author Bartlomiej Filipek
 *  @date March 2011
  * @Edited by genBTC 2017*
 */

#pragma once
#ifdef _WINDOWS
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/** logging will be by using simple "printf" function */
#define UT_LOG_WITH_PRINTF

#ifdef UT_LOG_WITH_PRINTF
#define utLOG(msg, ...) { printf("%s: ", __FUNCTION__); printf(msg, __VA_ARGS__); printf("\n"); }
#define utLOG_ERROR(msg, ...) { printf("ERR in %s, %d: ", __FUNCTION__, __LINE__); printf(msg, __VA_ARGS__); printf("\n"); }
#else
// TODO: implement
#endif

///////////////////////////////////////////////////////////////////////////////
// 2D and TEXT MODE
void utSetOrthographicProjection(float scrW, float scrH);
void utResetPerspectiveProjection();
void utDrawText2D(float x, float y, void *font, char *string);
void utDrawText3D(float x, float y, float z, void * font, char * string);

// simplier version for default font...
inline void utDrawText2D(float x, float y, char *string)
{
    utDrawText2D(x, y, GLUT_BITMAP_8_BY_13, string);
}
// simplier version for default font...
inline void utDrawText3D(float x, float y, float z, const char *string)
{
    utDrawText3D(x, y, z, GLUT_BITMAP_8_BY_13, (char *)string);
}

void utCalculateAndPrintFps(float x, float y);
void utCalculateAndPrintAngles(float x, float y, double x1, double y1);

extern void makeGLpentagon(const double(&_vertex)[7][3], double scale, int shape);
