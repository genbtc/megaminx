///////////////////////////////////////////////////////////////////////////////
// Headers
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include "engine/megaminx.h"
#include "common_physics/utils.h"
#include "common_physics/camera.h"

///////////////////////////////////////////////////////////////////////////////
const char *title = "Megaminx v1.29 - genBTC mod";
// initial window screen size
double REFRESH_RATE = 60.0;  	// monitor with 60 Hz
int WIDTH = 700;
int HEIGHT = 700;
int ZDIST = (WIDTH / HEIGHT) * 1.25 * HEIGHT;
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

// global Camera
Camera g_camera;
void ChangeSize(int x, int y)
{
    g_camera.ChangeViewportSize(x, y);
}
void resetCameraView()
{
    g_camera = Camera();
    g_camera.m_zoom = -ZDIST;
    g_camera.m_angleY = START_ANGLE;
    g_camera.m_forced_aspect_ratio = 1;
    g_areWeDraggingPoint = false;
    ChangeSize(WIDTH, HEIGHT);
}

// Glut prototype functions for callbacks (see main()
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
void HitTest();
void utPrintHelpMenu(float w, float h);
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id, submenu3_id, submenu4_id, submenu5_id;

Megaminx* megaminx = nullptr;
void createMegaMinx()
{
    resetCameraView();
    if (megaminx)
        delete megaminx;
    megaminx = new Megaminx;
}

int main(int argc, char *argv[])
{
    std::srand(time(nullptr));
    wsprintf(lastface, "%ws", L"STARTUP");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    window = glutCreateWindow(title);
    createMegaMinx();   //also includes glViewport(0,0,w,h);
    glClearColor(0.22, 0.2, 0.2, 1.0);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(view_distance_view_angle, 1.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    createMenu();  	//right click menu
    //Glut Functs:
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mousePressedMove);
    glutPassiveMotionFunc(processMousePassiveMotion);
    glutKeyboardFunc(onKeyboard);
    glutSpecialFunc(onSpecialKeyPress);
    //Display and Loop forever
    glutDisplayFunc(RenderScene);
    glutIdleFunc(Idle);
    glutMainLoop();
    return 1;
}

//TODO Figure out how to actually limit to 60 FPS 
// (This function does nothing currently besides count time).
void Idle()
{
    static double lastDeltas[3] = { 0.0, 0.0, 0.0 };
    static const double REFRESH_TIME = 1.0 / REFRESH_RATE;
    
    // in milliseconds
    const int t = glutGet(GLUT_ELAPSED_TIME);
    const double newTime = (double)t * 0.001;

    double deltaTime = newTime - g_appTime;
    if (deltaTime > REFRESH_TIME) deltaTime = REFRESH_TIME;

    // average:
    deltaTime = (deltaTime + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

    // set global:
    g_appTime = g_appTime + deltaTime;

    //ReRender Scene:
    glutPostRedisplay();

    // save delta:
    lastDeltas[0] = lastDeltas[1];
    lastDeltas[1] = lastDeltas[2];
    lastDeltas[2] = deltaTime;
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GLUT_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA);
    glLineWidth(4);
    glPointSize(5);

    glPushMatrix();
    {
        // spinning can be toggled w/ spacebar
        if(spinning)
            g_camera.m_angleX++;
        //Rotate the Cube into View
        g_camera.RotateGLCameraView();
        //Render it.
        megaminx->render();
    }
    glPopMatrix();
    glColor3f(0, 1, 0);
    //Print out Text (FPS display + Angles + face Name )
    utSetOrthographicProjection(WIDTH, HEIGHT);
    {
        utCalculateAndPrintFps(10, 20);
        utCalculateAndPrintAngles(10, HEIGHT - 20, g_camera.m_angleX, g_camera.m_angleY);
        GetCurrentFace();
        utDrawText2D(10, HEIGHT - 40, lastface);
        //Print out Text (Help display)
        if (!help)
            utPrintHelpMenu(WIDTH - 245, HEIGHT - 190);
    }
    utResetPerspectiveProjection();
    //Clean up.
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHT1);
    glDisable(GLUT_MULTISAMPLE);
    glutSwapBuffers();
}

