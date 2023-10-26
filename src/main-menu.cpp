#include "main-menu.hpp"
#include "engine/megaminx.hpp"
#include "engine/load.hpp"
#include "main.h"

// Main Keyboard Handler
void myglutOnKeyboard(unsigned char key, int x, int y) {
	//Ctrl + keys first
    const auto specialKey = glutGetModifiers();
    if (specialKey & GLUT_ACTIVE_CTRL) {
        switch (key) {
        case 'c': // Ctrl+C
        case 3:  // Ctrl+C
            glutDestroyWindow(1);
            exit(0);
            break;
        case 'z': // Ctrl+Z
        case 26: // Ctrl+Z
            megaminx->undo();
            break;
        case 's': // CTRL+S //Save Game State
        case 19: // CTRL+S //Save Game State
            SaveCubetoFile();
            break;
        case 'r': // CTRL+R //Restore Game State
        case 18: // CTRL+R //Restore Game State
            RestoreCubeFromFile();
            break;
        case 8: // Ctrl+backspace
            createMegaMinx();
            break;
        default:
            break;
        }
    }
    //Game commands
    switch (key) {
    case ' ':	// spacebar 32
        toggleSpinning();
        break;
    case 'h':
    case 'H':	// help
        g_help = !g_help;
        break;
    case 8:		// backspace
        resetCameraViewport();
        break;
    case 13:	// enter
        readlineShell();
        break;
    case 27:	// escape
        megaminx->resetQueue();
        break;
    case 127:	// delete
        megaminx->scramble();
        break;
    default:
        break;
    }
    // Cube Rotation commands
    // rotate neighbors of current face (call relational rotation function)
    const int dir = GetDirFromSpecialKey();
    const auto face = g_faceNeighbors[currentFace];
    switch (key) {
    //Direction of Rotation
    case 'w': // Upper Face (Top)
    case 'W':
        megaminx->rotate(face.up, dir);
        break;
    case 's': // Front
    case 'S':
        megaminx->rotate(face.front, dir);
        break;
    case 'a': // Left
    case 'A':
        megaminx->rotate(face.left, dir);
        break;
    case 'd': // Right
    case 'D':
        megaminx->rotate(face.right, dir);
        break;
    case 'z': // Diagonal/Corner Left Lower
    case 'Z':
        megaminx->rotate(face.downl, dir);
        break;
    case 'c': // Diagonal/Corner Right Lower
    case 'C':
        megaminx->rotate(face.downr, dir);
        break;
    case 'x': // Bottom
    case 'X':
        megaminx->rotate(face.bottom, dir);
        break;
    //Teleport Flips
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':   //Color
        megaminx->flipCornerColor(currentFace, (int)key - 48);
        break;
    case '!':
    case '#':
    case '$':
    case '%':   //Edge (1,3,4,5)
        megaminx->flipEdgeColor(currentFace, (int)key - 32);
        break;
    case '@':   //Edge 2 (char symbol discontinuity)
        megaminx->flipEdgeColor(currentFace, (int)key - 62);
        break;
    default:
        break;
    }
}
// Secondary Keyboard Handler (Special Keys)
void myglutOnSpecialKeyPress(int key, int x, int y) {
    //const int dir = GetDirFromSpecialKey();
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
    //DEV: THESE GLUT FUNC KEYS _HAVE_ TO BE HANDLED IN HERE, but dont encode too much logic in the menu
    case GLUT_KEY_F1:
        //DEV: manually call these (the double clone cube call was messed up)
        MakeShadowCubeClone();	// init
        megaminx->rotateSolveWhiteEdges(shadowDom);
        megaminx->rotateSolveWhiteCorners(shadowDom);
        break; // rotate_1st-layer-edges+corners
    case GLUT_KEY_F2:
        menuHandler(302);
        break; // rotate_2nd-layer-edges
    case GLUT_KEY_F3:
        menuHandler(303);
        break; // rotate_3rd_layer-corners
    case GLUT_KEY_F4:
        menuHandler(304);
        break; // rotate_4th_layer-edges
    case GLUT_KEY_F5:
        menuHandler(305);
        break; // rotate_5th_layer-corners
    case GLUT_KEY_F6:
        menuHandler(306);
        break; // rotate_6th_layer-edges
    case GLUT_KEY_F7:
        menuHandler(307);
        break; // rotate_7th_layer-edges
    case GLUT_KEY_F8:
        menuHandler(308);
        break; // rotate_7th_layer-corners
    case GLUT_KEY_F9:
        menuHandler(309);
        break; // Layers 1-7 ALL at once
    case GLUT_KEY_F10:  //inactive
    case GLUT_KEY_F11:  //inactive
        // break;
    case GLUT_KEY_F12:
        menuHandler(312);
        break; // Brute-Force Checker for solver. NOTE:(stomps on savefile)
    default:
        break;
    }
    // Route the arrow keys to the camera for motion (main.cpp)
    doCameraMotionSpecial(key, x, y);
}

