///////////////////////////////////////////////////////////////////////////////
/* MegaMinx2 - v1.3 October24 - December12 2017 - genBTC mod
             - v1.3.2 Nov 22, 2018
             - v1.3.3 Dec 19, 2018
* Uses some code originally from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
* genBTC 2017 + 2018 / genbtc@gmx.com / @genr8_ / github.com/genbtc/
*/
// Headers
int main(int argc, char *argv[]);
#ifdef _WINDOWS
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { main(0, 0); }
#endif
#include <cstdlib>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//#include <GL/glui.h> TODO
#include <time.h>
#include "main.h"

//Resize Window glut callBack function passthrough to the camera class
void ChangeSize(int x, int y)
{
    g_camera.ChangeViewportSize(x, y);
}

//Camera - init/reset camera and vars, set view angles, etc.
void resetCameraView()
{
    g_camera = Camera();
    g_camera.m_zoom = -ZDIST;
    g_camera.m_angleY = START_ANGLE;
    g_camera.m_forced_aspect_ratio = 1;
    g_camera.g_areWeDraggingPoint = false;
    ChangeSize((int)WIDTH, (int)HEIGHT);
}
//Megaminx Dodecahedron. Creates object class and resets camera.
void createMegaMinx()
{
    if (megaminx)
        delete megaminx;
    megaminx = new Megaminx();
    resetCameraView();
}

//Entire Program is 28 lines,sorta.
int main(int argc, char *argv[])
{
    srand((int)time(nullptr));
    sprintf_s(lastface, 32, "%ws", L"STARTUPTITLE");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_DEPTH);
    glutInitWindowSize((int)WIDTH, (int)HEIGHT);
    window = glutCreateWindow(title);
    createMegaMinx();   //also includes glViewport(0,0,w,h);
    glClearColor(0.22f, 0.2f, 0.2f, 1.0f);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(view_distance_view_angle, 1.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //Set up GL Params in main so we dont have to recall them over and over in RenderScene()
    glEnable(GLUT_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA);
    glLineWidth(4);
    glPointSize(5);
    //Right click menu:
    createMenu();
    glutMenuStatusFunc(menuVisible);
    //Glut Functs:
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mousePressedMove);
    //glutPassiveMotionFunc(processMousePassiveMotion);
    glutKeyboardFunc(onKeyboard);
    glutSpecialFunc(onSpecialKeyPress);
    //Idle func, tracks frame-rate
    Idle(0);
    //Main: Display Render and Loop forever
    glutDisplayFunc(RenderScene);
    glutMainLoop();
    return 0;
}

//Wait for Refresh Rate, FrameRate Cap.
void Idle(int) {
    glutPostRedisplay();
    glutTimerFunc(REFRESH_WAIT - 1, Idle, 0);
}

//OpenGL Render Func
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    {
        // spinning can be toggled w/ spacebar
        if (spinning)
            g_camera.m_angleX += 0.5;
        //Rotate the Cube into View
        g_camera.RotateGLCameraView();
        //Render it.
        megaminx->render();
    }
    glPopMatrix();
    //Print out Text (FPS display + Angles + face Name)
    glColor3f(0, 1, 0); // ...in green.
    utSetOrthographicProjection(WIDTH, HEIGHT);
    {
        utCalculateAndPrintFps(10.f, 20.f);
        utCalculateAndPrintAngles(10.f, HEIGHT - 20.f, g_camera.m_angleX, g_camera.m_angleY);
        GetCurrentFace();
        utDrawText2D(10.f, HEIGHT - 40.f, lastface);
        //Print out Text (Help display)
        if (!help)
            utPrintHelpMenu(WIDTH - 245.f, HEIGHT - 265.f);
        else
            utDrawText2D(WIDTH - 130.f, HEIGHT - 14.f, "[H]elp");
        if (megaminx->isFullySolved() && !help)
            utDrawText2D(WIDTH - 130.f, HEIGHT - 28.f, "SOLVED!");
        else
            utDrawText2D(WIDTH - 130.f, HEIGHT - 28.f, "[F9] = SOLVER");
        shadowQueueLength = megaminx->getRotateQueueNum();
        if (shadowQueueLength > 0) {
            static char rotquestr[21];
            snprintf(rotquestr, 21, "Rotate Queue: %5d", shadowQueueLength);
            utDrawText2D((WIDTH / 2) - 80, HEIGHT - 12.f, rotquestr);
        }
        if (solveravg > 0) {
            static char solvquestr[21];
            snprintf(solvquestr, 21, "Solver Avg: %5g", solveravg);
            utDrawText2D((WIDTH / 2) - 80, HEIGHT - 12.f, solvquestr);
        }

    }
    utResetPerspectiveProjection();
    glutSwapBuffers();
}

//query Megaminx for what face we're looking at?
void GetCurrentFace()
{
    const int tempFace = getCurrentFaceFromAngles((int)g_camera.m_angleX, (int)g_camera.m_angleY);
    if (tempFace != 0) {
        currentFace = tempFace;
        sprintf_s(lastface, 32, "%ws", g_colorRGBs[currentFace].name);
        //Save it into the viewmodel (sync view)
        megaminx->setCurrentFaceActive(currentFace);
    }
}

//Shift key Directional Shortcut, Shift on = Counterclockwise.
//CAPS LOCK cannot work.
int GetDirFromSpecialKey()
{
    return (glutGetModifiers() == GLUT_ACTIVE_SHIFT) ? (int)Face::CCW : (int)Face::CW;
}

