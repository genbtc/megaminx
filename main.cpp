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

double view_distance_view_angle = 20;
int activeWindow = 0;

bool paused = false;

double defK = 0;
double defN = 0;
double defMX, defMY;

int pressedButton;
int specialKey;

void display();
void reshape(int x, int y);
void timer(int);
void mousePressed(int button, int state, int x, int y);
void processMousePassiveMotion(int x, int y);
void mousePressedMove(int x, int y);
void double_click(int x, int y);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void rotateDispatch(unsigned char key);

using namespace std;

const char *title = "Megaminx v1.1 - genBTC mod";
// initial window screen size
int WIDTH = 700;
int HEIGHT = 700;
int ZDIST = -900;

Megaminx* megaminx;

static int window,menu_id, submenu0_id, submenu1_id,submenu2_id, submenu3_id, submenu4_id;

void createMegaMinx()
{
    megaminx = new Megaminx;
}

int currentFace;

void menu(int num) {
    if (num == 9)
    {
        delete megaminx;
        createMegaMinx();
    }
    if (num == 1)
        paused = !paused;
    if (num == 2)
        //TODO: Get current position from mouse 
        //or infer angle and get the current face 
        //and then set it to a global variable
        currentFace = num;
    if (num == 3)
        rotateDispatch('f');
    if (num == 23)  //rotate corner piece
        megaminx->swapOneCorner(8, 1);
    if (num == 24)  //rotate edge piece
        megaminx->swapOneEdge(8, 1);
    if (num == 100)
        megaminx->scramble();
    if (num == 102)
    {
        glutDestroyWindow(1);
        exit(0);
    }
    //if (num == 33)
    //    megaminx->swapOneCorner(6, 2);
    //if (num == 34)
    //    megaminx->swapOneEdge(7, 3);
    //if (num == 35)
    //    megaminx->swapOneEdge(8, 7);
}

void createMenu(void) {
    //SubLevel 0 menu - last layer
    submenu0_id = glutCreateMenu(menu);
    glutAddMenuEntry("Scramble", 100);
    glutAddMenuEntry("Redraw", 101);
    glutAddMenuEntry("Quit", 102);
    
    //SubLevel 1 menu - last layer
    submenu1_id = glutCreateMenu(menu);
    glutAddMenuEntry("Make Grey Star", 31);
    glutAddMenuEntry("Make Grey Corners", 32);
    //glutAddMenuEntry("One Edge Swap", 33);
    //glutAddMenuEntry("One Corner Swap", 34);
    //glutAddMenuEntry("Two Corner Swaps", 35);
    
    //SubLevel2 Menu - rotations
    submenu2_id = glutCreateMenu(menu);
    glutAddMenuEntry("Rotate Front Face", 3);
    glutAddMenuEntry("Set Face Color", 22);
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
    
        
    //Top Level Menu
    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Toggle Spinning", 1);    
    glutAddMenuEntry("Mark as Front Face", 2);
    glutAddSubMenu("Hidden/Admin.", submenu0_id);
    glutAddSubMenu("Last Layer ->", submenu1_id);
    glutAddSubMenu("Rotations -->", submenu2_id);
    glutAddSubMenu("Steps------->", submenu3_id);
    glutAddSubMenu("Algos ------>", submenu4_id);    
    glutAddMenuEntry("Solve All/(reset)", 9);
    glutAddMenuEntry("Exit Menu....", 9999);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char *argv[])
{
	srand(time(nullptr));
    createMegaMinx();
	glutInit(&argc, argv);
	// int w = glutGet(GLUT_SCREEN_WIDTH) - 500;
	// int h = glutGet(GLUT_SCREEN_HEIGHT) - 200;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GL_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
    window = glutCreateWindow(title);

	glClearColor(0.22, 0.2, 0.2, 1.0);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(view_distance_view_angle, 1.0 , 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);
	glutMouseFunc(mousePressed);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutMotionFunc(mousePressedMove);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	// glutFullScreen();
    // glutSetWindow(1);

    createMenu();	

	glTranslated(0, 0, ZDIST);
	glRotated(-90, 1, 0, 0);    //puts the F1 key on the bottom.

	glutMainLoop();
    
	return 0;
}

void reshape(int x, int y)
{
    if (x == WIDTH && y == HEIGHT)
        return;
    const auto w = glutGet(GLUT_WINDOW_WIDTH);
    const auto h = glutGet(GLUT_WINDOW_HEIGHT);
    const auto minx = min(w, h);
	glutReshapeWindow(minx, minx);
    glViewport(0, 0, minx, minx);
}

int depth = 0;
float angleX = 0.0;
float angle = 0.0;

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
    //orient the cube the way we want. N = Y axis
	glRotated(megaminx->n, -1, 0, 0);
    glRotated(-30, -1, 0, 0);
    //rotated it down by 30 degrees off the Y 
    // so we can see more faces - steeper angles
    //angle makes it spin to the right. /2 is slower
    //K = X axis
	glRotated(megaminx->k + angle/2, 0, 0, 1);
    //spinning can be disabled(toggled)
    if (paused)
        angle++;

	megaminx->render();

	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHT1);
    glDisable(GL_MULTISAMPLE_ARB);
	glutSwapBuffers();
}

