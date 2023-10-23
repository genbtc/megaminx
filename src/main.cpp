static const char *myglutTitle = "GenBTC's Megaminx Solver v1.4.2";
///////////////////////////////////////////////////////////////////////////////
/* MegaMinx2 - v1.3 Oct24-Dec12, 2017 - genBTC mod
             - v1.3.2 Nov 22, 2018
             - v1.3.3 Dec 19, 2018
             - v1.3.7 Mar 13, 2020
             - v1.3.8 Feb 03, 2023
             - v1.3.9 Mar 06, 2023
             - v1.4.0-b1 Mar 8, 2023 - CLI Shell (readline)
             - v1.4.1 Oct 14, 2023 (bug: ld gc'ed needed symbols)
             - v1.4.2 Oct 22, 2023 (clean code)
* Uses some code originally from:
* Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
* Modified by:
* genBTC 2017-2020+2023 / genbtc@gmx.com / @genr8_ / github.com/genbtc/
*/
// Headers
#ifdef _WINDOWS
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { main(0, 0); }
#endif
#include <cstdlib>
#include <time.h>
#include "main.h"
#include "ui/opengl.h"
#include "ui/camera.hpp"
#include "engine/megaminx.hpp"

// global vars main
int g_window;
char lastface[32] = {};
int currentFace;
bool g_help = false;
Camera g_camera;
double g_appRenderTimeTotal = 0.0;
double g_appIdleTime = 0.0;
double g_solveravg = 0.;

// Megaminx Dodecahedron. Creates object class and resets camera.
void createMegaMinx() {
    if (megaminx)
        delete megaminx;
    megaminx = new Megaminx();
    resetCameraViewport();
}

// Begin Main Program.
int main(int argc, char *argv[]) {
    srand((int)time(nullptr)); // seed rand()
    glutInit(&argc, argv);
    // set GL render modes
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);

    // create window + title
    g_window = glutCreateWindow(myglutTitle);
    // new *megaminx dodecahedron
    createMegaMinx(); //also handles /calls\ to glViewport(0,0,w,h);

    // Setup of GL Params
    glClearColor(0.22f, 0.2f, 0.2f, 1.0f);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(view_distance_view_angle, 1.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Enable each GL Params
    glEnable(GLUT_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA);
    // Stylistic thick line borders
    glLineWidth(4);
    glPointSize(4);

    // Glut Callbacks:
    glutReshapeFunc(myglutChangeWindowSize);
    glutMouseFunc(myglutMousePressed);
    glutMotionFunc(myglutMousePressedMove);

    // Right click menu:
    createMenu();
    glutMenuStatusFunc(myglutMenuVisible);
    // main-menu.cpp controls:
	glutKeyboardFunc(myglutOnKeyboard);
    glutSpecialFunc(myglutOnSpecialKeyPress);

    // Main: Display Render and Loop forever
    myglutIdle(0);  // Idle func tracks frame-rate
    glutDisplayFunc(myglutRenderScene);
    glutMainLoop(); // Run
    return 0;
}

// idle, Wait for Refresh Rate, FrameRate Cap. save CPU when idle (GLUT callback)
void myglutIdle(int) {
    glutPostRedisplay();
    glutTimerFunc(REFRESH_WAIT_TIME - 1, myglutIdle, 0);
}

// Entire OpenGL World Render Main Scene : (GLUT callback)
void myglutRenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    {
        // Spinning can be toggled w/ spacebar
        if (g_camera.isSpinning)
            g_camera.m_angleX += 0.5;
        // Rotate the Camera so cube is in View
        g_camera.RotateGLCameraView();
        // Render Megaminx.
        megaminx->render();
    }
    glPopMatrix();
    utSetOrthographicProjection(WIDTH, HEIGHT);
    { //scoped { ... for OrthographicProjection context ... }
        // Print out Green Text, FPS display + Angles + face Name
        glColor3f(0, 1, 0); // ...in green.
        utCalculateAndPrintFps(10.f, 20.f);
        utCalculateAndPrintAngles(10.f, HEIGHT - 20.f,
								  g_camera.m_angleX, g_camera.m_angleY);
        GetCurrentFace();
        utDrawText2D(10.f, HEIGHT - 40.f, lastface);

        // Print out Text (Help display)
        if (g_help)
            utPrintHelpMenu(WIDTH - 245.f, HEIGHT - 265.f);
        else {
            utDrawText2D(WIDTH - 130.f, HEIGHT - 14.f, "[H]elp");
			if (megaminx->isFullySolved() && g_help)
				utDrawText2D(WIDTH - 130.f, HEIGHT - 28.f, "SOLVED!");
			else
				utDrawText2D(WIDTH - 130.f, HEIGHT - 28.f, "[F9] = SOLVER");
		}
        // Footer
        int shadowQueueLength = megaminx->getRotateQueueNum();
        if (shadowQueueLength > 0) {
            static char rotquestr[32];
            snprintf(rotquestr, 32, "Rotate Queue: %5d", shadowQueueLength);
            utDrawText2D((WIDTH / 2) - 80, HEIGHT - 12.f, rotquestr);
        }
        if (g_solveravg > 0) {
            static char solvquestr[32];
            snprintf(solvquestr, 32, "Solver Avg: %5g", g_solveravg);
            utDrawText2D((WIDTH / 2) - 80, HEIGHT - 12.f, solvquestr);
        }
    }
    //Reset ortho scoped text and present backbuffer
    utResetPerspectiveProjection();
    glutSwapBuffers();
}

