#include "../ui/opengl.h"
#include "../engine/megaminx.h"

extern Megaminx* megaminx;
extern Megaminx* shadowDom;
extern int currentFace;
extern bool spinning;
void createMegaMinx();
void resetCameraView();
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id,
submenu3_id, submenu4_id, submenu5_id, submenu6_id;
void menuHandler(int num);
void serializeVectorInt60(std::vector<int> list1, std::string filename);
const std::vector<int> ReadPiecesFileVector(std::string filename);
void FromCubeToVectorFile();
void FromVectorFileToCube();
void FromCubeToShadowCube();

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
    glutAddMenuEntry("1st Layer White Star", 40);
    glutAddMenuEntry("1st Layer White Corners", 41);
    glutAddMenuEntry("2nd Layer Edges", 42);
    glutAddMenuEntry("3rd Layer Low Y's", 43);
    glutAddMenuEntry("4th Layer Edges", 44);
    glutAddMenuEntry("5th Layer High Y's", 45);
    glutAddMenuEntry("6th Layer Edges", 46);
    glutAddMenuEntry("7th Layer Grey Star", 47);
    glutAddMenuEntry("7th Layer Grey Corners", 48);

    //SubLevel4 Menu - Human Rotate Algos
    submenu4_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("r u R' U'", 51);
    glutAddMenuEntry("l u L' U'", 52);
    glutAddMenuEntry("U' L' u l", 53);
    glutAddMenuEntry("u r U' R'", 54);
    glutAddMenuEntry("R' D' R D", 55);
