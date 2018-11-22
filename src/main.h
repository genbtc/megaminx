#pragma once
#include "engine/megaminx.h"
#include "common_physics/camera.h"

///////////////////////////////////////////////////////////////////////////////
const char *title = "Megaminx v1.30 - genBTC mod";
// initial window screen size
double REFRESH_RATE = 60.0;     // monitor with 60 Hz
float WIDTH = 700;
float HEIGHT = 700;
double ZDIST = (WIDTH / HEIGHT) * 1.25 * HEIGHT;
double START_ANGLE = 60.0f;
double view_distance_view_angle = 20;
///////////////////////////////////////////////////////////////////////////////
// data for mouse selection
MouseRayTestData g_rayTest;
unsigned int g_lastHitPointID;
unsigned int g_draggedPointID;
bool g_areWeDraggingPoint;

// global vars
double g_appTime = 0.0;
bool spinning = false;
bool help = true;
int currentFace = 0;
char lastface[32];
#define DOUBLE_CLICK_INTERVAL   400
static int bnstate[16];

// global Camera
Camera g_camera;

// global main Megaminx object (pointer, managed)
Megaminx* megaminx;

// Glut function callbacks' prototypes
void Idle();
void RenderScene();
void mousePressed(int button, int state, int x, int y);
void processMousePassiveMotion(int x, int y);
void mousePressedMove(int x, int y);
void GetCurrentFace();
void double_click(int x, int y);
void onKeyboard(unsigned char key, int x, int y);
void onSpecialKeyPress(int key, int x, int y);
void rotateDispatch(unsigned char key);
void createMenu();
void menuHandler(int num);
void menuVisible(int status, int x, int y);
void utPrintHelpMenu(float w, float h);
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id, submenu3_id, submenu4_id,
       submenu5_id;
