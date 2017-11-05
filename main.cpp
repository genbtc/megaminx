///////////////////////////////////////////////////////////////////////////////
// Headers
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstdlib>
#include <cmath>
#include "engine/megaminx.h"
#include "common_physics/utils.h"
#include "common_physics/camera.h"

///////////////////////////////////////////////////////////////////////////////
const char *title = "Megaminx v1.1105 - genBTC mod";
// initial window screen size
int WIDTH = 700;
int HEIGHT = 700;
int ZDIST = (WIDTH / HEIGHT) * 1.25 * HEIGHT;
double view_distance_view_angle = 20;
///////////////////////////////////////////////////////////////////////////////
// data for mouse selection
MouseRayTestData g_rayTest;
unsigned int g_lastHitPointID;
unsigned int g_draggedPointID;
bool g_areWeDraggingPoint;

// global app time in seconds
double g_appTime = 0.0;
int activeWindow = 0;

bool spinning = false;
bool help = true;

int pressedButton;
int specialKey;
int currentFace;

// globals
Camera g_camera;
void ChangeSize(int w, int h)
{
	g_camera.ChangeViewportSize(w, h);
}
void resetCameraView()
{
	g_camera = Camera();
	g_camera.m_zoom = -ZDIST;
	g_camera.m_angleY = 60.0f;
	g_camera.m_forced_aspect_ratio = 1;
	g_areWeDraggingPoint = false;
	ChangeSize(WIDTH, HEIGHT);
}

// Glut prototype functions for callbacks (see main()
void display();
void reshape(int x, int y);
void timer(int);
void mousePressed(int button, int state, int x, int y);
void processMousePassiveMotion(int x, int y);
void mousePressedMove(int x, int y);
void double_click(int x, int y);
void keyboard(unsigned char key, int x, int y);
void PressSpecialKey(int key, int x, int y);
void rotateDispatch(unsigned char key);
void createMenu();
void menu(int num);
void HitTest();
void printHelpMenu();
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id, submenu3_id, submenu4_id, submenu5_id, submenu6_id;

using namespace std;
Megaminx* megaminx;
void createMegaMinx()
{
	resetCameraView();
    megaminx = new Megaminx;
}

int main(int argc, char *argv[])
{
	srand(time(nullptr));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GL_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
    window = glutCreateWindow(title);
	createMegaMinx();
	glClearColor(0.22, 0.2, 0.2, 1.0);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(view_distance_view_angle, 1.0 , 1.0, 1000);	//<-?
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutTimerFunc(0, timer, 0);
	createMenu();  	//right click menu
	//Glut Functs:
    glutReshapeFunc(ChangeSize);
	glutMouseFunc(mousePressed);
	glutMotionFunc(mousePressedMove);
    glutPassiveMotionFunc(processMousePassiveMotion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(PressSpecialKey);
	//Display and Loop forever
    glutDisplayFunc(display);
	glutMainLoop();
	return 1;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA);
    glLineWidth(4);
	glPointSize(5);

	glPushMatrix();

    const int t = glutGet(GLUT_ELAPSED_TIME);
    const double newTime = (double)t * 0.001;
	double deltaTime = newTime - g_appTime;
	static const double REFRESH_TIME = 1.0 / 60.0; 	// monitor with 60 Hz
	static double lastDeltas[3] = { 0.0, 0.0, 0.0 };
	if (deltaTime > REFRESH_TIME) deltaTime = REFRESH_TIME;
	// average:
	deltaTime = (deltaTime + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;
	g_appTime = g_appTime + deltaTime;

	// spinning - can be toggled w/ spacebar
	if(spinning)
	    g_camera.m_angleX++;
	//Rotate the Cube into View
	g_camera.RotateGLCameraView();
	//Render it.
	megaminx->render();
	//Pop
	glPopMatrix();
	//Print out Text (FPS display)
	glColor3f(0, 1, 0);
	utSetOrthographicProjection(WIDTH, HEIGHT);
		utCalculateAndPrintFps(10, 20);
		utCalculateAndPrintAngles(10, HEIGHT - 20, g_camera.m_angleX, g_camera.m_angleY);
	utResetPerspectiveProjection();
	//Print out Text (Help display)
	if (!help)
		printHelpMenu();
	//Clean up.
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT1);
    glDisable(GL_MULTISAMPLE_ARB);
	glutSwapBuffers();
		// save time deltas:
	lastDeltas[0] = lastDeltas[1];
	lastDeltas[1] = lastDeltas[2];
	lastDeltas[2] = deltaTime;
}
void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(2, timer, 0);	
}

void double_click(int x, int y)
{
	HitTest();
    //not implemented
	//#define DOUBLE_CLICK_INTERVAL	400
}