void GetCurrentFace()
{
    const auto tempFace = megaminx->getCurrentFaceFromAngles(g_camera.m_angleX, g_camera.m_angleY);
    if (tempFace != 0) {
        currentFace = tempFace;
        wsprintf(lastface, "%ws", g_colorRGBs[currentFace].name);
        //Save it into the viewmodel (sync view)
        megaminx->setCurrentFaceActive(currentFace);
    }
}

int GetDirFromSpecialKey()
{
    const int result = (glutGetModifiers() == GLUT_ACTIVE_SHIFT) ? 
        (int)Face::CCW : (int)Face::CW;
    return result;
}

void double_click(int x, int y)
{
    const int dir = GetDirFromSpecialKey();
    megaminx->rotate(currentFace, dir);
}

#define DOUBLE_CLICK_INTERVAL	400
static int bnstate[16];
void mousePressed(int button, int state, int x, int y)
{
    g_camera.ProcessMouse(button, state, x, y);

    // can we move?
    if(g_camera.m_isLeftPressed && g_rayTest.m_hit)
    {
        g_draggedPointID = g_lastHitPointID;
        g_areWeDraggingPoint = true;
    }
    else
        g_areWeDraggingPoint = false;

    static unsigned int prev_left_click;
    static int prev_left_x, prev_left_y;

    bnstate[button] = state == GLUT_DOWN ? 1 : 0;
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            const unsigned int msec = glutGet(GLUT_ELAPSED_TIME);
            const int dx = abs(x - prev_left_x);
            const int dy = abs(y - prev_left_y);

            if (msec - prev_left_click < DOUBLE_CLICK_INTERVAL && dx < 3 && dy < 3) {
                double_click(x, y);
                prev_left_click = 0;
            }
            else {
                prev_left_click = msec;
                prev_left_x = x;
                prev_left_y = y;
            }
        }
    }
    else if (state == GLUT_UP)		return; // Disregard redundant GLUT_UP events	//
    //	Mouse Wheels are 3 and 4 on this platform. (usually 4 / 5)	//
    if(button == 3) {
        //Mouse wheel up
        g_camera.m_zoom += 2;
    }
    if (button == 4) {
        //Mouse wheel down
        g_camera.m_zoom -= 2;
    }
}

void processMousePassiveMotion(int x, int y) {
    // called when no mouse btn are pressed and mouse moves
    g_camera.ProcessPassiveMouseMotion(x, y);
}

void mousePressedMove(int x, int y)
{
    // if we are dragging any ball we cannot use mouse for scene rotation
    g_camera.ProcessMouseMotion(x, y, !g_areWeDraggingPoint);
}

void rotateDispatch(unsigned char key)
{
    const int dir = GetDirFromSpecialKey();
    switch (key)
    {
    case 'a':   //Left
    case 'A':
        megaminx->rotate(GLUT_KEY_F12, dir); break;
    case 'd':   //Right
    case 'D':
        megaminx->rotate(GLUT_KEY_F9, dir); break;
    case 'w':   //Upper(Top)
    case 'W':
        megaminx->rotate(GLUT_KEY_F7, dir); break;
    case 's':   //Front
    case 'S':
        megaminx->rotate(GLUT_KEY_F8, dir); break;
    case 'b':   //Bottom(White)
    case 'B':
        megaminx->rotate(GLUT_KEY_F1, dir); break;
    case 'c':   //Diagonal/Corner
    case 'C':   //alias because its close to the keyboard.
        megaminx->rotate(GLUT_KEY_F5, dir); break;
    case 'z':   //Back Reverze Diag
    case 'Z':
        megaminx->rotate(GLUT_KEY_F3, dir); break;
    case 'x':   //Semantically near on QWERTY
    case 'X':
        megaminx->rotate(GLUT_KEY_F4, dir); break;
    default:
        break;
    }
}

