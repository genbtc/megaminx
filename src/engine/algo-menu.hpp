#ifndef __ALGOMENU_HPP__
#define __ALGOMENU_HPP__

struct StringAlgo {
    const char* algo;
    int num;
};

//SubLevel5 Menu - Basic Computer InstaSolve by Faces - Single Face
//submenu5_id = glutCreateMenu(menuHandler);    //(return to solved position = "reset")
constexpr int MAX_FACES_ALGORITHMS = 13;
constexpr StringAlgo faces_algorithms[MAX_FACES_ALGORITHMS] = {
    { "", 0},   // Initialize 0 with empty blank
    { " 1 WHITE",       171 },  //resetFace(1+num-171)
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

//SubLevel2 Menu - Current Face Method Manipulatations
//submenu2_id = glutCreateMenu(menuHandler);    //Color Flips
constexpr int MAX_FLIP_COLOR_ALGORITHMS = 11;
constexpr StringAlgo flipcolor_algorithms[MAX_FLIP_COLOR_ALGORITHMS] = {
    { "", 0},   // Initialize 0 with empty blank
    { "Flip Color: Edge [Shift+1]", 24 },   //flipEdgeColor(currentFace, num - 23)
    { "Flip Color: Edge [Shift+2]", 25 },
    { "Flip Color: Edge [Shift+3]", 26 },
    { "Flip Color: Edge [Shift+4]", 27 },
    { "Flip Color: Edge [Shift+5]", 28 },
    { "Flip Color: Corner  [1]",    29 },   //flipCornerColor(currentFace, num - 28)
    { "Flip Color: Corner  [2]",    30 },
    { "Flip Color: Corner  [3]",    31 },
    { "Flip Color: Corner  [4]",    32 },
    { "Flip Color: Corner  [5]",    33 },
};

//Sublevel6 Menu - AutoSwap Piece w/ Teleport
//submenu6_id = glutCreateMenu(menuHandler);
constexpr int MAX_SWAP_ALGORITHMS = 21;
constexpr StringAlgo swap_algorithms[MAX_SWAP_ALGORITHMS] = {
    { "", 0},   // Initialize 0 with empty blank
    { "Swap Edges 1 & 2",   125 },  // g_currentFace->swapEdges(0, 1+num-125)
    { "Swap Edges 1 & 3",   126 }, //humans
    { "Swap Edges 1 & 4",   127 }, //will
    { "Swap Edges 1 & 5",   128 }, //enjoy
    { "Swap Edges 2 & 3",   129 }, //the
    { "Swap Edges 2 & 4",   130 }, //order
    { "Swap Edges 2 & 5",   131 },
    { "Swap Edges 3 & 4",   132 },
    { "Swap Edges 3 & 5",   133 },
    { "Swap Edges 4 & 5",   134 },
    { "Swap Corners 1 & 2", 135 },  // g_currentFace->swapCorners(0, 1+num-135)
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

//Sublevel Y = Human Manual Rotate Routines (insert one piece manually into layer)
//submenu7_id = glutCreateMenu(menuHandler);
constexpr int MAX_HUMANLAYER_ALGORITHMS = 9;
constexpr StringAlgo humanlayer_algorithms[MAX_HUMANLAYER_ALGORITHMS] = {
    { "", 0},   // Initialize 0 with empty blank
    { "2nd Layer, 1 Edge (Left drop)",  291 },  // g_AlgoStringsLayer[num - 290].algo
    { "2nd Layer, 1 Edge (Right drop)", 292 },  // g_AlgoStringsLayer[2].algo,
    { "4th Layer, 1 Edge (Left drop)",  293 },  //                   [3]
    { "4th Layer, 1 Edge (Left+Flip)",  294 },  //                    4
    { "4th Layer, 1 Edge (Right drop)", 296 },  //            (swap)  6
    { "4th Layer, 1 Edge (Right+Flip)", 295 },  //            (swap)  5
    { "6th Layer, 1 Edge (Left drop)",  297 },  //                    7
    { "6th Layer, 1 Edge (Right drop)", 298 },  //                    8
};

//Sublevel1 Menu = Human Rotate Bulk-Solve using best layer routines (Solve.cpp)
//submenu1_id = glutCreateMenu(menuHandler);
constexpr int MAX_LAYERSOLVE_ALGORITHMS = 11;
constexpr StringAlgo layersolve_algorithms[MAX_LAYERSOLVE_ALGORITHMS] = {
    { "", 0},   // Initialize 0 with empty blank
    { "1st Layer: White Edges (Rotate)",   300 },   //rotateSolveWhiteEdges
    { "1st Layer: White Corners (Rotate)", 301 },   //rotateSolveWhiteCorners
    { "2nd Layer: All Edges (Rotate)",     302 },   //rotateSolveLayer2Edges
    { "3rd Layer: All Corners (Rotate)",   303 },   //rotateSolve3rdLayerCorners
    { "4th Layer: All Edges (Rotate)",     304 },   //rotateSolveLayer4Edges
    { "5th Layer: All Corners (Rotate)",   305 },   //rotateSolve5thLayerCorners
    { "6th Layer: All Edges (Rotate)",     306 },   //rotateSolveLayer6Edges
    { "7th Layer: Gray Edges (Rotate)",    307 },   //rotateSolveLayer7Edges
    { "7th Layer: Gray Corners (Rotate)",  308 },   //rotateSolve7thLayerCorners
    { "ALL Layers: #1 - #7 (Auto Solve)",  309 }, // 309 = Solve All
};

//SubLevel3 Menu - Computer Auto-InstaSolve - Layer by Layer using Teleport
//                 (internally "pops-out" pieces to move them, aka cheating)
//submenu3_id = glutCreateMenu(menuHandler);
constexpr int MAX_COMPUSOLVE_ALGORITHMS = 10;
constexpr StringAlgo compusolve_algorithms[MAX_COMPUSOLVE_ALGORITHMS] = {
    { "", 0},   // Initialize 0 with empty blank
    { "1st Layer: White Star (Teleport)",    40 },  //1st Layer edges    = resetFacesEdges(WHITE)
    { "1st Layer: White Corners (Teleport)", 41 },  //1st Layer corners  = resetFacesCorners(WHITE)
    { "2nd Layer: Edges (Teleport)",         42 },  //2nd Layer edges    = resetSecondLayerEdges()
    { "3rd Layer: Low Y's (Teleport)",       43 },  //3rd Layer corners  = resetlowYmiddleWCorners()
    { "4th Layer: Edges (Teleport)",         44 },  //4th Layer edges    = resetFourthLayerEdges()
    { "5th Layer: High Y's (Teleport)",      45 },  //5th Layer corners  = resethighYmiddleWCorners()
    { "6th Layer: Edges (Teleport)",         46 },  //6th Layer edges    = resetSixthLayerEdges()
    { "7th Layer: Gray Star (Teleport)",     47 },  //Last Layer edges   = resetFacesEdges(GRAY)
    { "7th Layer: Gray Corners (Teleport)",  48 },  //Last Layer corners = resetFacesCorners(GRAY)
};

//SubLevel4 Menu - Human Bulk Rotate Algos - Algorithms.hpp
//submenu4_id = glutCreateMenu(menuHandler);
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
    { "38. 7LL-Edge 5-way CCW cycle by +2 all", 88 },         //40
    { "17. 7LL-Edge 5-way  CW cycle by -2 all", 67 },         //41
    { "39. 7LL-Edge 5-way CCW star by 1,2,-1,2,1", 189 },    //39
    { "43. 7LL-Edge 5-way  CW star by -1,-2,1,-2,-1", 193 }, //43
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
//COMMENTED OUT:
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

#endif // __ALGOMENU_HPP__
