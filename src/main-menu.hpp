// GLUT main.cpp - Glut function callbacks prototypes

int GetDirFromSpecialKey();
void doDoubleClickRotate(int x, int y);
void doCameraMotionSpecial(int key, int x, int y);
void menuHandler(int num);
static int submenu0_id, submenu1_id, submenu2_id, submenu3_id,
           submenu4_id, submenu5_id, submenu6_id, submenu7_id, menu_id;

extern int currentFace;
extern bool help;
extern double solveravg;

void createMegaMinx();
void resetCameraViewport();
void isSpinning();