void utPrintHelpMenu(float w, float h)
{
    glColor3f(1, 1, 1);
    static char helpStr[255];
    sprintf(helpStr, "Help Menu:"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[Right Click]  Action Menu"); utDrawText2D(w, h, helpStr); h += 15;
//	sprintf(helpStr, "Front Face is Blue"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[Dbl Click]  Rotate Current >>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "  /+Shift  CounterClockwise <<"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[D/d]  Rotate Right Face <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[A/a]  Rotate Left Face <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[S/s]  Rotate Front Face <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[W/w]  Rotate Upper Face <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[Zz,Xx,Cc]  Rotate Diag <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[B/b]  Rotate Bottom Face <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[F1]-[F12]/+Shift  Face # <>"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[Space]  Toggle Auto-Spin"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[BackSpace]  Reset Camera"); utDrawText2D(w, h, helpStr); h += 15;
    sprintf(helpStr, "[Delete]  Scramble Puzzle"); utDrawText2D(w, h, helpStr); h += 15;
}

void onKeyboard(unsigned char key, int x, int y)
{
    const auto specialKey = glutGetModifiers();
    if (specialKey == GLUT_ACTIVE_CTRL) {
        switch (key) {
        case 3:	//Ctrl+C
            glutDestroyWindow(1); exit(0); break;
        case 26: //Ctrl+Z
            megaminx->undo(); break;
        default: break;
        }
    }
    switch (key)
    {
    case ' ': //spacebar 32
        spinning = !spinning; break;
    case 'h':
    case 'H':
        help = !help; break;
    case 8:   //backspace
        resetCameraView(); break;
    case 13:	//enter
        megaminx->resetFace(currentFace); break;
    case 127: //delete
        megaminx->scramble(); break;
    default: break;
    }
    //(Enter is 13, escape is 27)
    //call the megaminx specific key functions (above)
    rotateDispatch(key);
}

void onSpecialKeyPress(int key, int x, int y)
{
    //TODO Add Caps Lock to determine rotation direction also.
    const int dir = GetDirFromSpecialKey();
    switch (key)
    {
    case GLUT_KEY_PAGE_UP:
    case GLUT_KEY_PAGE_DOWN:
    case GLUT_KEY_HOME:
    case GLUT_KEY_END:
    case GLUT_KEY_INSERT:
            break;
    case GLUT_KEY_F1:
    case GLUT_KEY_F2:
    case GLUT_KEY_F3:
    case GLUT_KEY_F4:
    case GLUT_KEY_F5:
    case GLUT_KEY_F6:
    case GLUT_KEY_F7:
    case GLUT_KEY_F8:
    case GLUT_KEY_F9:
    case GLUT_KEY_F10:
    case GLUT_KEY_F11:
    case GLUT_KEY_F12:
        megaminx->rotate(key, dir);
        break;
    default:
        break;
    }
    g_camera.PressSpecialKey(key, x, y);
}

void createMenu() {
    //SubLevel 0 menu - Functions
    submenu0_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Edit... Undo", 91);
    glutAddMenuEntry("Solve All/(reset)", 92);
    glutAddMenuEntry("Reset Camera", 93);
    glutAddMenuEntry("Scramble", 100);
    glutAddMenuEntry("Quit", 102);
    
    //SubLevel 1 menu - Last Layer
    submenu1_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Grey Star", 31);
    glutAddMenuEntry("Grey Corners", 32);
    glutAddMenuEntry("Swap 1 Gray Edge", 33);
    glutAddMenuEntry("Swap 1 Gray Corner", 34);
    glutAddMenuEntry("Swap 2 Gray Corners", 35);
    
    //SubLevel2 Menu - Rotations
    submenu2_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Solve/Reset Current Face (All)", 21);
    glutAddMenuEntry("Solve Current Face's Edges", 22);
    glutAddMenuEntry("Solve Current Face's Corners", 23);
    glutAddMenuEntry("Swap Face's 1st Edge", 24);
    glutAddMenuEntry("Rotate Face's 1st Corner", 25);
     
    //SubLevel3 Menu - Steps
    submenu3_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("White Star", 41);
    glutAddMenuEntry("White Face", 42);
    //TODO Add the rest of these:
//	glutAddMenuEntry("2nd Layer Edges", 43);
//	glutAddMenuEntry("Low Y's", 44);
//	glutAddMenuEntry("4th Layer Edges", 45);
//	glutAddMenuEntry("High Y's", 46);
//	glutAddMenuEntry("6th Layer Edges", 47);

    //SubLevel4 Menu - Algos
    submenu4_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Rotate Current Face CW", 50);
    glutAddMenuEntry("r u R' U'", 51);
    glutAddMenuEntry("l u L' U'", 52);
    glutAddMenuEntry("U' L' u l u r U' R'", 53);
    glutAddMenuEntry("r u R' u r 2U' R'", 54);
    glutAddMenuEntry("u l U' R' u L' U' r", 55);
    glutAddMenuEntry("u r 2U' L' 2u R' 2U' l u", 56);
    glutAddMenuEntry("R' D' R D", 57);

    //SubLevel5 Menu - Reset Faces
    submenu5_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry(" 1 WHITE", 61);
    glutAddMenuEntry(" 2 DARK_BLUE", 62);
    glutAddMenuEntry(" 3 RED", 63);
    glutAddMenuEntry(" 4 DARK_GREEN", 64);
    glutAddMenuEntry(" 5 PURPLE", 65);
    glutAddMenuEntry(" 6 YELLOW", 66);
    glutAddMenuEntry(" 7 GRAY", 67);
    glutAddMenuEntry(" 8 LIGHT_BLUE", 68);
    glutAddMenuEntry(" 9 ORANGE", 69);
    glutAddMenuEntry("10 LIGHT_GREEN", 70);
    glutAddMenuEntry("11 PINK", 71);
    glutAddMenuEntry("12 BEIGE", 72);    
        
    //Top Level - Main Menu
    menu_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Toggle Spinning", 1);
    glutAddSubMenu("Function  -->", submenu0_id);
    glutAddSubMenu("Last Layer ->", submenu1_id);
    glutAddSubMenu("Rotations -->", submenu2_id);
    glutAddSubMenu("Steps  ----->", submenu3_id);
    glutAddSubMenu("Algos  ----->", submenu4_id);
    glutAddSubMenu("Reset Face: -->", submenu5_id);
    glutAddMenuEntry("Exit Menu...", 9999);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menuHandler(int num) {
    if (num == 1)
        spinning = !spinning;
    if (num == 21)
        megaminx->resetFace(currentFace);
    if (num == 22)  //rotate edge piece
        megaminx->resetFacesEdges(currentFace);
    if (num == 23)  //rotate corner piece
        megaminx->resetFacesCorners(currentFace);
    if (num == 24)  //rotate edge piece
        megaminx->swapOneEdge(currentFace, std::rand() % 5);
    if (num == 25)  //rotate corner piece
        megaminx->swapOneCorner(currentFace, std::rand() % 5);
    if (num == 31)	//make GRAY edges (star)
        megaminx->resetFacesEdges(GRAY);
    if (num == 32)	//make GRAY corners
        megaminx->resetFacesCorners(GRAY);
    if (num == 41)	//make WHITE edges (star)
        megaminx->resetFacesEdges(WHITE);
    if (num == 42)	//make WHITE Face
        megaminx->resetFace(WHITE);
    if (num == 50)
        megaminx->rotate(currentFace, Face::CW);
    if (num >= 51 && num <= 57)
        megaminx->rotateAlgo(currentFace, num - 50);
    if (num >= 61 && num <= 72)
        megaminx->resetFace(num - 60);
    if (num == 91)
        megaminx->undo();
    if (num == 92)
        createMegaMinx();
    if (num == 93)
        resetCameraView();
    if (num == 100)
        megaminx->scramble();
    if (num == 102)
    {
        glutDestroyWindow(1);
        exit(0);
    }
}

void MakeaVec3dOutofNormPlanes()
{ 
    auto const vertex = megaminx->g_currentFace->_vertex[0];
    std::vector<Vec3d> g_points;
    //12 faces
    for(int i = 0 ; i < 12 ; i++) {
        const Vec3d vtx3d(vertex);
        g_points.push_back(vtx3d);
    }
}

///////////////////////////////////////////////////////////////////////////////
void HitTest()
{
    // calculate ray for mouse:
    g_rayTest.CalculateRay(g_camera);

    if (g_areWeDraggingPoint == false)
    {
        auto const vertex = megaminx->g_currentFace->_vertex[0];
        Vec3d vtx3d(vertex);
        // perform hit test with all point in the scene
        // not optimal - one can use some scene tree system to optimise it... 
        g_rayTest.m_hit = megaminx->g_currentFace->RayTest(g_rayTest.m_start, g_rayTest.m_end, &vtx3d, &g_rayTest.m_lastT);
        // ??? idk
        // now in:
        // m_hit - did we hit something?
        // m_lastT - param that can be used to calculate next ball position
        // note that can point to 2D position, so we can only move a point in TWO directions on a plane
        // that is parallel to camera's near plane
    }
}