void mousePressed(int button, int state, int x, int y)
{
	g_camera.ProcessMouse(button, state, x, y);

	// can we move any ball?
	if(g_camera.m_isLeftPressed && g_rayTest.m_hit)
	{
		g_draggedPointID = g_lastHitPointID;
		g_areWeDraggingPoint = true;
	}
	else
		g_areWeDraggingPoint = false;

	if (state == GLUT_UP)		return; // Disregard redundant GLUT_UP events	//
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
	specialKey = glutGetModifiers();
	int dir = -1;
	if (specialKey == GLUT_ACTIVE_SHIFT) 
		dir = (specialKey == 1) ? 1 : -1;
    switch (key)
    {
    case 'a':   //Left
    case 'A':
        megaminx->rotate(GLUT_KEY_F12 - 1, dir); break;
    case 'd':   //Right
    case 'D':
        megaminx->rotate(GLUT_KEY_F9 - 1, dir); break;
    case 'w':   //Upper(Top)
    case 'W':
        megaminx->rotate(GLUT_KEY_F7 - 1, dir); break;
    case 's':   //Front
    case 'S':
        megaminx->rotate(GLUT_KEY_F8 - 1, dir); break;
    case 'b':   //Bottom(White)
    case 'B':
        megaminx->rotate(GLUT_KEY_F1 - 1, dir); break;
    case 'c':   //Diagonal/Corner
    case 'C':   //alias because its close to the keyboard.
        megaminx->rotate(GLUT_KEY_F5 - 1, dir); break;
    case 'z':   //Back Reverze Diag
    case 'Z':
        megaminx->rotate(GLUT_KEY_F3 - 1, dir); break;
    case 'x':   //Semantically near on QWERTY
    case 'X':
        megaminx->rotate(GLUT_KEY_F4 - 1, dir); break;
    default:
        break;
    }
}
void printHelpMenu()
{
	glColor3f(1, 1, 1);
	utSetOrthographicProjection(WIDTH, HEIGHT);
	static char helpStr[240];
	const int startwidth = WIDTH - 240;
	const int startheight = 510;
	int w = startwidth;
	int h = startheight;
	sprintf(helpStr, "Help Menu:"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "Front Face is Blue"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[D/d] = Rotate Right Face <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[A/a] = Rotate Left Face <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[S/s] = Rotate Front Face <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[W/w] = Rotate Upper Face <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[Zz,Xx,Cc] = Rotate Diag <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[B/b] = Rotate Bottom Face <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[F1]-[F12]/+Shift = Face # <>"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[Space] = Toggle Auto-Spin"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[BackSpace] = Reset Camera"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[Delete] = Scramble Puzzle"); utDrawText2D(w, h, helpStr); h += 15;
	sprintf(helpStr, "[Right Click] = Menu"); utDrawText2D(w, h, helpStr); h += 15;
	utResetPerspectiveProjection();
}
void keyboard(unsigned char key, int x, int y)
{
	specialKey = glutGetModifiers();
	if (specialKey == GLUT_ACTIVE_CTRL) {
		switch (key) {
		case 3:	//Ctrl+C
			exit(0);		  
		case 26: //Ctrl+Z
			megaminx->undo();
		default: break;
		}
	}
	switch (key)
	{
	case ' ':
		//spacebar
		spinning = !spinning;
		break;
	case 'h':
	case 'H':
		help = !help;
		break;
	case 8:
		//backspace
		resetCameraView();
		break;
	case 127:
		//delete
		megaminx->scramble();
		break;
    default:
        break;
	}
	//call the megaminx specific key functions (above)
    rotateDispatch(key);
}

void PressSpecialKey(int key, int x, int y)
{
    specialKey = glutGetModifiers();
    const int dir = (specialKey == 1) ? 1 : -1;
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
		megaminx->rotate(key - GLUT_KEY_F1, dir);
		break;
    default:
        break;
	}
	g_camera.PressSpecialKey(key, x, y);
}

