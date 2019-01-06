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
    return 42;
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
        wsprintf(lastface, "%ws", g_colorRGBs[currentFace].name);
        //Save it into the viewmodel (sync view)
        megaminx->setCurrentFaceActive(currentFace);
    }
}

//Shift key Directional Shortcut, Shift on = Counterclockwise.
//CAPS LOCK cannot work.
int GetDirFromSpecialKey()
{
    return (glutGetModifiers() == GLUT_ACTIVE_SHIFT) ?
            (int)Face::CCW : (int)Face::CW;
}

//Double click Rotates Current Face with Shift Modifier.
void double_click(int , int )
{
    megaminx->rotate(currentFace, GetDirFromSpecialKey());
}

void mousePressed(int button, int state, int x, int y)
{
    g_camera.ProcessMouse(button, state, x, y);

    // can we move?
    if(g_camera.m_isLeftPressed){// && g_rayTest.m_hit) {
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

//FIXED: FINALLY block cube rotate from happening right after we come back from right click menu visible:
static int OldmenuVisibleState = 0;
void menuVisible(int status, int x, int y)
{
    menuVisibleState = (status == GLUT_MENU_IN_USE);
    if (menuVisibleState)
        OldmenuVisibleState = 1;
}
void mousePressedMove(int x, int y)
{
    //TODO: X/Y coords still change in the background when Menu is showing, and the first drag after will "jump to pos"
    if (!menuVisibleState && OldmenuVisibleState) {
        OldmenuVisibleState = 0;  //maybe wanna store old x and y here then re-kajigger it ?
        return;
    }
    if (!menuVisibleState && !OldmenuVisibleState)
        g_camera.ProcessMouseMotion(x, y, !menuVisibleState);        
}

//Help menu with Glut commands and line by line iteration built in.
void utPrintHelpMenu(float w, float h)
{
    constexpr char helpStrings[18][32] = { "[H]elp Menu:",
                                           "[Right Click]  Actions Menu",
                                           "[Dbl Click]  Rotate Current >>",
                                           "[F1-F12]     Rotate Face #  >>",
                                           "  +Shift  CounterClockwise <<",
                                           "1,2,3,4,5 Flip Curr. Corner #",
                                           "!,@,#,$,% Flip Curr. Edge  #",
                                           "[W/w]  Rotate Upper Face </>",
                                           "[S/s]  Rotate Front Face </>",
                                           "[A/a]  Rotate Left  Face </>",
                                           "[D/d]  Rotate Right Face </>",
                                           "[Z/z]  Rotate Diag/Left  </>",
                                           "[C/c]  Rotate Diag/Right </>",
                                           "[X/x]  Rotate Bottom Face </>",
                                           "[Space]  Toggle Auto-Spinning",
                                           "[BackSpace]  Reset Camera Pos",
                                           "[Delete]  Scramble Puzzle",
                                           "[Enter] Solve Current Face"
                                         };
    glColor3f(1, 1, 1); //White
    float incrementHeight = h;
    for (int i = 0; i < 18; i++) {
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
    //TODO make a Lua console to input these commands
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
    //SubLevel 0 menu - Main Menu
    submenu0_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Camera Home Pos.", 93);
    glutAddMenuEntry("Edit... Undo", 91);
    glutAddMenuEntry("Edit... Undo By Two", 94);
    glutAddMenuEntry("Edit... Undo Quad", 95);
    glutAddMenuEntry("Scramble [Del]", 100);
    //Sublevel 1 meun - Admin Mode
    //submenu1_id = glutCreateMenu(menuHandler);    
    glutAddMenuEntry("Save Cube State", 98);
    glutAddMenuEntry("Restore Cube State", 99);
    glutAddMenuEntry("Exit!", 102);

    //SubLevel2 Menu - Current Face
    submenu2_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Rotate CCW <<", 19);
    glutAddMenuEntry("Rotate  CW >>", 20);
    //glutAddMenuEntry("-----------------------", 0);
    glutAddMenuEntry("Place/Solve Entire Face", 21);
    glutAddMenuEntry("Place/Solve Five Edges", 22);
    glutAddMenuEntry("Place/Solve Five Corners", 23);
    //glutAddMenuEntry("-----------------------", 0);
    glutAddMenuEntry("Flip Colors Edge 1", 24);
    glutAddMenuEntry("Flip Colors Edge 2", 25);
    glutAddMenuEntry("Flip Colors Edge 3", 26);
    glutAddMenuEntry("Flip Colors Edge 4", 27);
    glutAddMenuEntry("Flip Colors Edge 5", 28);
    glutAddMenuEntry("Flip Colors Corner 1", 29);    
    glutAddMenuEntry("Flip Colors Corner 2", 30);    
    glutAddMenuEntry("Flip Colors Corner 3", 31);
    glutAddMenuEntry("Flip Colors Corner 4", 32);
    glutAddMenuEntry("Flip Colors Corner 5", 33);

    submenu6_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Swap Edges 1 & 2", 125);
    glutAddMenuEntry("Swap Edges 1 & 3", 126);
    glutAddMenuEntry("Swap Edges 1 & 4", 127);
    glutAddMenuEntry("Swap Edges 1 & 5", 128);
    glutAddMenuEntry("Swap Edges 2 & 3", 129);
    glutAddMenuEntry("Swap Edges 2 & 4", 130);
    glutAddMenuEntry("Swap Edges 2 & 5", 131);
    glutAddMenuEntry("Swap Edges 3 & 4", 132);
    glutAddMenuEntry("Swap Edges 3 & 5", 133);
    glutAddMenuEntry("Swap Edges 4 & 5", 134);
    glutAddMenuEntry("Swap Corners 1 & 2", 135);
    glutAddMenuEntry("Swap Corners 1 & 3", 136);
    glutAddMenuEntry("Swap Corners 1 & 4", 137);
    glutAddMenuEntry("Swap Corners 1 & 5", 138);
    glutAddMenuEntry("Swap Corners 2 & 3", 139);
    glutAddMenuEntry("Swap Corners 2 & 4", 140);
    glutAddMenuEntry("Swap Corners 2 & 5", 141);
    glutAddMenuEntry("Swap Corners 3 & 4", 142);
    glutAddMenuEntry("Swap Corners 3 & 5", 143);
    glutAddMenuEntry("Swap Corners 4 & 5", 144);

    //SubLevel3 Menu - Auto Solve Steps
    submenu3_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("* Solve Entire Puzzle *", 92);
    glutAddMenuEntry("1st Layer White Star", 41);
    glutAddMenuEntry("1st Layer White Corners", 42);
    glutAddMenuEntry("2nd Layer Edges", 151);
    glutAddMenuEntry("3rd Layer Low Y's", 152);
    glutAddMenuEntry("4th Layer Edges", 153);
    glutAddMenuEntry("5th Layer High Y's", 154);
    glutAddMenuEntry("6th Layer Edges", 155);
    glutAddMenuEntry("Last Layer Grey Star", 156);
    glutAddMenuEntry("Last Layer Grey Corners", 157);

    //SubLevel4 Menu - Human Rotate Algos
    submenu4_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("r u R' U'", 51);
    glutAddMenuEntry("l u L' U'", 52);
    glutAddMenuEntry("U' L' u l", 53);
    glutAddMenuEntry("u r U' R'", 150);
    glutAddMenuEntry("r u R' u r 2U' R'", 54);
    glutAddMenuEntry("u l U' R' u L' U' r", 55);
    glutAddMenuEntry("u r 2U' L' 2u R' 2U' l u", 56);
    glutAddMenuEntry("R' D' R D", 57);
    glutAddMenuEntry("R' D' R D x4", 257);
    glutAddMenuEntry("Edge Permu 1+ x5", 58);
    glutAddMenuEntry("Edge Permu 2- x5", 59);
    glutAddMenuEntry("Edge Permu 3+CORNERS Front=Safe", 60);
    glutAddMenuEntry("Edge Permu 4a+ Front/Left=Safe", 61);
    glutAddMenuEntry("Edge Permu 4b- Both/Back=Safe", 251);
    glutAddMenuEntry("Edge Permu 4c- Right/Back=Safe", 252);
    glutAddMenuEntry("Edge Permu 5+CORNERS", 62);
    glutAddMenuEntry("Edge Permu LL #203", 253);
    glutAddMenuEntry("Edge Permu LL #204", 254);
    glutAddMenuEntry("Edge Permu LL #205", 255);
    glutAddMenuEntry("Edge Permu LL #206", 256);
    glutAddMenuEntry("2nd Layer Star Left", 68);
    glutAddMenuEntry("2nd Layer Star Right", 67);
    //Find desired edge piece, surf it around to the gray layer, then back down to the top of the star either to the left or the right of dropping it into place.
    //Drop in procedure: Move star-top away from the drop-in location, then spin the R/L side UP (the side thats opposite of the star-top turn-away direction) (up is either CW or CCW depending on the side) and then rotate both back
    //this will group the correct edge to the correct the corner, above the drop-in location. A second similar drop-in move is needed, likely "u r U' R'" or "u l U' L'"
    //3rd layer Corners = Low Y's involve flipping the puzzle upside down, white face on top, and positioning the desired piece on the bottom layer, then swiveling the bottom face around to orient it,
    //and then rotating it up and into the Low Y. since the entire rest of the puzzle is unsolved, this can be done intuitively, just rotate the piece on the bottom until its colored correctly, then drop it in.
    glutAddMenuEntry("4th Layer Star Left", 64);
    glutAddMenuEntry("4th Layer Star Right", 63);
    glutAddMenuEntry("6th Layer Star Left", 65);
    glutAddMenuEntry("6th Layer Star Right", 66);

    //SubLevel5 Menu - Reset Faces
    submenu5_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry(" 1 WHITE", 71);
    glutAddMenuEntry(" 2 DARK_BLUE", 72);
    glutAddMenuEntry(" 3 RED", 73);
    glutAddMenuEntry(" 4 DARK_GREEN", 74);
    glutAddMenuEntry(" 5 PURPLE", 75);
    glutAddMenuEntry(" 6 YELLOW", 76);
    glutAddMenuEntry(" 7 GRAY", 77);
    glutAddMenuEntry(" 8 LIGHT_BLUE", 78);
    glutAddMenuEntry(" 9 ORANGE", 79);
    glutAddMenuEntry("10 LIGHT_GREEN", 80);
    glutAddMenuEntry("11 PINK", 81);
    glutAddMenuEntry("12 BEIGE", 82);

    //Top Level - Main Menu
    menu_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Toggle Spinning", 1);
    glutAddSubMenu("Main Menu ---->", submenu0_id);
    //glutAddMenuEntry("---------------", 0);
    //glutAddSubMenu("Admin Mode --->", submenu1_id);
    glutAddSubMenu("Current Face ->", submenu2_id);
    glutAddSubMenu("Piece Swaps -->", submenu6_id);
    glutAddSubMenu("Algorithms --->", submenu4_id);
    glutAddSubMenu("Auto Solve --->", submenu3_id);
    glutAddSubMenu("Solve Face --->", submenu5_id);    
    glutAddMenuEntry("Close Menu...", 9999);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//Right Click Menu event Handler
void menuHandler(int num)
{
    switch (num) {
    case 1:
        spinning = !spinning; break;
    case 19:
        megaminx->rotate(currentFace, Face::CCW); break;
    case 20:
        megaminx->rotate(currentFace, Face::CW); break;
    case 21:
        megaminx->resetFace(currentFace); break;
    case 22:  //place/solve all 5 edges
        megaminx->resetFacesEdges(currentFace); break;
    case 23:  //place/solve all 5 corners
        megaminx->resetFacesCorners(currentFace); break;
    case 24:  //color flip edge piece 1
    case 25:  //color flip edge piece 2
    case 26:  //color flip edge piece 3
    case 27:  //color flip edge piece 4
    case 28:  //color flip edge piece 5
        megaminx->flipEdgeColor(currentFace, num - 23); break;
    case 29:  //color flip corner piece 1
    case 30:  //color flip corner piece 2
    case 31:  //color flip corner piece 3
    case 32:  //color flip corner piece 4
    case 33:  //color flip corner piece 5
        megaminx->flipCornerColor(currentFace, num - 28); break;
    case 41:  //1st Layer = WHITE Edges
        megaminx->resetFacesEdges(WHITE); break;
    case 42:  //1st Layer = WHITE Corners
        megaminx->resetFacesCorners(WHITE); break;
    case 151: //2nd Layer
        megaminx->secondLayerEdges(); break;
    case 152: //3rd Layer
        megaminx->lowYmiddleW(); break;
    case 153: //4th Layer
        megaminx->fourthLayerEdges(); break;
    case 154: //5th Layer
        megaminx->highYmiddleW(); break;
    case 155: //6th Layer
        megaminx->sixthLayerEdges(); break;
    case 156:  //Last Layer GRAY Edges
        megaminx->resetFacesEdges(GRAY); break;
    case 157:  //Last Layer GRAY Corners
        megaminx->resetFacesCorners(GRAY); break;
    case 51:
    case 52:
    case 53:
    case 150:
    case 54:
    case 55:
    case 56:
    case 57:
    case 257:
    case 58:
    case 59:
    case 60:
    case 61:
    case 251:
    case 252:
    case 253:
    case 254:
    case 255:
    case 256:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
        megaminx->rotateAlgo(currentFace, num - 50); break;
    //Solve Faces (Reset) 1-12:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
        megaminx->resetFace(num - 70); break;
    case 91:
        megaminx->undo(); break;
    case 92:
        createMegaMinx(); break;
    case 93:
        resetCameraView(); break;
    case 94:
        megaminx->undoDouble(); break;
    case 95:
        megaminx->undoQuad(); break;
    case 100:
        megaminx->scramble(); break;
    case 102: 
        glutDestroyWindow(1);
        exit(0); break;
    case 125: //Edge Piece Swaps
        megaminx->g_currentFace->edge[0]->swapdata(megaminx->g_currentFace->edge[1]->data); break;
    case 126:
        megaminx->g_currentFace->edge[0]->swapdata(megaminx->g_currentFace->edge[2]->data); break;
    case 127:
        megaminx->g_currentFace->edge[0]->swapdata(megaminx->g_currentFace->edge[3]->data); break;
    case 128:
        megaminx->g_currentFace->edge[0]->swapdata(megaminx->g_currentFace->edge[4]->data); break;
    case 129:
        megaminx->g_currentFace->edge[1]->swapdata(megaminx->g_currentFace->edge[2]->data); break;
    case 130:
        megaminx->g_currentFace->edge[1]->swapdata(megaminx->g_currentFace->edge[3]->data); break;
    case 131:
        megaminx->g_currentFace->edge[1]->swapdata(megaminx->g_currentFace->edge[4]->data); break;
    case 132:
        megaminx->g_currentFace->edge[2]->swapdata(megaminx->g_currentFace->edge[3]->data); break;
    case 133:
        megaminx->g_currentFace->edge[2]->swapdata(megaminx->g_currentFace->edge[4]->data); break;
    case 134:
        megaminx->g_currentFace->edge[3]->swapdata(megaminx->g_currentFace->edge[4]->data); break;
    case 135: //Corner Piece Swaps
        megaminx->g_currentFace->corner[0]->swapdata(megaminx->g_currentFace->corner[1]->data); break;
    case 136:
        megaminx->g_currentFace->corner[0]->swapdata(megaminx->g_currentFace->corner[2]->data); break;
    case 137:
        megaminx->g_currentFace->corner[0]->swapdata(megaminx->g_currentFace->corner[3]->data); break;
    case 138:
        megaminx->g_currentFace->corner[0]->swapdata(megaminx->g_currentFace->corner[4]->data); break;
    case 139:
        megaminx->g_currentFace->corner[1]->swapdata(megaminx->g_currentFace->corner[2]->data); break;
    case 140:
        megaminx->g_currentFace->corner[1]->swapdata(megaminx->g_currentFace->corner[3]->data); break;
    case 141:
        megaminx->g_currentFace->corner[1]->swapdata(megaminx->g_currentFace->corner[4]->data); break;
    case 142:
        megaminx->g_currentFace->corner[2]->swapdata(megaminx->g_currentFace->corner[3]->data); break;
    case 143:
        megaminx->g_currentFace->corner[2]->swapdata(megaminx->g_currentFace->corner[4]->data); break;
    case 144:
        megaminx->g_currentFace->corner[3]->swapdata(megaminx->g_currentFace->corner[4]->data); break;
    case 98: //Save Game State
        FromCubeToVectorFile();
        break;
    case 99: //Restore Game State
        FromVectorFileToCube();
        break;
    default:
        break;
    }
}

//Save State filenames
#define EDGEFILE "EdgeCurPos.dat"
#define CORNERFILE "CornerCurPos.dat"
#define EDGEFILECOLORS "EdgeColorPos.dat"
#define CORNERFILECOLORS "CornerColorPos.dat"
//Store (Write)
void FromCubeToVectorFile() {
    WritePiecesFile(EDGEFILE,false);
    WritePiecesFile(CORNERFILE,true);
    serializeVectorInt(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}
//Load (Read)
void FromVectorFileToCube() {
    const std::vector<int> &readEdgevector = ReadPiecesFileVector(EDGEFILE);
    const std::vector<int> &readEdgeColorvector = ReadPiecesFileVector(EDGEFILECOLORS);
    megaminx->LoadNewEdgesFromVector(readEdgevector, readEdgeColorvector);
    //megaminx->LoadNewEdgesFromVector(readEdgevector);
    //megaminx->LoadNewEdgesFromVector(readEdgevector);
    const std::vector<int> &readCornervector = ReadPiecesFileVector(CORNERFILE);
    const std::vector<int> &readCornerColorvector = ReadPiecesFileVector(CORNERFILECOLORS);
    megaminx->LoadNewCornersFromVector(readCornervector, readCornerColorvector);
    //megaminx->LoadNewCornersFromVector(readCornervector);
    //megaminx->LoadNewCornersFromVector(readCornervector);
}

void serializeVectorInt(std::vector<int> list1, std::string filename) {
    std::ofstream file(filename);
    file << "{ ";
    for (auto l : list1) {
        file << l << ", ";
    }
    file << " }\n";
    file.close();
}

void WritePiecesFile(std::string filename, bool corner)
{
    std::ofstream file(filename);
    for (int i = 1; i <= 12; ++i) {
        file << "----------[ " << i << " ]----------\n";
        std::vector<int> f = corner ? megaminx->findCorners(i) : megaminx->findEdges(i);
        file << f[0] << ", " << f[1] << ", " << f[2] << ", " << f[3] << ", " << f[4] << "\n";
    }
    file.close();
}

const std::vector<int> ReadPiecesFileVector(std::string filename)
{
    std::ifstream input(filename);
    std::vector<int> readvector;
    std::string line;                          // iterate each line
    while (std::getline(input, line)) {        // getline returns the stream by reference, so this handles EOF
        std::stringstream ss(line);            // create a stringstream out of each line
        int readint = -1;                      // start a new node
        while (ss) {                           // while the stream is good
            std::string word;                  // get first word
            if (ss >> word) {
                if ((word.find("{") == 0) || (word.find("}") == 0))
                    continue;
                if ((word.find("---") == 0) || (word[0] == '-'))
                    break;
                readint = std::stoi(word); //convert to int
                if (readint < 0)
                    break;
                readvector.push_back(readint);
            }
        }        
    }
    //TEST:
    //serializeVectorInt(readvector, "ReOutput" + filename );
    return readvector;
}
