///////////////////////////////////////////////////////////////////////////////
/* MegaMinx2 - v1.30 October24-December12 2017 - genBTC mod
             - v1.3.1 Nov 22, 2018
* Uses code originally from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
* genBTC 2017 + 2018- genbtc@gmx.com / @genr8_ / github.com/genbtc/
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
#include <time.h>
#include "engine/megaminx.h"
#include "common_physics/utils.h"
#include "common_physics/camera.h"
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
    g_areWeDraggingPoint = false;
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
    //std::random_device rd; std::mt19937 gen{rd()}; std::uniform_int_distribution<int>dis{1, 100000}; std::cout << dis(gen);
    sprintf_s(lastface, 32, "%ws", L"STARTUP");
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
    createMenu();   //right click menu
    glutMenuStatusFunc(menuVisible);
    //Glut Functs:
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(mousePressed);
    glutMotionFunc(mousePressedMove);
    glutPassiveMotionFunc(processMousePassiveMotion);
    glutKeyboardFunc(onKeyboard);
    glutSpecialFunc(onSpecialKeyPress);
    //Display and Loop forever
    glutIdleFunc(Idle);
    glutDisplayFunc(RenderScene);
    glutMainLoop();
    return 1;
}

//Actually limiting the FPS to REFRESH_RATE now:
void Idle()
{
    static const double REFRESH_TIME = 1.0 / REFRESH_RATE;
    // in milliseconds
    const int total = glutGet(GLUT_ELAPSED_TIME);
    const double newTime = (double)total * 0.001;

    double deltaTime = newTime - g_appTime;
    if (deltaTime > REFRESH_TIME) {
        deltaTime = REFRESH_TIME;
        //ReRender Scene:
        glutPostRedisplay();
        // set global:
        g_appTime = g_appTime + deltaTime;
    }
    /*
    // average?:
    static double lastDeltas[3] = { 0.0, 0.0, 0.0 };
    double deltaAvgTime = (deltaTime + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

    // save delta?:
    lastDeltas[0] = lastDeltas[1];
    lastDeltas[1] = lastDeltas[2];
    lastDeltas[2] = deltaTime;
    */
}

//OpenGL Render Func
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
            utPrintHelpMenu(WIDTH - 245.f, HEIGHT - 235.f);
        else
            utDrawText2D(WIDTH - 135.f, HEIGHT - 14.f, "h = help");
    }
    utResetPerspectiveProjection();
    //Clean up.
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHT1);
    glDisable(GLUT_MULTISAMPLE);
    glutSwapBuffers();
}

//query Megaminx for what face we're looking at?
void GetCurrentFace()
{
    const auto tempFace = getCurrentFaceFromAngles((int)g_camera.m_angleX, (int)g_camera.m_angleY);
    if (tempFace != 0) {
        currentFace = tempFace;
        wsprintf(lastface, "%ws", g_colorRGBs[currentFace].name);
        //Save it into the viewmodel (sync view)
        megaminx->setCurrentFaceActive(currentFace);
    }
}

//Shift key Directional Shortcut, Shift on = Counterclockwise.
//CAPS LOCK cannot work.
int GetDirFromSpecialKey()
{
    const int result = (glutGetModifiers() == GLUT_ACTIVE_SHIFT) ?
                       (int)Face::CCW : (int)Face::CW;
    return result;
}

//Double click Rotates Current Face with Shift Modifier.
void double_click(int , int )
{
    const int dir = GetDirFromSpecialKey();
    megaminx->rotate(currentFace, dir);
}
int menuVisibleState = 0;
void menuVisible(int status, int x, int y)
{
    menuVisibleState = status == GLUT_MENU_IN_USE;
}
void mousePressed(int button, int state, int x, int y)
{
    g_camera.ProcessMouse(button, state, x, y);

    // can we move?
    if(g_camera.m_isLeftPressed && g_rayTest.m_hit) {
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
    // Disregard redundant GLUT_UP events
    else if (state == GLUT_UP)
        return;
    //  Mouse Wheels are 3 and 4 on this platform. (usually 4 / 5) 
    if(button == 3) {
        //Mouse wheel up
        g_camera.m_zoom += 5;
    }
    else if (button == 4) {
        //Mouse wheel down
        g_camera.m_zoom -= 5;
    }
}

void processMousePassiveMotion(int x, int y)
{
    // called when no mouse btn are pressed and mouse moves
    // does nothing but record mouse position in camera class.
    //g_camera.ProcessPassiveMouseMotion(x, y);
}

void mousePressedMove(int x, int y)
{
    // if we are dragging any ball we cannot use mouse for scene rotation
    //TODO block from happening during right click menu visible.
    if (!menuVisibleState)
        g_camera.ProcessMouseMotion(x, y, !menuVisibleState);
}

//Switch for routing directional key commands - rotate neighbors of current face
void rotateDispatch(unsigned char key)
{
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
    default:
        break;
    }
}

