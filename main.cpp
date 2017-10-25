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

double defN = 0;
double defK = 0;

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

Megaminx* megaminx;

static int window,menu_id,submenu_id,submenu2_id;

bool doSpin = false;

void createMegaMinx()
{
    megaminx = new Megaminx;

}
void menu(int num) {
    if (num == 1)
    {
        delete megaminx;
        createMegaMinx();
    }
    if (num == 2)
    {
        rotateDispatch('f');
    }
    if (num == 3)
        doSpin = !doSpin;
}
void createMenu(void) {
    //SubLevel 2 menu
    submenu2_id = glutCreateMenu(menu);
    glutAddMenuEntry("Make Grey Star", 31);
    glutAddMenuEntry("Make Grey Corners", 32);
    glutAddMenuEntry("One Edge Swap", 31);
    glutAddMenuEntry("One Corner Swap", 32);
    glutAddMenuEntry("Scramble", 33);

    //SubLevel Menu
    submenu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Set Face Color", 22);
    glutAddMenuEntry("Rotate Corner Piece", 23);
    glutAddMenuEntry("Swap Edge Piece", 24);
    glutAddSubMenu("Last Layer -->", submenu2_id);
 
    glutAddMenuEntry("", 22);
    
    //Top Level Menu
    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Solve All/(reset)", 1);
    glutAddMenuEntry("Rotate Front Face", 2);
    glutAddMenuEntry("Mark as Front Face", 4);
    glutAddMenuEntry("Toggle Spinning", 3);
    glutAddSubMenu("Algorithms --->", submenu_id);
    //glutAddSubMenu("Redraw", submenu2_id);
    //glutAddMenuEntry("Quit", 0);     
    glutAttachMenu(GLUT_LEFT_BUTTON);
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

	glClearColor(0.2, 0.2, 0.2, 1.0);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(view_distance_view_angle, WIDTH / (double)HEIGHT, 1.0, 10000.0);
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

	glTranslated(0, 0, -800);
	glRotated(-90, 1, 0, 0);    //puts the F1 key on the bottom.
	glLineWidth(4);

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
    if (doSpin)
        angle++;

	megaminx->render();

	//glTranslated(0, 0, -100 + depth);
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
    if (button == 4)
    {
        //depth++;
        const int dira = (y - megaminx->n) < -180 ? 1 : -1;
        const int dirb = (x - megaminx->k) / 180;
        if (pressedButton == GLUT_LEFT_BUTTON)
        {
            megaminx->rotate(GLUT_KEY_F1 + dirb, dira);
        }
    }
    else if (button == 5)
    {
    }
}

void processMousePassiveMotion(int x, int y) {
    //
}


void mousePressedMove(int x, int y)
{
	if (pressedButton == GLUT_RIGHT_BUTTON)
	{
		megaminx->n = defN + (defMY - y) / 3;
		megaminx->k = defK + (x - defMX) / 3;
	}
}

void rotateDispatch(unsigned char key)
{
    switch (key)
    {
    case 'l':
        megaminx->rotate(GLUT_KEY_F12 - GLUT_KEY_F1, -1);
    case 'L':
        megaminx->rotate(GLUT_KEY_F12 - GLUT_KEY_F1, 1);
    case 'r':
        megaminx->rotate(GLUT_KEY_F9 - GLUT_KEY_F1, -1);
    case 'R':
        megaminx->rotate(GLUT_KEY_F9 - GLUT_KEY_F1, 1);
    case 'u':
        megaminx->rotate(GLUT_KEY_F7 - GLUT_KEY_F1, -1);
    case 'U':
        megaminx->rotate(GLUT_KEY_F7 - GLUT_KEY_F1, 1);
    case 'f':
        megaminx->rotate(GLUT_KEY_F8 - GLUT_KEY_F1, -1);
    case 'F':
        megaminx->rotate(GLUT_KEY_F8 - GLUT_KEY_F1, 1);
    default:
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
        //escape
	case 27:
		glutDestroyWindow(1);
		exit(0);
		break;
        //spacebar
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