//Help menu with Glut commands and line by line iteration built in.
void utPrintHelpMenu(float w, float h)
{
    constexpr char helpStrings[18][32] = { "[H]elp Menu:",
                                           "[Right Click]  Actions Menu",
                                           "[Dbl Click]  Rotate Current CW>",
                                           "  +Shift  CounterClockwise <<",
                                           "1,2,3,4,5 Flip Edge Color #",
                                           "!,@,#,$,% Flip Corner Color #",
                                           "[W/w]  Rotate Upper Face </>",
                                           "[S/s]  Rotate Front Face </>",
                                           "[A/a]  Rotate Left  Face </>",
                                           "[D/d]  Rotate Right Face </>",
                                           "[Z/z]  Rotate DownLeft   </>",
                                           "[C/c]  Rotate DownRight  </>",
                                           "[X/x]  Rotate Bottom Face </>",
                                           "[BackSpace] Default Camera Pos",
                                           "[Space]  Toggle Auto-Spinning",
                                           "[Delete]  Scramble Puzzle",
                                           "[Enter]  Default Current Face",
                                           "[F1-F8=F9] Layer'# Human Solver",
    };
    glColor3f(1, 1, 1); //White
    float incrementHeight = h;
    for (int i = 0; i < 18; i++) {
        utDrawText2D(w, incrementHeight, helpStrings[i]);
        incrementHeight += 15;
    }
}