// Query Megaminx - what face we are looking at? (helper)
//TODO: NOTE: This executes a lot. TODO: How to Skip when not moving ?
void GetCurrentFace() {
    const int tempFace = getCurrentFaceFromAngles((int)g_camera.m_angleX,
                                                  (int)g_camera.m_angleY);
    if (tempFace != 0) {
        currentFace = tempFace;
		sprintf(lastface, "%s", g_colorRGBs[currentFace].name);
        // Save it into the viewmodel (sync view)
        megaminx->setCurrentFaceActive(currentFace);
    }
}

// camera spin movement - convenience toggle function (helper)
void toggleSpinning() {
    g_camera.isSpinning = !g_camera.isSpinning;
}

// main-menu.cpp - Shift key controls Directional rotation: On = CounterClockwise.
int GetDirFromSpecialKey() {
    return (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? (int)Face::CCW
/*  (CAPS LOCK cannot scan here). */                : (int)Face::CW;
}

// Controls,Camera - Double click Rotates Current Face with Shift Modifier too.
void doDoubleClickRotate(int, int) {
    megaminx->rotate(currentFace, GetDirFromSpecialKey());
}

// Camera - init/reset Camera+vars to default, set view angles, etc. (and spin)
void resetCameraViewport() {
    g_camera = Camera();
    g_camera.m_zoom = -ZDIST;
    g_camera.m_angleY = START_ANGLE;
    g_camera.m_forced_aspect_ratio = 1;
    g_camera.g_areWeDraggingPoint = false;
    myglutChangeWindowSize(WIDTH, HEIGHT); //GLUT resize window to default
}

// Controls,Camera - Route keyboard arrow keys to the camera for motion
void doCameraMotionSpecial(int key, int x, int y) {
    g_camera.PressSpecialKey(key, x, y);
}

// Controls,Camera - Handle mouse/rotation/camera movement (GLUT callback)
void myglutMousePressed(int button, int state, int x, int y) {
    g_camera.ProcessMouse(button, state, x, y);
}

// Controls, Mouse, Camera, Menu - special Right click handling for menu
int OldmenuVisibleState = 0;
int oldmenux = 0, oldmenuy = 0;
// diplay right-click menu (GLUT callback)
void myglutMenuVisible(int status, int x, int y) {
	//check for Menu first, otherwise bug from click/drag through past it
    g_camera.menuVisibleState = (status == GLUT_MENU_IN_USE);
    if (g_camera.menuVisibleState) {
        OldmenuVisibleState = 1;
        oldmenux = x;
        oldmenuy = y;
    }
} //continued (part 2):
//stops cube from rotating after right click menu visible (GLUT callback)
void myglutMousePressedMove(int x, int y) {
	//if the menu is visible, do nothing
	if (g_camera.menuVisibleState)
		return;
	//if the menu is not visible and it was never visible, we can drag.
    if (!OldmenuVisibleState)
        g_camera.ProcessMouseMotion(x, y, true);
	//if the menu is not visible but it just _WAS_, disable drag temporarly
    else {
        OldmenuVisibleState = 0;
        g_camera.ProcessMouseMotion(oldmenux, oldmenuy, false);
    }
}

// Camera, Window - Resize Window function passthrough to the camera class (GLUT callback)
void myglutChangeWindowSize(int x, int y) {
    g_camera.ChangeViewportSize(x, y);
}

//see main-menu.cpp for rest of main GUI program
