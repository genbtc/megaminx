#pragma once
#include "engine/megaminx.h"
#include "ui/camera.h"

///////////////////////////////////////////////////////////////////////////////
//testing framerate cap:
//better for monitor with 60 Hz:
double REFRESH_RATE = 60.0;
static const double REFRESH_TIME_MS = (1000.0 / REFRESH_RATE);
static const unsigned int REFRESH_WAIT = (const unsigned int)REFRESH_TIME_MS;
// initial window screen size
float WIDTH = 700;
float HEIGHT = 700;
double ZDIST = (WIDTH / HEIGHT) * 1.25 * HEIGHT;
double START_ANGLE = 60.0f;
double view_distance_view_angle = 20;
///////////////////////////////////////////////////////////////////////////////

// global vars
double g_appRenderTimeTotal = 0.0;
double g_appIdleTime = 0.0;
bool spinning = false;
bool help = true;
int currentFace = 0;
char lastface[32];
int shadowQueueLength = 0;
double solveravg = 0.;

// global Camera
Camera g_camera;

// global main Megaminx object (pointer, managed)
Megaminx* megaminx;
Megaminx* shadowDom;

// Glut function callbacks' prototypes
void myglutIdle(int);
void myglutRenderScene();
void GetCurrentFace();
void doDoubleClickRotate(int x, int y);
void myglutMousePressed(int button, int state, int x, int y);
void myglutMousePressedMove(int x, int y);
void myglutOnKeyboard(unsigned char key, int x, int y);
void myglutOnSpecialKeyPress(int key, int x, int y);
void createMenu();
void menuHandler(int num);
void myglutMenuVisible(int status, int x, int y);
void utPrintHelpMenu(float w, float h);
static int window, submenu0_id, submenu1_id, submenu2_id, submenu3_id,
          menu_id, submenu4_id, submenu5_id, submenu6_id;
void SaveCubetoFile();
void RestoreCubeFromFile();
void MakeShadowCubeClone();
