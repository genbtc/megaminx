#ifndef __MAIN_H__
#define __MAIN_H__
///////////////////////////////////////////////////////////////////////
//framerate cap for monitor with 60 Hz:
const double REFRESH_RATE = 60.0;
const double REFRESH_IN_MS = (1000.0 / REFRESH_RATE);
const unsigned int REFRESH_WAIT_TIME = (const unsigned int)REFRESH_IN_MS;
// initial window screen size
const int WIDTH = 640;
const int HEIGHT = WIDTH; //uncomment if not 1:1
const double aspectRatio = 1.0; //(WIDTH / HEIGHT)
const double ZDIST = /* aspectRatio* */ 1.25 * HEIGHT;
const double START_ANGLE = 60.0f;   //must replicate to camera.cpp@getCurrentFaceFromAngles().s
const double fovy = 20.;    //gluPerspective - fovy (view_distance_view_angle)
const double ZNEAR = 1.0;   //gluPerspective - zNear
const double ZFAR = 10000.; //gluPerspective - zFar
////////////////////////////////////////////////////////////////////////
// main / main.cpp
int main(int argc, char *argv[]);

// global refs - megaminx.cpp
void createMegaMinx();
void GetCurrentFace();

// global camera main.cpp -> camera.cpp
void doCameraMotionSpecial(int key, int x, int y);
void resetCameraViewport();
void toggleSpinning();

// glut callbacks
void myglutIdle(int);
void myglutRenderScene();
void myglutMouseFunc(int button, int state, int x, int y);
void myglutMousePressedMove(int x, int y);
void myglutChangeWindowSize(int x, int y);

// main-menu.cpp
void createMenu();
void myglutOnKeyboard(unsigned char key, int x, int y);
void myglutOnSpecialKeyPress(int key, int x, int y);
void myglutMenuVisible(int status, int x, int y);
void utPrintHelpMenu(float w, float h);
int GetDirFromSpecialKey();

/** logging will be done by using simple "printf" function */
#define UT_LOG_WITH_PRINTF
#ifdef UT_LOG_WITH_PRINTF
#define utLOG(msg, ...) { printf("%s: ", __FUNCTION__); printf(msg, __VA_ARGS__); printf("\n"); }
#define utLOG_ERROR(msg, ...) { printf("ERR in %s, %d: ", __FUNCTION__, __LINE__); printf(msg, __VA_ARGS__); printf("\n"); }
#endif

#endif // __MAIN_H__
