static const char *myglutTitle = "GenBTC's Megaminx Solver v1.4.4";
///////////////////////////////////////////////////////////////////////////////
/* Uses some code originally from:
 * Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
 * Now Modified by Gen
 * genBTC 2017-2020+2023 / genbtc@gmx.com / @genr8_ / github.com/genbtc/
 * MegaMinx2 - v1.3 Oct24-Dec12, 2017 - genBTC mod. ChangeLog
             - v1.3.2 Nov 22, 2018
             - v1.3.3 Dec 19, 2018
             - v1.3.7 Mar 13, 2020
             - v1.3.8 Feb 03, 2023
             - v1.3.9 Mar 06, 2023
             - v1.4.0-b1 Mar 8, 2023 - CLI Shell (readline)
             - v1.4.1 Oct 14, 2023 (bug: ld gc'ed needed symbols)
             - v1.4.2 Oct 22, 2023 (still bugged.)
             - v1.4.3 Oct 25, 2023 (symbol errors fixed, piece number tooltips)
             - v1.4.4 Oct 26, 2023 Create algorithms.hpp and refactor algostring menus
*/
// Headers
#ifdef _WINDOWS
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { main(0, 0); }
#endif
#include <cstdlib> //srand
#include <time.h>  //time
// Includes
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
double g_solveravg = 0.0;

/**
 * \brief Megaminx Dodecahedron. Creates object class and resets camera.
 */
void createMegaMinx() {
    if (megaminx)
        delete megaminx;
    megaminx = new Megaminx();
    resetCameraViewport();
}

/**
 * \brief Begin Main Program.
 */
int main(int argc, char *argv[]) {
    srand((int)time(nullptr)); // seed rand()
    glutInit(&argc, argv);
    // set OpenGL render modes
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);

    // create window + title
    g_window = glutCreateWindow(myglutTitle);
    // new *megaminx dodecahedron
    createMegaMinx(); //also handles /calls\ to glViewport(0,0,w,h);

    // Setup of OpenGL Params
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

    // Glut Window Resize Callback
    glutReshapeFunc(myglutChangeWindowSize);

    // Glut Mouse Handler Callbacks
    glutMouseFunc(myglutMousePressed);
    glutMotionFunc(myglutMousePressedMove);

    // Glut Keyboard Handler Callbacks (main-menu.cpp controls)
    glutKeyboardFunc(myglutOnKeyboard);
    glutSpecialFunc(myglutOnSpecialKeyPress);

    // Glut Menu Handler Callback (main-menu.cpp create Right click GUI)
    createMenu();
    glutMenuStatusFunc(myglutMenuVisible);

    // Run Main: Display Render, and Loop forever
    myglutIdle(0);  // Idle func tracks frame-rate
    // Glut Render Function Callback
    glutDisplayFunc(myglutRenderScene);
    glutMainLoop(); // Run
    return 0;
}

/**
 * \brief  idle, Wait for Refresh Rate, FrameRate Cap. save CPU when idle (GLUT callback)
 */
void myglutIdle(int) {
    glutPostRedisplay();
    glutTimerFunc(REFRESH_WAIT_TIME - 1, myglutIdle, 0);
}

/**
 * \brief  Entire OpenGL World Render Main Scene : (GLUT callback)
 */
void myglutRenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    { //scoped { ... for GL in the matrix context ... }
        // Spinning can be toggled w/ spacebar
        if (g_camera.isSpinning)
            g_camera.m_angleX += 0.5;   //TODO: spinconfig
        // Rotate the Camera so cube is in View
        g_camera.RotateGLCameraView();
        // Render Megaminx.
        megaminx->render();
    }
    glPopMatrix();
    utSetOrthographicProjection(WIDTH, HEIGHT);
    { //scoped { ... for OrthographicProjection context ... }
        // Print out Green Text, FPS display + Angles + face Name
        glColor3f(0, 1, 0); // ...in green. TODO: colorconfig
        utCalculateAndPrintFps(10.f, 20.f);
        utCalculateAndPrintAngles(10.f, HEIGHT - 20.f,
								  g_camera.m_angleX, g_camera.m_angleY);
        GetCurrentFace();
        utDrawText2D(10.f, HEIGHT - 40.f, lastface);

        // Print out Text (Help display)
        if (g_help)
            utPrintHelpMenu(WIDTH - 245.f, HEIGHT - 265.f);
        else {
            utDrawText2D(WIDTH - 130.f, HEIGHT - 14.f,      "[H]elp");
			if (megaminx->isFullySolved() && g_help)
                utDrawText2D(WIDTH - 130.f, HEIGHT - 28.f,  "SOLVED!");
			else
                utDrawText2D(WIDTH - 130.f, HEIGHT - 28.f,  "[F9] = SOLVER");
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
            //TODO: fix this from lingering.
        }
    } //end ortho scope
    //Reset text overlay projections
    utResetPerspectiveProjection();
    //Present GL back buffer to front (swap)
    glutSwapBuffers();
}

/**
 * \brief Query Megaminx - what face we are looking at? (helper)
*/
//TODO: NOTE: This executes a lot. TODO: How to Skip when not moving ?
void GetCurrentFace() {
    const int tempFace = getCurrentFaceFromAngles((int)g_camera.m_angleX,
                                                  (int)g_camera.m_angleY);
    if (currentFace != tempFace) {
        currentFace = tempFace;
		sprintf(lastface, "%s", g_colorRGBs[currentFace].name);
        // Save it into the viewmodel (sync view)
        megaminx->setCurrentFaceActive(currentFace);
    }
}

/**
 * \brief  camera spin movement - convenience toggle function (helper)
 */
void toggleSpinning() {
    g_camera.isSpinning = !g_camera.isSpinning;
}

/**
 * \brief  Shift key controls Directional rotation: On = CounterClockwise. (main-menu.cpp helper)
 */
int GetDirFromSpecialKey() {
    return (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? (int)Face::CCW
/*  (CAPS LOCK cannot scan here). */                : (int)Face::CW;
}

/**
 * \brief Controls,Camera - Double click Rotates Current Face with Shift Modifier too.
 */
void doDoubleClickRotate(int, int) {
    megaminx->rotate(currentFace, GetDirFromSpecialKey());
}

/**
 * \brief Camera - init/reset Camera+vars to default, set view angles, etc. (and spin)
 */
void resetCameraViewport() {
    g_camera = Camera();
    g_camera.m_zoom = -ZDIST;
    g_camera.m_angleY = START_ANGLE;
    g_camera.m_forced_aspect_ratio = 1;
    g_camera.g_areWeDraggingPoint = false;
    myglutChangeWindowSize(WIDTH, HEIGHT); //GLUT resize window to default
}

/**
 * \brief Controls,Camera - Route keyboard arrow keys to the camera for motion
 */
void doCameraMotionSpecial(int key, int x, int y) {
    g_camera.PressSpecialKey(key, x, y);
}

/**
 * \brief Controls,Camera - Handle mouse/rotation/camera movement (GLUT callback)
 */
void myglutMousePressed(int button, int state, int x, int y) {
    g_camera.ProcessMouse(button, state, x, y);
}

/**
 * \brief Controls, Mouse, Camera, Menu - special Right click handling for menu
 */
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
/**
 * \brief mouse Pressed Move = Drag, stops cube from rotating after right click menu visible (GLUT callback)
 */
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

/**
 * \brief Camera, Window - Resize Window function passthrough to the camera class (GLUT callback)
 */
void myglutChangeWindowSize(int x, int y) {
    g_camera.ChangeViewportSize(x, y);
}

//see main-menu.cpp for rest of main GUI program