void createMenu(void) {
	//SubLevel 0 menu - last layer
	submenu0_id = glutCreateMenu(menu);
	glutAddMenuEntry("Edit... Undo", 91);
	glutAddMenuEntry("Solve All/(reset)", 92);
	glutAddMenuEntry("Scramble", 100);
	glutAddMenuEntry("Redraw", 101);
	glutAddMenuEntry("Quit", 102);
	
    
	//SubLevel 1 menu - last layer
	submenu1_id = glutCreateMenu(menu);
	glutAddMenuEntry("Make Grey Star", 31);
	glutAddMenuEntry("Make Grey Corners", 32);
	glutAddMenuEntry("One Edge Swap", 33);
	glutAddMenuEntry("One Corner Swap", 34);
	glutAddMenuEntry("Two Corner Swaps", 35);
    
	//SubLevel2 Menu - rotations
	submenu2_id = glutCreateMenu(menu);
	glutAddMenuEntry("Rotate Front Face", 3);
	glutAddMenuEntry("Solve/Reset Current Face", 21);
	glutAddMenuEntry("Rotate Corner Piece", 23);
	glutAddMenuEntry("Swap Edge Piece", 24);
     
	//SubLevel3 Menu - steps
	submenu3_id = glutCreateMenu(menu);
	glutAddMenuEntry("White Star", 41);
	glutAddMenuEntry("White Face", 42);
	glutAddMenuEntry("2nd Layer Edges", 43);
	glutAddMenuEntry("Low Y's", 44);
	glutAddMenuEntry("4th Layer Edges", 45);
	glutAddMenuEntry("High Y's", 46);
	glutAddMenuEntry("6th Layer Edges", 47);
    
	//SubLevel4 Menu - algos
	submenu4_id = glutCreateMenu(menu);
	glutAddMenuEntry("r u R' U'", 51);
	glutAddMenuEntry("l u L' U'", 52);
	glutAddMenuEntry("U' L' u l u r U' R'", 53);
	glutAddMenuEntry("r u R' u r 2U' R'", 54);
	glutAddMenuEntry("u l U' R' u L' U' r", 55);
	glutAddMenuEntry("u r 2U' L' 2u R' 2U' l u", 56);
	glutAddMenuEntry("R' D' R D", 57);

	//SubLevel6 Menu - Faces
	submenu6_id = glutCreateMenu(menu);
	glutAddMenuEntry("1  WHITE", 61);
	glutAddMenuEntry("[2] BLUE", 62);
	glutAddMenuEntry("3  RED", 63);
	glutAddMenuEntry("4  GREEN", 64);
	glutAddMenuEntry("5  PURPLE", 65);
	glutAddMenuEntry("6  YELLOW", 66);
	glutAddMenuEntry("7  GRAY", 67);
	glutAddMenuEntry("8  LIGHT_BLUE", 68);
	glutAddMenuEntry("9  ORANGE", 69);
	glutAddMenuEntry("10 LIGHT_GREEN", 70);
	glutAddMenuEntry("11 PINK", 71);
	glutAddMenuEntry("12 BONE", 72);    
        
	//Top Level Menu
	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Toggle Spinning", 1);
	glutAddSubMenu("Function  -->", submenu0_id);
	glutAddSubMenu("Last Layer ->", submenu1_id);
	glutAddSubMenu("Rotations -->", submenu2_id);
	//glutAddSubMenu("Steps------->", submenu3_id);
	//glutAddSubMenu("Algos ------>", submenu4_id);
	glutAddSubMenu("Front is: -->", submenu6_id);
	glutAddMenuEntry("Exit Menu...", 9999);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void menu(int num) {
	if (num == 92)
	{
		delete megaminx;
		createMegaMinx();
	}
	if (num == 91)
		megaminx->undo();
	if (num == 1)
		spinning = !spinning;
	if (num == 3)
		rotateDispatch('s');
	if (num == 21)
		int result = megaminx->resetFace(GLUT_KEY_F8);
	if (num == 23)  //rotate corner piece
	    megaminx->swapOneCorner(GLUT_KEY_F8, 1);
	if (num == 24)  //rotate edge piece
	    megaminx->swapOneEdge(GLUT_KEY_F8, 1);
	if (num == 31)	//make gray edges (star)
		//auto resuEdge = megaminx->findEdges(GRAY);
		auto resuEdge = megaminx->resetFacesEdges(GRAY);
	if (num == 32)	//make gray corners
		//auto resuCor = megaminx->findCorners(GRAY);
		auto resuCor = megaminx->resetFacesCorners(GRAY);
	if (num >= 61 && num <= 72)
		megaminx->setCurrentFace(num - 60);
	if (num == 100)
		megaminx->scramble();
	if (num == 102)
	{
		glutDestroyWindow(1);
		exit(0);
	}
}

///////////////////////////////////////////////////////////////////////////////
void HitTest()
{
	// calculate ray for mouse:
	g_rayTest.CalculateRay(g_camera);

	if (g_areWeDraggingPoint == false)
	{
		megaminx->setCurrentFace(7);
		auto const vertex = megaminx->g_currentFace->_vertex[0];
		Vec3d vtx3d(vertex);
		// perform hit test with all point in the scene
		// not optimal - one can use some scene tree system to optimise it... 
		g_rayTest.m_hit = megaminx->g_currentFace->RayTest(g_rayTest.m_start, g_rayTest.m_end, &vtx3d, &g_rayTest.m_lastT);

		// now in:
		// m_hit - did we hit something?
		// m_lastT - param that can be used to calculate next ball position
		// note that can point to 2D position, so we can only move a point in TWO directions on a plane
		// that is parallel to camera's near plane
	}
}
///////////////////////////////////////////////////////////////////////////////