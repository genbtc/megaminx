#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__
#include "engine/megaminx.h"
#include "ui/camera.h"

////////////////////////////////////////////////////////////////////////
//framerate cap for monitor with 60 Hz:
const double REFRESH_RATE = 60.0;
const double REFRESH_IN_MS = (1000.0 / REFRESH_RATE);
const unsigned int REFRESH_WAIT = (const unsigned int)REFRESH_IN_MS;
// initial window screen size
const int WIDTH = 700;
const int HEIGHT = 700;
const double ZDIST = (WIDTH / HEIGHT) * 1.25 * HEIGHT;
const double START_ANGLE = 60.0f;
const double view_distance_view_angle = 20;
////////////////////////////////////////////////////////////////////////

// global main Megaminx object (pointer, managed)
extern Megaminx* megaminx;
extern Megaminx* shadowDom;

// global vars
static double g_appRenderTimeTotal = 0.0;
static double g_appIdleTime = 0.0;
static bool spinning = false;
static bool help = false;
extern int currentFace;
extern char lastface[32];
extern wchar_t lastfacew[32];
static int shadowQueueLength = 0;
static double solveravg = 0.;

// global camera.cpp
static Camera g_camera;

// main main.cpp
int main(int argc, char *argv[]);
void createMegaMinx();

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
          menu_id, submenu4_id, submenu5_id, submenu6_id;

// load.cpp
void SaveCubetoFile();
void RestoreCubeFromFile();
void MakeShadowCubeClone();

#endif //__MAIN_H__
