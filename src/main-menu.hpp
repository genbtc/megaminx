// main-menu.hpp

int GetDirFromSpecialKey();
void doDoubleClickRotate(int x, int y);
void doCameraMotionSpecial(int key, int x, int y);
void menuHandler(int num);
static int submenu0_id, submenu1_id, submenu2_id, submenu3_id,
           submenu4_id, submenu5_id, submenu6_id, submenu7_id, menu_id;

void createMegaMinx();
void resetCameraViewport();
void isSpinning();

// defined in main.cpp
extern int currentFace;
extern bool g_help;
extern double g_solveravg;

// defined in readline.cpp
void readlineShell();