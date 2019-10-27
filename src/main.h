#pragma once
#include "engine/megaminx.h"
#include "ui/camera.h"

///////////////////////////////////////////////////////////////////////////////
const char *title = "Megaminx v1.34 - genBTC mod";
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
// data for mouse selection
//MouseRayTestData g_rayTest;
unsigned int g_lastHitPointID;
unsigned int g_draggedPointID;
bool g_areWeDraggingPoint;
constexpr int DOUBLE_CLICK_INTERVAL = 400;
static int bnstate[16]; //mousebutton
int menuVisibleState = 0;

// global vars
double g_appRenderTimeTotal = 0.0;
double g_appIdleTime = 0.0;
bool spinning = false;
bool help = true;
int currentFace = 0;
char lastface[32];
int shadowQueueLength = 0;

// global Camera
Camera g_camera;

// global main Megaminx object (pointer, managed)
Megaminx* megaminx;
Megaminx* shadowDom;

// Glut function callbacks' prototypes
void Idle(int);
void RenderScene();

void mousePressed(int button, int state, int x, int y);
void processMousePassiveMotion(int x, int y); //currently unused
    // ^ called when no mouse btn are pressed and mouse moves
    // does nothing but record mouse position in camera class.
    //g_camera.ProcessPassiveMouseMotion(x, y);
void mousePressedMove(int x, int y);
void GetCurrentFace();
void double_click(int x, int y);
void onKeyboard(unsigned char key, int x, int y);
void onSpecialKeyPress(int key, int x, int y);
void createMenu();
void menuHandler(int num);
void menuVisible(int status, int x, int y);
void utPrintHelpMenu(float w, float h);
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id,
                            submenu3_id, submenu4_id, submenu5_id, submenu6_id;

void serializeVectorInt(std::vector<int> list1, std::string filename);
void WritePiecesFile(std::string filename, bool corner);
const std::vector<int> ReadPiecesFileVector(std::string filename); 
void FromCubeToVectorFile();
void FromVectorFileToCube();
void FromCubeToShadowCube();
