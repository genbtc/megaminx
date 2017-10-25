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


double view_distance_view_angle = 30;
int activeWindow = 0;

bool paused = false;

double defN = 0;
double defK = 0;

double defMX, defMY;
int pressedButton;

void display();
void reshape();
void timer(int);
void mousePressed(int button, int state, int x, int y);
void mouseMove(int x, int y);
void mousePressedMove(int x, int y);
void double_click(int x, int y);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);

using namespace std;

const long double FI = (1 + sqrt(5)) / 2;
const long double PI = acos(-1);
const long double SIDE_ANGLE = 2 * atan(FI);
const long double INS_SPHERE_RAD = 90 * sqrt(10 + 22 / sqrt(5.)) / 4 - 1;
const long double INS_CIRCLE_RAD = 70 / sqrt((5. - sqrt(5.)) / 2);
const char *title = "Megaminx v1.1 - genBTC mod";
Megaminx* megaminx;

static int window;
static int menu_id;
static int submenu_id;

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
}
void createMenu(void) {
    //Sub Menu
    submenu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Set Face Color", 2);
    glutAddMenuEntry("Rotate Corner Piece", 3);
    glutAddMenuEntry("Swap Edge Piece", 4);
    //Top Level Menu
    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Solve All/(reset)", 1);
    glutAddSubMenu("Algorithms", submenu_id);
    //glutAddSubMenu("Redraw", submenu2_id);
    glutAddMenuEntry("Quit", 0);     glutAttachMenu(GLUT_MIDDLE_BUTTON);
}


int main(int argc, char *argv[])
{
	srand(time(NULL));
    createMegaMinx();
	glutInit(&argc, argv);
	// int w = glutGet(GLUT_SCREEN_WIDTH) - 500;
	// int h = glutGet(GLUT_SCREEN_HEIGHT) - 200;
	const int w = 700;
	const int h = 700;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GL_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(w, h);    
    window = glutCreateWindow(title);

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	gluPerspective(view_distance_view_angle, w / (double)h, 1.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);
	glutMouseFunc(mousePressed);
	glutPassiveMotionFunc(mouseMove);   //not implemented
	glutMotionFunc(mousePressedMove);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	// glutFullScreen();
    // glutSetWindow(1);
    // put all the menu functions in one nice procedure
    createMenu();

	glTranslated(0, 0, -800);
	glRotated(-90, 1, 0, 0);    //puts the F1 key on the bottom.
	glLineWidth(4);

	glutMainLoop();
	return 0;
}

void reshape()
{
    const auto w = glutGet(GLUT_WINDOW_WIDTH);
    const auto h = glutGet(GLUT_WINDOW_HEIGHT);
	glutReshapeWindow(w, h);
	glLoadIdentity();
}

int depth = 0;
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA);
	glPointSize(5);
		
	glPushMatrix();
	glRotated(megaminx->n, -1, 0, 0);
	glRotated(megaminx->k, 0, 0, 1);

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
    pressedButton = button;
    if (state == GLUT_DOWN)
    {
        defMX = x;
        defMY = y;
        defN = megaminx->n;
        defK = megaminx->k;
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
        depth++;
    }
    else if (button == 5)
    {
        depth--;
    }
}

void mouseMove(int x, int y)
{
    //not implemented
}

void mousePressedMove(int x, int y)
{
	if (pressedButton == GLUT_RIGHT_BUTTON)
	{
		megaminx->n = defN + (defMY - y) / 3;
		megaminx->k = defK + (x - defMX) / 3;
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
}

void specialKeyboard(int key, int x, int y)
{
    const int modif = glutGetModifiers();
    if ((modif == GLUT_ACTIVE_CTRL) &&
        ((key == 'c') || (key == 'C'))) {
        exit(0);
    }
    const int dir = (modif == 1) ? 1 : -1;
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