//  glutAddMenuEntry("R' D' R D [x2]", 55);
//  glutAddMenuEntry("R' D' R D [x4]", 55);
//  glutAddMenuEntry("LL Corners: Cycle- CCW L.Back=Safe", 56);
//  glutAddMenuEntry("LL Corners: Cycle- CCW Left=Safe", 78);
    glutAddMenuEntry("LL Corners: Cycle- CCW Front=Safe", 57);
    glutAddMenuEntry("LL Corners: Cycle+  CW Right=Safe", 77);    
    glutAddMenuEntry("LL Edge+Corn 3,  CW Front/FL.=Safe", 58);
    glutAddMenuEntry("LL Edge+Corn BUNNY 1&3,2&4 R.Back=Safe", 59);
    glutAddMenuEntry("LL Edge+Corn 2, CCW Fr./L.=Safe", 60);
    glutAddMenuEntry("LL Edge+Corn 1&3, CCW Fr./R.Back=Safe", 61);
    glutAddMenuEntry("LL Edge+C 2- CCW x1 Fr./R.Back=Safe", 81);
    glutAddMenuEntry("LL Edge+C 1+  CW x1 Fr./R.Back=Safe", 80);
    glutAddMenuEntry("LL Edge 1 - CCW [x5] Fr./R.Back=Safe", 62);
    glutAddMenuEntry("LL Edge 2 +  CW [x5] Fr./R.Back=Safe", 63);
    glutAddMenuEntry("LL Edge 3a- CCW Front/Left=Safe", 64);
    glutAddMenuEntry("LL Edge 3e+  CW Front/Left=Safe", 83);
    glutAddMenuEntry("LL Edge 3d- CCW Both+Backs=Safe", 82);
    glutAddMenuEntry("LL Edge 3b+  CW Both+Backs=Safe", 65);
    glutAddMenuEntry("LL Edge 3c+  CW Right/Back=Safe", 66);
    glutAddMenuEntry("LL Edge 2+2swap Adj.Swaps  2&3,4&5", 69);
    glutAddMenuEntry("LL Edge 2+2swap Opp.Flips  2&4,3&5", 70);
    glutAddMenuEntry("LL Edge 2+2swap Color.Only 2,3,4,5", 79);
    glutAddMenuEntry("LL Edge 5-way CCW cycle by 1,2,-1,2,1", 68);
    glutAddMenuEntry("LL Edge 5-way  CW cycle by -2 all", 67);
    glutAddMenuEntry("LL Edge 2+2swap BUNNY Permute Colors #36", 86);
    glutAddMenuEntry("LL Edge 2+2swap BUNNY (keep colors) #37", 87);
    glutAddMenuEntry("2nd Layer, Place Edge (Left)", 71);
    glutAddMenuEntry("2nd Layer, Place Edge (Right)",72);
    glutAddMenuEntry("4th Layer, Place Edge (Left)", 73);
    glutAddMenuEntry("4th Layer, Place Edge (Left+Flip)", 84);
    glutAddMenuEntry("4th Layer, Place Edge (Right)", 74);
    glutAddMenuEntry("4th Layer, Place Edge (Right+Flip)",85);
    glutAddMenuEntry("6th Layer, Place Edge (Left)", 75);
    glutAddMenuEntry("6th Layer, Place Edge (Right)",76);
    //Sublevel X = Human Rotate Bulk-Solve whole layer routines
    glutAddMenuEntry("1st Layer: White Edges (Auto)", 300);
    glutAddMenuEntry("1st Layer: White Corners (Auto)", 301);
    glutAddMenuEntry("2nd Layer: All Edges (Auto)", 302);
    glutAddMenuEntry("3rd Layer: All Corners (Auto)", 303);
    glutAddMenuEntry("4th Layer: All Edges (Auto)", 304);
    glutAddMenuEntry("5th Layer: All Corners (Auto)", 305);
    glutAddMenuEntry("6th Layer: All Edges (Auto)", 306);
    glutAddMenuEntry("7th Layer: Gray Edges (Auto)", 307);
    glutAddMenuEntry("7th Layer: Gray Corners (Auto)", 308);

    //SubLevel5 Menu - Solve Current Faces (Reset to solved position)
    submenu5_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry(" 1 WHITE", 171);
    glutAddMenuEntry(" 2 DARK_BLUE", 172);
    glutAddMenuEntry(" 3 RED", 173);
    glutAddMenuEntry(" 4 DARK_GREEN", 174);
    glutAddMenuEntry(" 5 PURPLE", 175);
    glutAddMenuEntry(" 6 YELLOW", 176);
    glutAddMenuEntry(" 7 GRAY", 177);
    glutAddMenuEntry(" 8 LIGHT_BLUE", 178);
    glutAddMenuEntry(" 9 ORANGE", 179);
    glutAddMenuEntry("10 LIGHT_GREEN", 180);
    glutAddMenuEntry("11 PINK", 181);
    glutAddMenuEntry("12 BEIGE", 182);

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
//TODO: refactor all into one map of { "menu name", "menu handle #", function+args }

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
    case 40:  //1st Layer = WHITE Edges
        megaminx->resetFacesEdges(WHITE); break;
    case 41:  //1st Layer = WHITE Corners
        megaminx->resetFacesCorners(WHITE); break;
    case 42: //2nd Layer
        megaminx->secondLayerEdges(); break;
    case 43: //3rd Layer
        megaminx->lowYmiddleW(); break;
    case 44: //4th Layer
        megaminx->fourthLayerEdges(); break;
    case 45: //5th Layer
        megaminx->highYmiddleW(); break;
    case 46: //6th Layer
        megaminx->sixthLayerEdges(); break;
    case 47:  //Last Layer GRAY Edges
        megaminx->resetFacesEdges(GRAY); break;
    case 48:  //Last Layer GRAY Corners
        megaminx->resetFacesCorners(GRAY); break;
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
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
    case 83:
    //case 84:
    //case 85:
    case 86:
    case 87:
    //case 88:
    //case 89:
        megaminx->rotateAlgo(currentFace, num - 50); break;
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
        //Solve Faces (Reset) 1-12:
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
    case 176:
    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
        megaminx->resetFace(num - 170); break;
    case 98: //Save Game State
        FromCubeToVectorFile();
        break;
    case 99: //Restore Game State
        FromVectorFileToCube();
        break;
    case 300: //layer 1 edges rotate+autosolve F1
        FromCubeToShadowCube();
        megaminx->rotateSolveWhiteEdges(shadowDom);
        break;
    case 301: //layer 1 corners rotate+autosolve F2
        FromCubeToShadowCube();
        megaminx->rotateSolveWhiteCorners(shadowDom);
        break;
    case 302: //layer 2 edges rotate+autosolve F3
        FromCubeToShadowCube();
        megaminx->rotateSolveLayer2Edges(shadowDom);
        break;
    case 303: //layer 3 corners rotate+autosolve F4
        FromCubeToShadowCube();
        megaminx->rotateSolve3rdLayerCorners(shadowDom);
        break;
    case 304: //layer 4 edges rotate+autosolve F5
        FromCubeToShadowCube();
        megaminx->rotateSolveLayer4Edges(shadowDom);
        break;
    case 305: //layer 5 corners rotate+autosolve F6
        FromCubeToShadowCube();
        megaminx->rotateSolve5thLayerCorners(shadowDom);
        break;
    case 306: //layer 6 edges rotate+autosolve F7
        FromCubeToShadowCube();
        megaminx->rotateSolveLayer6Edges(shadowDom);
        break;
    case 307: //layer 7 edges
        FromCubeToShadowCube();
        megaminx->rotateSolveLayer7Edges(shadowDom);
        break;
    case 308: //layer 7 corners
        FromCubeToShadowCube();
        megaminx->rotateSolve7thLayerCorners(shadowDom);
        break;
    case 309: //layers 1-7 all rotate+autosolve F8
        FromCubeToShadowCube();
        megaminx->rotateSolveWhiteEdges(shadowDom);
        megaminx->rotateSolveWhiteCorners(shadowDom);
        megaminx->rotateSolveLayer2Edges(shadowDom);
        megaminx->rotateSolve3rdLayerCorners(shadowDom);
        megaminx->rotateSolveLayer4Edges(shadowDom);
        megaminx->rotateSolve5thLayerCorners(shadowDom);
        megaminx->rotateSolveLayer6Edges(shadowDom);
        megaminx->rotateSolveLayer7Edges(shadowDom);
        megaminx->rotateSolve7thLayerCorners(shadowDom);
        break;
    default:
        break;
    }
}