//Right Click Menu text/function mappings, and actual user interface front-end logic
void createMenu()
{
    StringAlgo la={};
    //SubLevel 0 menu - Main Menu
    submenu0_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Camera go Home", 93);
    glutAddMenuEntry("Edit.Undo [^Z]", 91);
    glutAddMenuEntry("Edit.Undo*2", 94);
    glutAddMenuEntry("Edit.Undo*4", 95);
    glutAddMenuEntry("Edit.Undo-Seq", 96);
    glutAddMenuEntry("Scramble! [Del]", 100);
    glutAddMenuEntry("New Cube...", 92);
    glutAddMenuEntry("Save Cube...", 98);
    glutAddMenuEntry("Restore Cube...", 99);
    glutAddMenuEntry("Readline Shell:>", 101);
    glutAddMenuEntry("Exit!", 102);

    //SubLevel4 Menu - Human Bulk Rotate Algos
    submenu4_id = glutCreateMenu(menuHandler);
//NOTE:I consider these 3 below redundant
//  glutAddMenuEntry("32. 7LL-Edge3d- CCW MUSHROOM- Both+Backs=Safe", 82);
//  glutAddMenuEntry("15. 7LL-Edge3b+  CW MUSHROOM+ Both+Backs=Safe", 65);
//  glutAddMenuEntry("16. 7LL-Edge3c+  CW MUSHROOM+ Right/Back=Safe", 66);
// -glutAddMenuEntry("18. 7LL-Edge 5-way CCW cycle by 1,2,-1,2,1", 68);
//NOTE:I consider these 2 below redundant:
//  glutAddMenuEntry("25. LL Corners: Cycle- CCW L.Back=Safe", 75);
//  glutAddMenuEntry("28. LL Corners: Cycle- CCW Left=Safe", 78);
    //glutAddMenuEntry("03. U' L' u l", 53);
    //glutAddMenuEntry("04. u r U' R'", 54);
    constexpr int MAX_HUMANBULK_ALGORITHMS = 25;
    constexpr StringAlgo humanbulk_algorithms[MAX_HUMANBULK_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { "01. Corners r u R' U'", 51 },
        { "02. Corners l u L' U'", 52 },
        { "14. 7LL-Edge3a- CCW MUSHROOM- Fr./L.=Safe", 64 },
        { "33. 7LL-Edge3e+  CW MUSHROOM+ Fr./L.=Safe", 83 },
        { "12. 7LL-Edge1-  CCW x5 HORSE- Fr./R.Back=Safe", 62 },
        { "13. 7LL-Edge2+   CW x5 HORSE+ Fr./R.Back=Safe", 63 },
        { "20. 7LL-Edge 2+2swap BUNNY Opp. 2&4,3&5 + INVERT 8'/1'", 70 },
        { "37. 7LL-Edge 2+2swap BUNNY BEST 2&5,3&4", 87 },
        { "19. 7LL-Edge 2+2swap BUNNY Adj. 2&3,4&5", 69 },
        { "29. 7LL-Edge 2+2swap BUNNY Colors ONLY 2,3,4,5", 79 },
        { "38. 7LL-Edge 5-way CCW cycle by +2 all", 88 },
        { "17. 7LL-Edge 5-way  CW cycle by -2 all", 67 },
        { "39. 7LL-Edge 5-way CCW cycle by 1,2,-1,2,1", 189 },
        { "43. 7LL-Edge 5-way  CW cycle by -1,-2,1,-2,-1", 193 },
        { "11. 7LL-E+C #1/3 CCW HORSEdge- Fr./R.Back=Safe", 61 },
        { "10. 7LL-E+C #2 CCW MUSHEdge- Fr./L.=Safe", 60 },
        { "08. 7LL-E+C #3  CW MUSHEdge+ Fr.Line/L.Edge=Safe", 58 },
        { "09. 7LL-E+C 2+2swap BUNNY 1&3,2&4 R.Back=Safe", 59 },
        { "36. 7LL-E+C 2+2swap BUNNY 2&5,3&4 Front=Safe", 86 },
        { "31. 7LL-E+C x1 CCW HORSEdge- Fr./R.Back=Safe", 81 },
        { "30. 7LL-E+C x1  CW HORSEdge+ Fr./R.Back=Safe", 80 },
        { "50. 7LL Corner: R' D' r dr (Permute) C2+Safe", 200 },
        { "26. 7LL Corners: Cycle- CCW FrontLine=Safe", 76 },
        { "27. 7LL Corners: Cycle+  CW RightLine=Safe", 77 },
    };
    //(goes into menu submenu4_id: Algorithms by Number) 51-88(-50),189-202(-150)
    for (int i = 1; i < MAX_HUMANBULK_ALGORITHMS; i++) {
        la = humanbulk_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }

    submenu7_id = glutCreateMenu(menuHandler);
    //Sublevel Y = Human Manual Rotate Routines (insert one piece manually into layer)
    constexpr int MAX_HUMANLAYER_ALGORITHMS = 9;
    constexpr StringAlgo humanlayer_algorithms[MAX_HUMANLAYER_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { "2nd Layer, 1 Edge (Left drop)",  291 },
        { "2nd Layer, 1 Edge (Right drop)", 292 },
        { "4th Layer, 1 Edge (Left drop)",  293 },
        { "4th Layer, 1 Edge (Left+Flip)",  294 },
        { "4th Layer, 1 Edge (Right drop)", 296 },
        { "4th Layer, 1 Edge (Right+Flip)", 295 },
        { "6th Layer, 1 Edge (Left drop)",  297 },
        { "6th Layer, 1 Edge (Right drop)", 298 },
    };
    for (int i = 1; i < MAX_HUMANLAYER_ALGORITHMS; i++) {
        la = humanlayer_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }

    //Sublevel1 Menu = Human Rotate Bulk-Solve using best layer routines (Solve.cpp)
    submenu1_id = glutCreateMenu(menuHandler);
    constexpr int MAX_LAYERSOLVE_ALGORITHMS = 11;
    constexpr StringAlgo layersolve_algorithms[MAX_LAYERSOLVE_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { "1st Layer: White Edges (Rotate)",   300 },
        { "1st Layer: White Corners (Rotate)", 301 },
        { "2nd Layer: All Edges (Rotate)",     302 },
        { "3rd Layer: All Corners (Rotate)",   303 },
        { "4th Layer: All Edges (Rotate)",     304 },
        { "5th Layer: All Corners (Rotate)",   305 },
        { "6th Layer: All Edges (Rotate)",     306 },
        { "7th Layer: Gray Edges (Rotate)",    307 },
        { "7th Layer: Gray Corners (Rotate)",  308 },
        { "ALL Layers: #1 - #7 (Auto Solve)",  309 }, // 309 = Solve All
    };
    for (int i = 1; i < MAX_LAYERSOLVE_ALGORITHMS; i++) {
        la = layersolve_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }

    //SubLevel3 Menu - Computer Auto-InstaSolve - Layer by Layer using Teleport
    //                 (internally "pops-out" pieces to move them, aka cheating)
    submenu3_id = glutCreateMenu(menuHandler);
    constexpr int MAX_COMPUSOLVE_ALGORITHMS = 10;
    constexpr StringAlgo compusolve_algorithms[MAX_COMPUSOLVE_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { "1st Layer: White Star (Teleport)",    40 },
        { "1st Layer: White Corners (Teleport)", 41 },
        { "2nd Layer: Edges (Teleport)",         42 },
        { "3rd Layer: Low Y's (Teleport)",       43 },
        { "4th Layer: Edges (Teleport)",         44 },
        { "5th Layer: High Y's (Teleport)",      45 },
        { "6th Layer: Edges (Teleport)",         46 },
        { "7th Layer: Grey Star (Teleport)",     47 },
        { "7th Layer: Grey Corners (Teleport)",  48 },
    };
    for (int i = 1; i < MAX_COMPUSOLVE_ALGORITHMS; i++) {
        la = compusolve_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }
    //these three computer "solves" are for teleporting/cheating
    glutAddMenuEntry("Compu.Solve Current Face",  21);
    glutAddMenuEntry("Compu.Solve Cur 5 Edges",   22);
    glutAddMenuEntry("Compu.Solve Cur 5 Corners", 23);

    //SubLevel5 Menu - Computer InstaSolve by Faces - Single
    //                 (return to solved position = "reset")
    submenu5_id = glutCreateMenu(menuHandler);
    constexpr int MAX_FACES_ALGORITHMS = 13;
    constexpr StringAlgo faces_algorithms[MAX_FACES_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { " 1 WHITE",       171 },
        { " 2 DARK_BLUE",   172 },
        { " 3 RED",         173 },
        { " 4 DARK_GREEN",  174 },
        { " 5 PURPLE",      175 },
        { " 6 YELLOW",      176 },
        { " 7 GRAY",        177 },
        { " 8 LIGHT_BLUE",  178 },
        { " 9 ORANGE",      179 },
        { "10 LIGHT_GREEN", 180 },
        { "11 PINK",        181 },
        { "12 BEIGE",       182 },
    };
    for (int i = 1; i < MAX_FACES_ALGORITHMS; i++) {
        la = faces_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }

    //SubLevel2 Menu - Current Face Method Manipulatations
    submenu2_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Rotate CounterClockwise <<", 19);
    glutAddMenuEntry("Rotate >>>>>> Clockwise >>", 20);
    //Color Flips
    constexpr int MAX_FLIP_COLOR_ALGORITHMS = 11;
    constexpr StringAlgo flipcolor_algorithms[MAX_FLIP_COLOR_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { "Flip Color: Edge [Shift+1]", 24 },
        { "Flip Color: Edge [Shift+2]", 25 },
        { "Flip Color: Edge [Shift+3]", 26 },
        { "Flip Color: Edge [Shift+4]", 27 },
        { "Flip Color: Edge [Shift+5]", 28 },
        { "Flip Color: Corner  [1]",    29 },
        { "Flip Color: Corner  [2]",    30 },
        { "Flip Color: Corner  [3]",    31 },
        { "Flip Color: Corner  [4]",    32 },
        { "Flip Color: Corner  [5]",    33 },
    };
    for (int i = 1; i < MAX_LAYERSOLVE_ALGORITHMS; i++) {
        la = layersolve_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }

    //Sublevel6 Menu - AutoSwap Piece w/ Teleport
    submenu6_id = glutCreateMenu(menuHandler);
    constexpr int MAX_SWAP_ALGORITHMS = 21;
    constexpr StringAlgo swap_algorithms[MAX_SWAP_ALGORITHMS] = {
        { "", 0},   // Initialize 0 with empty blank
        { "Swap Edges 1 & 2",   125 },
        { "Swap Edges 1 & 3",   126 }, //humans
        { "Swap Edges 1 & 4",   127 }, //will
        { "Swap Edges 1 & 5",   128 }, //enjoy
        { "Swap Edges 2 & 3",   129 }, //the
        { "Swap Edges 2 & 4",   130 }, //order
        { "Swap Edges 2 & 5",   131 },
        { "Swap Edges 3 & 4",   132 },
        { "Swap Edges 3 & 5",   133 },
        { "Swap Edges 4 & 5",   134 },
        { "Swap Corners 1 & 2", 135 },
        { "Swap Corners 1 & 3", 136 },
        { "Swap Corners 1 & 4", 137 },
        { "Swap Corners 1 & 5", 138 },
        { "Swap Corners 2 & 3", 139 },
        { "Swap Corners 2 & 4", 140 },
        { "Swap Corners 2 & 5", 141 },
        { "Swap Corners 3 & 4", 142 },
        { "Swap Corners 3 & 5", 143 },
        { "Swap Corners 4 & 5", 144 },
    };
    for (int i = 1; i < MAX_LAYERSOLVE_ALGORITHMS; i++) {
        la = layersolve_algorithms[i];
        glutAddMenuEntry(la.algo, la.num);
    }

    //Top Level = Main Menu + Attach SubMenus
    menu_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Toggle Spinning..", 1);
    glutAddSubMenu("Main Menu", submenu0_id);
    glutAddSubMenu("Algorithms ---->", submenu4_id);
    glutAddSubMenu("Place #Layer Edge", submenu7_id);
    glutAddSubMenu("Human.Solve Layer", submenu1_id);
    glutAddSubMenu("Comp.Solve Layer", submenu3_id);
    glutAddSubMenu("Comp.Solve Face", submenu5_id);
    glutAddSubMenu("Compu.Mod Face", submenu2_id);
    glutAddSubMenu("Compu.Mod Piece", submenu6_id);
    glutAddMenuEntry("Close Menu...", 9999);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//TODO: refactor all into one map of { "menu name", "menu handle #", function+args }
//      working on it.

//Right Click Menu event Handler, user interface back-end
void menuHandler(int num)
{
    auto face = megaminx->g_currentFace->getNum()+1;
    double sum = 0;
    switch (num) {
    case 1:
        toggleSpinning(); break;
    //menu submenu0_id: Main Menu
    case 91:
        megaminx->undo(); break;
    case 92:
        createMegaMinx(); break;
    case 93:
        resetCameraViewport(); break;
    case 94:
        megaminx->undoDouble(); break;
    case 95:
        megaminx->undoQuad(); break;
    case 96:
        megaminx->undoBulk(); break;
    case 100:
        megaminx->scramble(); break;
    case 101:
        readlineShell(); break;
    case 102:
        glutDestroyWindow(1);
        exit(0); break;
    case 98: //Save Cube State to File
        SaveCubetoFile();
        break;
    case 99: //Restore Cube State to File
        RestoreCubeFromFile();
        break;

    //menu submenu2_id: Compu.Mod Current Face
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

    //menu submenu6_id: (dispatch logic in menu)
    // Edge Piece Swaps (teleport)
	case 125 ... 128:
		megaminx->g_currentFace->swapEdges(0, 1+num-125); break;
    case 129 ... 131:
        megaminx->g_currentFace->swapEdges(1, 2+num-129); break;
    case 132 ... 133:
        megaminx->g_currentFace->swapEdges(2, 3+num-132); break;
    case 134:
        megaminx->g_currentFace->swapEdges(3, 4); break;
    // Corner Piece Swaps (teleport)
    case 135 ... 138:
        megaminx->g_currentFace->swapCorners(0, 1+num-135); break;
    case 139 ... 141:
        megaminx->g_currentFace->swapCorners(1, 2+num-139); break;
    case 142 ... 143:
        megaminx->g_currentFace->swapCorners(2, 3+num-142); break;
    case 144:
        megaminx->g_currentFace->swapCorners(3, 4); break;

    //menu submenu5_id:
	// Solve a Face (Reset) any selected face 1-12:
    case 171 ... 182:
        megaminx->resetFace(1+num-171); break;

    //menu SubLevel3: Teleport Reset by Layer
    case 40:  //1st Layer = WHITE Edges
        megaminx->resetFacesEdges(WHITE); break;
    case 41:  //1st Layer = WHITE Corners
        megaminx->resetFacesCorners(WHITE); break;
    case 42: //2nd Layer edges
        megaminx->resetSecondLayerEdges(); break;
    case 43: //3rd Layer corners
        megaminx->resetlowYmiddleWCorners(); break;
    case 44: //4th Layer edges
        megaminx->resetFourthLayerEdges(); break;
    case 45: //5th Layer corners
        megaminx->resethighYmiddleWCorners(); break;
    case 46: //6th Layer edges
        megaminx->resetSixthLayerEdges(); break;
    case 47:  //Last Layer GRAY Edges
        megaminx->resetFacesEdges(GRAY); break;
    case 48:  //Last Layer GRAY Corners
        megaminx->resetFacesCorners(GRAY); break;

    //menu submenu4_id: Algorithms by Number
    case 51 ... 88:     //1-38
        megaminx->rotateAlgo(num - 50); break;
    case 189 ... 202:   //39-52
        megaminx->rotateAlgo(num - 150); break;
    case 291 ... 299:   //1-8
        megaminx->rotateBulkAlgoString(g_AlgoStringsLayer[num - 290].algo,
                                   g_faceNeighbors[face], num - 290); break;

    //menu submenu1_id:  AutoSolve by Layer = solve.cpp
    case 300: //layer 1 edges rotate+autosolve F1
        MakeShadowCubeClone();
        megaminx->rotateSolveWhiteEdges(shadowDom);
        break;
    case 301: //layer 1 corners rotate+autosolve F1
        MakeShadowCubeClone();
        megaminx->rotateSolveWhiteCorners(shadowDom);
        break;
    case 302: //layer 2 edges rotate+autosolve F2
        MakeShadowCubeClone();
        megaminx->rotateSolveLayer2Edges(shadowDom);
        break;
    case 303: //layer 3 corners rotate+autosolve F3
        MakeShadowCubeClone();
        megaminx->rotateSolve3rdLayerCorners(shadowDom);
        break;
    case 304: //layer 4 edges rotate+autosolve F4
        MakeShadowCubeClone();
        megaminx->rotateSolveLayer4Edges(shadowDom);
        break;
    case 305: //layer 5 corners rotate+autosolve F5
        MakeShadowCubeClone();
        megaminx->rotateSolve5thLayerCorners(shadowDom);
        break;
    case 306: //layer 6 edges rotate+autosolve F6
        MakeShadowCubeClone();
        megaminx->rotateSolveLayer6Edges(shadowDom);
        break;
    case 307: //layer 7 edges rotate+autosolve F7
        MakeShadowCubeClone();
        megaminx->rotateSolveLayer7Edges(shadowDom);
        break;
    case 308: //layer 7 corners rotate+autosolve F8
        MakeShadowCubeClone();
        megaminx->rotateSolve7thLayerCorners(shadowDom);
        break;
    case 309: //layers 1-7 ALL at once rotate+autosolve F9
        MakeShadowCubeClone(); //init
        megaminx->rotateSolveWhiteEdges(shadowDom); //start
        megaminx->rotateSolveWhiteCorners(shadowDom);
        megaminx->rotateSolveLayer2Edges(shadowDom);
        megaminx->rotateSolve3rdLayerCorners(shadowDom);
        megaminx->rotateSolveLayer4Edges(shadowDom);
        megaminx->rotateSolve5thLayerCorners(shadowDom);
        megaminx->rotateSolveLayer6Edges(shadowDom);
        megaminx->rotateSolveLayer7Edges(shadowDom);
        megaminx->rotateSolve7thLayerCorners(shadowDom);
        std::cout << "AUTOSOLVER: Number of Rotations Queued up to Solve: " << megaminx->getRotateQueueNum() << std::endl;
        break;
    case 312:   //brute force checker for solver = F11
        std::cout << "Starting Brute Force Checker. Running 50000 Iterations" << std::endl;
        for (int i = 0; i < 50000; ++i) {   //loop
            SaveCubetoFile();   //save
            menuHandler(309);   //run one virtual solver
            //check if complete, count how many moves, and reset
            if (shadowDom->isFullySolved()) {
                sum += megaminx->getRotateQueueNum();
                megaminx->resetQueue(); //Cancel
                megaminx->scramble(); //scramble
            }
            else
            //produce a debug error immediately, and Save. then hit Abort to close, and Restore.
                assert(shadowDom->isFullySolved()); //check
        }
        //solver avg global is processed in main.cpp render func
        g_solveravg = sum / 50000.;
        std::cout << "Brute Force Checker Complete! - Solver Average: " << g_solveravg << std::endl;
        //Solver average was 588.627
        break;
    default:
        break;
    }
}