//Double click Rotates Current Face with Shift Modifier.
void double_click(int , int )
{
    megaminx->rotate(currentFace, GetDirFromSpecialKey());
}

void mousePressed(int button, int state, int x, int y)
{
    g_camera.ProcessMouse(button, state, x, y);
}

//FIXED BUG: stop cube rotate from happening right after we come back from right click menu visible then drag
int OldmenuVisibleState = 0;
int oldmenux = 0, oldmenuy = 0;
void menuVisible(int status, int x, int y)
{
    g_camera.menuVisibleState = (status == GLUT_MENU_IN_USE);
    if (g_camera.menuVisibleState) {
        OldmenuVisibleState = 1;
        oldmenux = x;
        oldmenuy = y;
    }
}
void mousePressedMove(int x, int y)
{
    if (!g_camera.menuVisibleState && !OldmenuVisibleState)
        g_camera.ProcessMouseMotion(x, y, true);
    else if (!g_camera.menuVisibleState && OldmenuVisibleState) {
        OldmenuVisibleState = 0;
        g_camera.ProcessMouseMotion(oldmenux, oldmenuy, false);
    }
}

//Main Keyboard Handler
void onKeyboard(unsigned char key, int x, int y)
{
    const auto specialKey = glutGetModifiers();
    if (specialKey == GLUT_ACTIVE_CTRL) {
        switch (key) {
        case 3: //Ctrl+C
            glutDestroyWindow(1);
            exit(0); break;
        case 26: //Ctrl+Z
            megaminx->undo();
            break;
        case 19: //CTRL+S //Save Game State
            SaveCubetoFile();
            break;
        case 18: //CTRL+R //Restore Game State
            RestoreCubeFromFile();
            break;
        default:
            break;
        }
    }
    switch (key) {
    case ' ': //spacebar 32
        spinning = !spinning;
        break;
    case 'h':
    case 'H':
        help = !help;
        break;
    case 8:   //backspace
        resetCameraView();
        break;
    case 13:    //enter
        megaminx->resetFace(currentFace);
        break;
    case 27:   //escape
        megaminx->resetQueue();
        break;
    case 127: //delete
        megaminx->scramble();
        break;
    default:
        break;
    }
    //Switch for routing directional key commands - rotate neighbors of current face
    //call the megaminx specific key functions 
    const int dir = GetDirFromSpecialKey();
    const auto face = g_faceNeighbors[currentFace];
    switch (key) {
    case 'w':   //Upper(Top)
    case 'W':
        megaminx->rotate(face.up, dir);
        break;
    case 's':   //Front
    case 'S':
        megaminx->rotate(face.front, dir);
        break;
    case 'a':   //Left
    case 'A':
        megaminx->rotate(face.left, dir);
        break;
    case 'd':   //Right
    case 'D':
        megaminx->rotate(face.right, dir);
        break;
    case 'z':   //Diagonal/Corner Left
    case 'Z':
        megaminx->rotate(face.downl, dir);
        break;
    case 'c':   //Diagonal/Corner Right
    case 'C':
        megaminx->rotate(face.downr, dir);
        break;
    case 'x':   //Bottom
    case 'X':
        megaminx->rotate(face.bottom, dir);
        break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        megaminx->flipCornerColor(currentFace, (int)key - 48); break;
    case '!':
    case '#':
    case '$':
    case '%':
        megaminx->flipEdgeColor(currentFace, (int)key - 32); break;
    case '@':
        megaminx->flipEdgeColor(currentFace, (int)key - 62); break;
    default:
        break;
    }
}
//Secondary Keyboard Handler (Special Keys)
void onSpecialKeyPress(int key, int x, int y)
{
    //TODO maybe make a Lua console to input these commands
    const int dir = GetDirFromSpecialKey();
    switch (key) {
    case GLUT_KEY_PAGE_UP:
        break;
    case GLUT_KEY_PAGE_DOWN:
        break;
    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;
    case GLUT_KEY_INSERT:
        break;
    case GLUT_KEY_F1:
        menuHandler(300);        //Rotate_white_edges +
        //menuHandler(301);     //Rotate_white_corners
        break; 
    case GLUT_KEY_F2:
        menuHandler(302); break; //rotate_2nd-layer-edges
    case GLUT_KEY_F3:
        menuHandler(303); break; //rotate_3rd_layer-corners
    case GLUT_KEY_F4:
        menuHandler(304); break; //rotate_4th_layer-edges
    case GLUT_KEY_F5:
        menuHandler(305); break; //rotate_5th_layer-corners 
    case GLUT_KEY_F6:
        menuHandler(306); break; //rotate_6th_layer-edges
    case GLUT_KEY_F7:
        menuHandler(307); break; //rotate_7th_layer-edges
    case GLUT_KEY_F8:
        menuHandler(308); break; //rotate_7th_layer-corners
    case GLUT_KEY_F9:
        menuHandler(309); break; //Layers 1-7 all at once 
    case GLUT_KEY_F10:
    case GLUT_KEY_F11:
        //break;
    case GLUT_KEY_F12:
        menuHandler(312); break; //Brute-Force Checker for solver. stomps on savefile.
    default:
        break;
    }
    //Route the arrow keys to the camera for motion
    g_camera.PressSpecialKey(key, x, y);
}
