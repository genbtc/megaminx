#include "../ui/opengl.h"
#include "../engine/megaminx.h"

extern  int window, menu_id, submenu0_id, submenu1_id, submenu2_id,
                            submenu3_id, submenu4_id, submenu5_id, submenu6_id;
extern Megaminx* megaminx;
extern Megaminx* shadowDom;
extern int currentFace;
void menuHandler(int num);
void menuVisible(int status, int x, int y);
void utPrintHelpMenu(float w, float h);
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id,
submenu3_id, submenu4_id, submenu5_id, submenu6_id;

void serializeVectorInt(std::vector<int> list1, std::string filename);
void WritePiecesFile(std::string filename, bool corner);
const std::vector<int> ReadPiecesFileVector(std::string filename);
void FromCubeToVectorFile();
void FromVectorFileToCube();
void FromCubeToShadowCube();
extern bool spinning;
extern void createMegaMinx();
extern void resetCameraView();


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


//Right Click Menu text/function mappings, actual user interface front-end
void createMenu()
{
    //SubLevel 0 menu - Main Menu
    submenu0_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Camera Home Pos.", 93);
    glutAddMenuEntry("Edit.Undo", 91);
    glutAddMenuEntry("Edit.Undo [2x]", 94);
    glutAddMenuEntry("Edit.Undo [4x]", 95);
    glutAddMenuEntry("Edit.Undo [Bulk]", 96);
    glutAddMenuEntry("Scramble! [Del]", 100);
    glutAddMenuEntry("Save Cube...", 98);
    glutAddMenuEntry("Restore Cube...", 99);
    glutAddMenuEntry("Exit!", 102);

    //SubLevel2 Menu - Current Face
    submenu2_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Rotate CCW <<", 19);
    glutAddMenuEntry("Rotate  CW >>", 20);
    glutAddMenuEntry("Place/Solve Entire Face", 21);
    glutAddMenuEntry("Place/Solve Five Edges", 22);
    glutAddMenuEntry("Place/Solve Five Corners", 23);
    glutAddMenuEntry("Flip Colors Edge [1+Shift]", 24);
    glutAddMenuEntry("Flip Colors Edge [2+Shift]", 25);
    glutAddMenuEntry("Flip Colors Edge [3+Shift]", 26);
    glutAddMenuEntry("Flip Colors Edge [4+Shift]", 27);
    glutAddMenuEntry("Flip Colors Edge [5+Shift]", 28);
    glutAddMenuEntry("Flip Colors Corner [1]", 29);
    glutAddMenuEntry("Flip Colors Corner [2]", 30);
    glutAddMenuEntry("Flip Colors Corner [3]", 31);
    glutAddMenuEntry("Flip Colors Corner [4]", 32);
    glutAddMenuEntry("Flip Colors Corner [5]", 33);

    //Sublevel6 Menu - AutoSwap Piece
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

    //SubLevel3 Menu - Auto Solve Steps (internal fast solve)
    submenu3_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("*New/Reset/SolveAll*", 92);
    glutAddMenuEntry("1st Layer White Star", 41);
    glutAddMenuEntry("1st Layer White Corners", 42);
    glutAddMenuEntry("2nd Layer Edges", 151);
    glutAddMenuEntry("3rd Layer Low Y's", 152);
    glutAddMenuEntry("4th Layer Edges", 153);
    glutAddMenuEntry("5th Layer High Y's", 154);
    glutAddMenuEntry("6th Layer Edges", 155);
    glutAddMenuEntry("7th Layer Grey Star", 156);
    glutAddMenuEntry("7th Layer Grey Corners", 157);

    //SubLevel4 Menu - Human Rotate Algos
    submenu4_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("r u R' U'", 51);
    glutAddMenuEntry("l u L' U'", 52);
    glutAddMenuEntry("U' L' u l", 53);
    glutAddMenuEntry("u r U' R'", 150);
    glutAddMenuEntry("R' D' R D", 57);
    glutAddMenuEntry("R' D' R D [x2]", 258);
    glutAddMenuEntry("R' D' R D [x4]", 257);
    glutAddMenuEntry("LL Corner: u l U' R' u L' U' r", 55);
    glutAddMenuEntry("LL Corner: u r 2U' L' 2u R' 2U' l u", 56);
    glutAddMenuEntry("LL Edge+Corn, Front=Safe", 60);
    glutAddMenuEntry("LL Edge+Corn, Back/Left=Safe", 62);
    glutAddMenuEntry("LL Edge: r u R' u r 2U' R'", 54);
    glutAddMenuEntry("LL Edge: r u2, R' u, r u2, R'", 259);
    glutAddMenuEntry("LL Edge 1+ [x5]", 58);
    glutAddMenuEntry("LL Edge 2- [x5]", 59);
    glutAddMenuEntry("LL Edge 3a+, Front/Left=Safe", 61);
    glutAddMenuEntry("LL Edge 3b-, Both+Backs=Safe", 251);
    glutAddMenuEntry("LL Edge 3c-, Right/Back=Safe", 252);
    glutAddMenuEntry("LL Edge 5-way scramble (#203)", 253);
    glutAddMenuEntry("LL Edge 5-way scramble (#204)", 254);
    glutAddMenuEntry("LL Edge 5-way two swaps (#205)", 255);
    glutAddMenuEntry("LL Edge 5-way swap/INVERT (#206)", 256);
    glutAddMenuEntry("2nd Layer Edge (Left Algo)", 63);
    glutAddMenuEntry("2nd Layer Edge (Right Algo) ", 64);
    glutAddMenuEntry("4th Layer Edge (Left Algo)", 65);
    glutAddMenuEntry("4th Layer Edge (Right Algo)", 66);
    glutAddMenuEntry("6th Layer Edge (Left Algo)", 67);
    glutAddMenuEntry("6th Layer Edge (Right Algo)", 68);
    //Sublevel X = Human Rotate Bulk-Solve whole layer routines
    glutAddMenuEntry("1st Layer White Edges", 302);
    glutAddMenuEntry("1st Layer White Corners", 303);
    glutAddMenuEntry("2nd Layer Edges", 304);
    glutAddMenuEntry("3rd Layer Corners", 305);
    glutAddMenuEntry("4th Layer Edges", 306);
    glutAddMenuEntry("5th Layer Corners", 307);
    glutAddMenuEntry("6th Layer Edges", 308);

    //SubLevel5 Menu - Solve Current Faces (Reset to solved position)
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
    glutAddSubMenu("Algorithms --->", submenu4_id);
    glutAddSubMenu("AutoSolve Layer", submenu3_id);
    glutAddSubMenu("AutoSolve Face ", submenu5_id);
    glutAddSubMenu("AutoSwap Piece ", submenu6_id);
    glutAddSubMenu("Modify Face    ", submenu2_id);
    glutAddMenuEntry("Close Menu...", 9999);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//Right Click Menu event Handler, user interface back-end
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
    case 259:
    case 258:
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
    case 96:
        megaminx->undoBulk(); break;
    case 100:
        megaminx->scramble(); break;
    case 102:
        glutDestroyWindow(1);
        exit(0); break;
    case 125: //Edge Piece Swaps
        megaminx->g_currentFace->swapEdges(0, 1); break;
    case 126:
        megaminx->g_currentFace->swapEdges(0, 2); break;
    case 127:
        megaminx->g_currentFace->swapEdges(0, 3); break;
    case 128:
        megaminx->g_currentFace->swapEdges(0, 4); break;
    case 129:
        megaminx->g_currentFace->swapEdges(1, 2); break;
    case 130:
        megaminx->g_currentFace->swapEdges(1, 3); break;
    case 131:
        megaminx->g_currentFace->swapEdges(1, 4); break;
    case 132:
        megaminx->g_currentFace->swapEdges(2, 3); break;
    case 133:
        megaminx->g_currentFace->swapEdges(2, 4); break;
    case 134:
        megaminx->g_currentFace->swapEdges(3, 4); break;
    case 135: //Corner Piece Swaps
        megaminx->g_currentFace->swapCorners(0, 1); break;
    case 136:
        megaminx->g_currentFace->swapCorners(0, 2); break;
    case 137:
        megaminx->g_currentFace->swapCorners(0, 3); break;
    case 138:
        megaminx->g_currentFace->swapCorners(0, 4); break;
    case 139:
        megaminx->g_currentFace->swapCorners(1, 2); break;
    case 140:
        megaminx->g_currentFace->swapCorners(1, 3); break;
    case 141:
        megaminx->g_currentFace->swapCorners(1, 4); break;
    case 142:
        megaminx->g_currentFace->swapCorners(2, 3); break;
    case 143:
        megaminx->g_currentFace->swapCorners(2, 4); break;
    case 144:
        megaminx->g_currentFace->swapCorners(3, 4); break;
    case 98: //Save Game State
        FromCubeToVectorFile();
        break;
    case 99: //Restore Game State
        FromVectorFileToCube();
        break;
    case 301: //instanciate shadow cube - GLUT_KEY_INSERT
        FromCubeToShadowCube();
        break;
    case 302: //layer 1 edges rotate+autosolve F1
        megaminx->rotateSolveWhiteEdges(shadowDom);
        break;
    case 303: //layer 1 corners rotate+autosolve F2
        megaminx->rotateSolveWhiteCorners(shadowDom);
        break;
    case 304: //layer 2 edges rotate+autosolve F3
        megaminx->rotateSolveLayer2Edges(shadowDom);
        break;
    case 305: //layer 3 corners rotate+autosolve F4
        megaminx->rotateSolve3rdLayerCorners(shadowDom);
        break;
    case 306: //layer 4 edges rotate+autosolve F5
        megaminx->rotateSolveLayer4Edges(shadowDom);
        break;
    case 307: //layer 5 corners rotate+autosolve F6
        megaminx->rotateSolve5thLayerCorners(shadowDom);
        break;
    case 308: //layer 6 edges rotate+autosolve F7
        megaminx->rotateSolveLayer6Edges(shadowDom);
        break;
    case 309: //layers 1-6 all rotate+autosolve F8
        FromCubeToShadowCube();
        megaminx->rotateSolveWhiteEdges(shadowDom);
        megaminx->rotateSolveWhiteCorners(shadowDom);
        megaminx->rotateSolveLayer2Edges(shadowDom);
        megaminx->rotateSolve3rdLayerCorners(shadowDom);
        megaminx->rotateSolveLayer4Edges(shadowDom);
        megaminx->rotateSolve5thLayerCorners(shadowDom);
        megaminx->rotateSolveLayer6Edges(shadowDom);
        megaminx->rotateSolveLayer7Edges(shadowDom);
        //megaminx->testingAlgostrings(shadowDom);
        break;
    default:
        break;
    }
}