void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(2, timer, 0);	
}

void double_click(int x, int y)
{
    //not implemented
}

#define DOUBLE_CLICK_INTERVAL	400
static int bnstate[16];

static int prev_x = -1, prev_y;
void mousePressed(int button, int state, int x, int y)
{
    specialKey = glutGetModifiers();
    pressedButton = button;
    if (state == GLUT_DOWN)
    {
        defMX = x;
        defMY = y;
        defN = megaminx->n;
        defK = megaminx->k;
    }
    // User must press the SHIFT key to change the
    // rotation in the X axis
    if (pressedButton == GLUT_ACTIVE_SHIFT) {
        // setting the angle to be relative to the mouse
        // position inside the window
        if (x < 0)
            angleX = 0.0;
        else if (x > WIDTH)
            angleX = 180.0;
        else
            angleX = 180.0 * x / HEIGHT;
    }
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
    if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
    //Mouse Wheels are 3 and 4 on this platform. (usually 4 / 5)
    if (button == 3) {        
        //Mouse wheel up
        glTranslated(0, 0, +5);
    }
    if (button == 4) {
        //Mouse wheel down
        glTranslated(0, 0, -5);
    }

}

void processMousePassiveMotion(int x, int y) {
    //
}


void mousePressedMove(int x, int y)
{
	if (pressedButton == GLUT_LEFT_BUTTON)
	{
        megaminx->k = defK + (x - defMX) / 3;
	    megaminx->n = defN + (defMY - y) / 3;		
	}
}

void rotateDispatch(unsigned char key)
{
    switch (key)
    {
    case 'l':   //Left
        megaminx->rotate(GLUT_KEY_F12 - 1, -1); break;
    case 'L':
        megaminx->rotate(GLUT_KEY_F12 - 1, 1); break;
    case 'r':   //Right
        megaminx->rotate(GLUT_KEY_F9 - 1, -1); break;
    case 'R':
        megaminx->rotate(GLUT_KEY_F9 - 1, 1); break;
    case 'u':   //Upper(Top)
        megaminx->rotate(GLUT_KEY_F7 - 1, -1); break;
    case 'U':
        megaminx->rotate(GLUT_KEY_F7 - 1, 1); break;
    case 'f':   //Front
        megaminx->rotate(GLUT_KEY_F8 - 1, -1); break;
    case 'F':
        megaminx->rotate(GLUT_KEY_F8 - 1, 1); break;
    case 'b':   //Bottom(White)
        megaminx->rotate(GLUT_KEY_F1 - 1, -1); break;
    case 'B':
        megaminx->rotate(GLUT_KEY_F1 - 1, 1); break;
    case 'd':
    case 'c':   //Diagonal/Corner
        megaminx->rotate(GLUT_KEY_F5 - 1, -1); break;
    case 'D':
    case 'C':   //alias because its close to the keyboard.
        megaminx->rotate(GLUT_KEY_F5 - 1, 1); break;
    case 'z':   //Back Reverze Diag
        megaminx->rotate(GLUT_KEY_F3 - 1, -1); break;
    case 'Z':
        megaminx->rotate(GLUT_KEY_F3 - 1, 1); break;
    case 'x':   //Semantically near on QWERTY
        megaminx->rotate(GLUT_KEY_F4 - 1, -1); break;
    case 'X':
        megaminx->rotate(GLUT_KEY_F4 - 1, 1); break;
    case 'q':
        megaminx->swapOneEdge(8, 1); break;   //left by 1
    case 'w':
        megaminx->swapOneEdge(8, 2); break;    //right by 1
    case 'e':
        megaminx->swapOneEdge(8, 3); break;   //right by 1
    case 't':
        megaminx->swapOneEdge(8, 4); break;    // "
    case 'y': 
        megaminx->swapOneEdge(8, 0); break;   // "
    default:
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		paused = !paused;
		break;
        //backspace
	case 8:
		megaminx->scramble();
		break;
    default:
        break;
	}
    rotateDispatch(key);
}

void specialKeyboard(int key, int x, int y)
{
    specialKey = glutGetModifiers();
    if ((specialKey == GLUT_ACTIVE_CTRL) &&
        ((key == 'c') || (key == 'C'))) {
        exit(0);
    }
    const int dir = (specialKey == 1) ? 1 : -1;
	switch (key)
	{
    case GLUT_KEY_LEFT:
        megaminx->k -= 2;
        break;
	case GLUT_KEY_UP:
		megaminx->n += 2;
		break;
    case GLUT_KEY_RIGHT:
        megaminx->k += 2;
        break;
	case GLUT_KEY_DOWN:
		megaminx->n -= 2;
		break;
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

}