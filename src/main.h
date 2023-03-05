#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__
#include "engine/megaminx.hpp"
#include "ui/camera.hpp"
#include "engine/load.hpp"

////////////////////////////////////////////////////////////////////////
//framerate cap for monitor with 60 Hz:
const double REFRESH_RATE = 60.0;
const double REFRESH_IN_MS = (1000.0 / REFRESH_RATE);
const unsigned int REFRESH_WAIT = (const unsigned int)REFRESH_IN_MS;
// initial window screen size
const int WIDTH = 640;
const int HEIGHT = 640;
const double ZDIST = (WIDTH / HEIGHT) * 1.25 * HEIGHT;
const double START_ANGLE = 60.0f;
const double view_distance_view_angle = 20;
////////////////////////////////////////////////////////////////////////

// global camera.cpp
static Camera g_camera;

// global main.cpp
char lastface[32];
int currentFace;
bool spinning = false;
bool help = false;
double g_appRenderTimeTotal = 0.0;
double g_appIdleTime = 0.0;
int shadowQueueLength = 0;
double solveravg = 0.;

// main main.cpp
int main(int argc, char *argv[]);
void createMegaMinx();

//global megaminx.cpp
extern Megaminx* megaminx;
extern Megaminx* shadowDom;

// GLUT main.cpp - Glut function callbacks prototypes
void resetCameraViewport();
void myglutIdle(int);
void myglutRenderScene();
void GetCurrentFace();
int GetDirFromSpecialKey();
void doDoubleClickRotate(int x, int y);
void myglutMousePressed(int button, int state, int x, int y);
void myglutMousePressedMove(int x, int y);

// menu.cpp
void myglutOnKeyboard(unsigned char key, int x, int y);
void myglutOnSpecialKeyPress(int key, int x, int y);
void createMenu();
void menuHandler(int num);
void myglutMenuVisible(int status, int x, int y);
void utPrintHelpMenu(float w, float h);
static int window;
static int submenu0_id, submenu1_id, submenu2_id, submenu3_id,
           submenu4_id, submenu5_id, submenu6_id, menu_id;

/** logging will be by using simple "printf" function */
#define UT_LOG_WITH_PRINTF
#ifdef UT_LOG_WITH_PRINTF
#define utLOG(msg, ...) { printf("%s: ", __FUNCTION__); printf(msg, __VA_ARGS__); printf("\n"); }
#define utLOG_ERROR(msg, ...) { printf("ERR in %s, %d: ", __FUNCTION__, __LINE__); printf(msg, __VA_ARGS__); printf("\n"); }
#endif

#endif //__MAIN_H__