//Help menu with Glut commands and line by line iteration built in.
void utPrintHelpMenu(float w, float h)
{
    constexpr char helpStrings[16][32] = { "Help Menu:",
                                           "[Right Click]  Action Menu",
                                           "[Dbl Click]  Rotate Current >>",
                                           "[F1-F12]     Rotate Face #  >>",
                                           "  +Shift  CounterClockwise <<",
                                           "[W/w]  Rotate Upper Face <>",
                                           "[S/s]  Rotate Front Face <>",
                                           "[A/a]  Rotate Side/Left  <>",
                                           "[D/d]  Rotate Side/Right <>",
                                           "[Z/z]  Rotate Diag/Left  <>",
                                           "[C/c]  Rotate Diag/Right <>",
                                           "[X/x]  Rotate Bottom Face <>",
                                           "[Space]  Toggle Auto-Spin",
                                           "[BackSpace]  Reset Camera",
                                           "[Delete]  Scramble Puzzle",
                                           "[Enter] Solve Current Face"
                                         };
    glColor3f(1, 1, 1); //White
    float incrementHeight = h;
    for (int i = 0; i < 16; i++) {
        utDrawText2D(w, incrementHeight, (char *)helpStrings[i]);
        incrementHeight += 15;
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
            exit(0);
        case 26: //Ctrl+Z
            megaminx->undo();
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
    //call the megaminx specific key functions (above)
    rotateDispatch(key);
}
//Secondary Keyboard Handler (Special Keys)
void onSpecialKeyPress(int key, int x, int y)
{
    //TODO make a Lua console to input these commands
    //TODO make a function to load in text files to set custom pieces
    //TODO this can be re-used to serialize and save out the config and reload it.
    const int dir = GetDirFromSpecialKey();
    switch (key) {
    case GLUT_KEY_PAGE_UP:
    case GLUT_KEY_PAGE_DOWN:
    case GLUT_KEY_HOME:
    case GLUT_KEY_END:
    case GLUT_KEY_INSERT:
        //Unused ^^
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
    //Route the arrow keys to the camera for motion
    g_camera.PressSpecialKey(key, x, y);
}

//Right Click Menu structure definitions.
void createMenu()
{
    //SubLevel 0 menu - Functions
    submenu0_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Edit... Undo", 91);
    glutAddMenuEntry("Solve All/(reset)", 92);
    glutAddMenuEntry("Reset Camera", 93);
    glutAddMenuEntry("Scramble", 100);
    glutAddMenuEntry("Quit", 102);

    //SubLevel 1 menu - Last Layer
    submenu1_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Solve Grey Star", 31);
    glutAddMenuEntry("Solve Grey Corners", 32);
    //TODO make these work
//    glutAddMenuEntry("Swap 1 Gray Edge", 33);
//    glutAddMenuEntry("Swap 1 Gray Corner", 34);
//    glutAddMenuEntry("Swap 2 Gray Corners", 35);

    //SubLevel2 Menu - Rotations
    submenu2_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Rotate Current Face CW", 50);
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
//  glutAddMenuEntry("2nd Layer Edges", 43);
//  glutAddMenuEntry("Low Y's", 44);
//  glutAddMenuEntry("4th Layer Edges", 45);
//  glutAddMenuEntry("High Y's", 46);
//  glutAddMenuEntry("6th Layer Edges", 47);

    //SubLevel4 Menu - Algos
    submenu4_id = glutCreateMenu(menuHandler);
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

//Right Click Menu event Handler
void menuHandler(int num)
{
    switch (num) {
    case 1:
        spinning = !spinning; break;
    case 20:
        megaminx->rotate(currentFace, Face::CW); break;
    case 21:
        megaminx->resetFace(currentFace); break;
    case 22:  //rotate edge piece
        megaminx->resetFacesEdges(currentFace); break;
    case 23:  //rotate corner piece
        megaminx->resetFacesCorners(currentFace); break;
    case 24:  //rotate edge piece (random)
        megaminx->swapOneEdge(currentFace, rand() % 5); break;
    case 25:  //rotate corner piece (random)
        megaminx->swapOneCorner(currentFace, rand() % 5); break;
    case 31:  //make GRAY edges (star)
        megaminx->resetFacesEdges(GRAY); break;
    case 32:  //make GRAY corners
        megaminx->resetFacesCorners(GRAY); break;
    case 33: ; //one gray edge
    case 34: ; //one gray corner
    case 35: ; //two gray corners
    case 41:  //make WHITE edges (star)
        megaminx->resetFacesEdges(WHITE); break;
    case 42:  //make WHITE Face
        megaminx->resetFace(WHITE); break;
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
        megaminx->rotateAlgo(currentFace, num - 50); break;
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
        megaminx->resetFace(num - 60); break;
    case 91:
        megaminx->undo(); break;
    case 92:
        createMegaMinx(); break;
    case 93:
        resetCameraView(); break;
    case 100:
        megaminx->scramble(); break;
    case 102: 
        glutDestroyWindow(1);
        exit(0); break;
    default:
        break;
    }
}
