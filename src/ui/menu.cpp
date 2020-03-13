#include "../ui/opengl.h"
#include "../engine/megaminx.h"

extern Megaminx* megaminx;
extern Megaminx* shadowDom;
extern int currentFace;
extern bool spinning;
extern double solveravg;
void createMegaMinx();
void resetCameraViewport();
static int window, menu_id, submenu0_id, submenu1_id, submenu2_id,
submenu3_id, submenu4_id, submenu5_id, submenu6_id;
void menuHandler(int num);
void serializeVectorInt60(std::vector<int> list1, std::string filename);
const std::vector<int> ReadPiecesFileVector(std::string filename);
void SaveCubetoFile();
void RestoreCubeFromFile();
void MakeShadowCubeClone();

//Help menu with Glut commands and line by line iteration built in.
void utPrintHelpMenu(float w, float h)
{
    constexpr char helpStrings[18][32] = { "[H]elp Menu:",
                                           "[Right Click]  Actions Menu",
                                           "[Dbl Click]  Rotate Current CW>",
//                                           "[F1-F12]     Rotate Face #  >>",
                                           "  +Shift  CounterClockwise <<",
                                           "1,2,3,4,5 Flip Curr. Corner #",
                                           "!,@,#,$,% Flip Curr. Edge  #",
                                           "[W/w]  Rotate Upper Face </>",
                                           "[S/s]  Rotate Front Face </>",
                                           "[A/a]  Rotate Left  Face </>",
                                           "[D/d]  Rotate Right Face </>",
                                           "[Z/z]  Rotate DownLeft   </>",
                                           "[C/c]  Rotate DownRight  </>",
                                           "[X/x]  Rotate Bottom Face </>",
                                           "[Space]  Toggle Auto-Spinning",
                                           "[BackSpace]  Reset Camera Pos",
                                           "[Delete]  Scramble Puzzle",
                                           "[Enter] Solve Current Face",
                                           "[F1-F8=F9] Layer# Auto Solve",
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
    glutAddMenuEntry("Edit.Undo [^Z]", 91);
    glutAddMenuEntry("Edit.Undo*2", 94);
    glutAddMenuEntry("Edit.Undo*4", 95);
    glutAddMenuEntry("Edit.Undo*Bulk", 96);
    glutAddMenuEntry("Scramble! [Del]", 100);
    glutAddMenuEntry("New Cube...", 92);
    glutAddMenuEntry("Save Cube...", 98);
    glutAddMenuEntry("Restore Cube...", 99);
    glutAddMenuEntry("Exit!", 102);

    //SubLevel4 Menu - Human Bulk Rotate Algos
    submenu4_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("r u R' U'", 51);
    glutAddMenuEntry("l u L' U'", 52);
    glutAddMenuEntry("U' L' u l", 53);
    glutAddMenuEntry("u r U' R'", 54);
    //
    glutAddMenuEntry("7LL-Edge3a- CCW MUSHROOM- Fr./L.=Safe", 64);
    glutAddMenuEntry("7LL-Edge3e+  CW MUSHROOM+ Fr./L.=Safe", 83);
//I consider these 3 below redundant to the 2 above:
//  glutAddMenuEntry("7LL-Edge3d- CCW MUSHROOM- Both+Backs=Safe", 82);
//  glutAddMenuEntry("7LL-Edge3b+  CW MUSHROOM+ Both+Backs=Safe", 65);
//  glutAddMenuEntry("7LL-Edge3c+  CW MUSHROOM+ Right/Back=Safe", 66);
    glutAddMenuEntry("7LL-Edge1-  CCW x5 HORSE- Fr./R.Back=Safe", 62);
    glutAddMenuEntry("7LL-Edge2+   CW x5 HORSE+ Fr./R.Back=Safe", 63);
    glutAddMenuEntry("7LL-Edge20 2+2swap BUNNY Opp. 2&4,3&5 + INVERT 8'/1'", 70);
    glutAddMenuEntry("7LL-Edge37 2+2swap BUNNY BEST 2&5,3&4", 87);
    glutAddMenuEntry("7LL-Edge19 2+2swap BUNNY Adj. 2&3,4&5", 69);
    glutAddMenuEntry("7LL-Edge29 2+2swap BUNNY Colors ONLY 2,3,4,5", 79);    
    glutAddMenuEntry("7LL-Edge38 5-way CCW cycle by +2 all", 88);
    glutAddMenuEntry("7LL-Edge17 5-way  CW cycle by -2 all", 67);
    //glutAddMenuEntry("7LL-Edge18 5-way CCW cycle by 1,2,-1,2,1", 68);
    glutAddMenuEntry("7LL-Edge39 5-way CCW cycle by 1,2,-1,2,1", 189);
    glutAddMenuEntry("7LL-Edge43 5-way  CW cycle by -1,-2,1,-2,-1", 193);
    //
    glutAddMenuEntry("7LL-E+C #1/3 CCW HORSEdge- Fr./R.Back=Safe", 61);
    glutAddMenuEntry("7LL-E+C #2 CCW MUSHEdge- Fr./L.=Safe", 60);
    glutAddMenuEntry("7LL-E+C #3  CW MUSHEdge+ Fr.Line/L.Edge=Safe", 58);
    glutAddMenuEntry("7LL-E+C 2+2swap BUNNY 1&3,2&4 R.Back=Safe", 59);
    glutAddMenuEntry("7LL-E+C 2+2swap BUNNY 2&5,3&4 Front=Safe", 86);
    glutAddMenuEntry("7LL-E+C x1 CCW HORSEdge- Fr./R.Back=Safe", 81);
    glutAddMenuEntry("7LL-E+C x1  CW HORSEdge+ Fr./R.Back=Safe", 80);
    //
    glutAddMenuEntry("7LL Corner: R' D' r dr (Permute) C2+Safe", 55);
    //  glutAddMenuEntry("R' D' r dr [x2]", 55);
    //  glutAddMenuEntry("R' D' r dr [x4]", 55);
    glutAddMenuEntry("7LL Corners: Cycle- CCW FrontLine=Safe", 57);
    glutAddMenuEntry("7LL Corners: Cycle+  CW RightLine=Safe", 77);
//I consider these 2 below redundant to the 2 above:
//  glutAddMenuEntry("LL Corners: Cycle- CCW L.Back=Safe", 56);
//  glutAddMenuEntry("LL Corners: Cycle- CCW Left=Safe", 78);

    //Sublevel Y = Human Manual Rotate Routines (insert one piece manually into layer)
    glutAddMenuEntry("2nd Layer, Place Edge (Left)", 71);
    glutAddMenuEntry("2nd Layer, Place Edge (Right)",72);
    glutAddMenuEntry("4th Layer, Place Edge (Left)", 73);
    glutAddMenuEntry("4th Layer, Place Edge (Left+Flip)", 84);
    glutAddMenuEntry("4th Layer, Place Edge (Right)", 74);
    glutAddMenuEntry("4th Layer, Place Edge (Right+Flip)",85);
    glutAddMenuEntry("6th Layer, Place Edge (Left)", 75);
    glutAddMenuEntry("6th Layer, Place Edge (Right)",76);

    //Sublevel Z = Human Rotate Bulk-Solve by whole layer routines with Solve.cpp
    submenu1_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("1st Layer: White Edges (Rotate)", 300);
    glutAddMenuEntry("1st Layer: White Corners (Rotate)", 301);
    glutAddMenuEntry("2nd Layer: All Edges (Rotate)", 302);
    glutAddMenuEntry("3rd Layer: All Corners (Rotate)", 303);
    glutAddMenuEntry("4th Layer: All Edges (Rotate)", 304);
    glutAddMenuEntry("5th Layer: All Corners (Rotate)", 305);
    glutAddMenuEntry("6th Layer: All Edges (Rotate)", 306);
    glutAddMenuEntry("7th Layer: Gray Edges (Rotate)", 307);
    glutAddMenuEntry("7th Layer: Gray Corners (Rotate)", 308);
    glutAddMenuEntry("ALL Layers: #1 - #7 (Rotate)", 309);

    //SubLevel3 Menu - Computer InstaSolve by Layer (internally "pops-out"=aka cheating)
    submenu3_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("1st Layer: White Star (Teleport)", 40);
    glutAddMenuEntry("1st Layer: White Corners (Teleport)", 41);
    glutAddMenuEntry("2nd Layer: Edges (Teleport)", 42);
    glutAddMenuEntry("3rd Layer: Low Y's (Teleport)", 43);
    glutAddMenuEntry("4th Layer: Edges (Teleport)", 44);
    glutAddMenuEntry("5th Layer: High Y's (Teleport)", 45);
    glutAddMenuEntry("6th Layer: Edges (Teleport)", 46);
    glutAddMenuEntry("7th Layer: Grey Star (Teleport)", 47);
    glutAddMenuEntry("7th Layer: Grey Corners (Teleport)", 48);

    //SubLevel5 Menu - Computer InstaSolve by Faces (Reset to solved position)
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

    //SubLevel2 Menu - Current Face Modifications
    submenu2_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Rotate CounterClockwise <<", 19);
    glutAddMenuEntry("Rotate >>>>>> Clockwise >>", 20);
    glutAddMenuEntry("Insta-Solve Entire Face", 21);
    glutAddMenuEntry("Insta-Solve Five Edges", 22);
    glutAddMenuEntry("Insta-Solve Five Corners", 23);
    glutAddMenuEntry("Flip Color: Edge [Shift+1]", 24);
    glutAddMenuEntry("Flip Color: Edge [Shift+2]", 25);
    glutAddMenuEntry("Flip Color: Edge [Shift+3]", 26);
    glutAddMenuEntry("Flip Color: Edge [Shift+4]", 27);
    glutAddMenuEntry("Flip Color: Edge [Shift+5]", 28);
    glutAddMenuEntry("Flip Color: Corner  [1]", 29);
    glutAddMenuEntry("Flip Color: Corner  [2]", 30);
    glutAddMenuEntry("Flip Color: Corner  [3]", 31);
    glutAddMenuEntry("Flip Color: Corner  [4]", 32);
    glutAddMenuEntry("Flip Color: Corner  [5]", 33);

    //Sublevel6 Menu - AutoSwap Piece
    submenu6_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Swap Edges 1 & 2", 125);
    glutAddMenuEntry("Swap Edges 2 & 3", 129);
    glutAddMenuEntry("Swap Edges 3 & 4", 132);
    glutAddMenuEntry("Swap Edges 4 & 5", 134);
    glutAddMenuEntry("Swap Edges 1 & 3", 126);
    glutAddMenuEntry("Swap Edges 1 & 4", 127);
    glutAddMenuEntry("Swap Edges 1 & 5", 128);    
    glutAddMenuEntry("Swap Edges 2 & 4", 130);
    glutAddMenuEntry("Swap Edges 2 & 5", 131);    
    glutAddMenuEntry("Swap Edges 3 & 5", 133);    
    glutAddMenuEntry("Swap Corners 1 & 2", 135);
    glutAddMenuEntry("Swap Corners 2 & 3", 139);
    glutAddMenuEntry("Swap Corners 3 & 4", 142);
    glutAddMenuEntry("Swap Corners 4 & 5", 144);
    glutAddMenuEntry("Swap Corners 1 & 3", 136);
    glutAddMenuEntry("Swap Corners 1 & 4", 137);
    glutAddMenuEntry("Swap Corners 1 & 5", 138);    
    glutAddMenuEntry("Swap Corners 2 & 4", 140);
    glutAddMenuEntry("Swap Corners 2 & 5", 141);    
    glutAddMenuEntry("Swap Corners 3 & 5", 143);    

    //Top Level - Main Menu
    menu_id = glutCreateMenu(menuHandler);
    glutAddMenuEntry("Toggle Spinning..", 1);
    glutAddSubMenu("Main Menu", submenu0_id);
    glutAddSubMenu("Algorithms ---->", submenu4_id);
    glutAddSubMenu("Human.Solve Bulk", submenu1_id);
    glutAddSubMenu("Comp.Solve Layer", submenu3_id);
    glutAddSubMenu("Comp.Solve Face", submenu5_id);
    glutAddSubMenu("Compu.Mod Face", submenu2_id);
    glutAddSubMenu("Compu.Mod Piece", submenu6_id);
    glutAddMenuEntry("Close Menu...", 9999);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//TODO: refactor all into one map of { "menu name", "menu handle #", function+args }

//Right Click Menu event Handler, user interface back-end
void menuHandler(int num)
{
    double sum = 0;
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
    case 42: //2nd Layer edges
        megaminx->secondLayerEdges(); break;
    case 43: //3rd Layer corners
        megaminx->lowYmiddleW(); break;
    case 44: //4th Layer edges
        megaminx->fourthLayerEdges(); break;
    case 45: //5th Layer corners
        megaminx->highYmiddleW(); break;
    case 46: //6th Layer edges
        megaminx->sixthLayerEdges(); break;
    case 47:  //Last Layer GRAY Edges
        megaminx->resetFacesEdges(GRAY); break;
    case 48:  //Last Layer GRAY Corners
        megaminx->resetFacesCorners(GRAY); break;
    //case 49:
    //case 50:
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
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
        megaminx->rotateAlgo(currentFace, num - 50); break;
    case 189:
    case 190:
    case 191:
    case 192:
    case 193:
        megaminx->rotateAlgo(currentFace, num - 150); break;
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
    case 98: //Save Cube State to File
        SaveCubetoFile();
        break;
    case 99: //Restore Cube State to File
        RestoreCubeFromFile();
        break;
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
    case 307: //layer 7 edges F7
        MakeShadowCubeClone();
        megaminx->rotateSolveLayer7Edges(shadowDom);
        break;
    case 308: //layer 7 corners F8
        MakeShadowCubeClone();
        megaminx->rotateSolve7thLayerCorners(shadowDom);
        break;
    case 309:    //layers 1-7 all rotate+autosolve F9 = solve.cpp
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
        break;
    case 312:   //brute force checker for solver = F11
        for (int i = 0; i < 50000; ++i) {
            SaveCubetoFile(); //save
            menuHandler(309);   //solver
            //produce a debug error immediately, and Save. then hit Abort to close, and Restore.
            assert(shadowDom->isFullySolved()); //check
            if (shadowDom->isFullySolved()) {
                sum += megaminx->getRotateQueueNum();
                megaminx->resetQueue(); //Cancel
                megaminx->scramble(); //scramble
            }
            else
                continue;
        }
        solveravg = sum / 50000.;
        break;
    default:
        break;
    }
}
