//Algorithms.hpp - Megaminx Version 1.4.4 - @gen 2023
#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

struct StringAlgo {
    const char* algo;
    int num;
};
struct AlgoString {
    int num{};
    const char* algo{};
    int len{};
    int repeatX=0;
    int modby[5]{};
};

constexpr int ALL_ALGORITHMS = 53;
constexpr AlgoString g_AlgoStrings[ALL_ALGORITHMS] = {
    // Initialize 0 with empty blank
    {0, ""},

    // most common one, suitable for white corners or any.
    //IMPORTANT
    {
        .num = 1,
        .algo = "r u R' U' ",
        .len = 4
    },
    // opposite pair to #1, corners.
    {
        .num = 2,
        .algo = "l u L' U' ",
        .len = 4
    },

    //2nd Layer edges (simple) moved to algoLayer[]
    { 3, "" },
    { 4, "" },

    //LL S4 Corners simple RDRD flip moved to #50
    { 5, "" },
    //Step 3 moved 6 to 25 and 7 to 26
    { 6, "" },
    { 7, "" },

    // #7Last-Layer: Step 2/3 - Corner+Edge Permutation 3: // Entire Front Line = Safe. untouched is C1,C2,E1,E5
    // (5 to 1, 1 to 2, 2 to 5) = (front 2 corners, front/left 2 edges) Rotates chunks of 2 clockwise
    //NOTE: CORNERS ARE AFFECTED by this as well as EDGES too
    //      solved gray tops never change
    {
        .num = 8,
        .algo = "r u R' F' , r  u  R' U' , R' f r2 U' R' ",
        .len = 14,
        .repeatX = 1, //3rd Repetition = Undo
        .modby = { 0, -2, 1, 1, 0 }
    },

    // #7Last-Layer: Step 2/3 - Corner+Edge Permutation 5: //R.Back=Safe
    //(Corners 5 & 3 swap + 1 & 2 swap) && (Edges 1 & 5 swap + 4 & 2 swap)
    //NOTE: CORNERS ARE AFFECTED by this as well as EDGES too\
    //      solved gray tops never change
    {
        .num = 9,
        .algo = "l r u2, L' u R' , l U' r u2, L' u2 R' ",
        .len = 16,
        .repeatX = 1, //2nd Repetition = Undo
        .modby = { 1, -2, 0, 2, -1 }
    },
    // #7Last-Layer: Step 2/3 : Corner+Edge main - SUNE
    // Rotates the far 3 Star/Edge pieces into their correct position + (Affects corners too)
    //The nearest 1 corner and 2 edges stay the same (Corner 1 and Edge 1/5 remain untouched)
    // (The 6 and 8 o'clock pieces will remain unaffected and in their same position)
    //The remaining 3 will rotate cyclicly in an Anti Clockwise fashion. (same color)
    //#2. Two Edges Solved: (Solved edge in the front & lower left)
    {
        .num = 10,
        .algo = "r u R' u, r U2' R' ",
        .len = 8,
        .modby = { 0, -1, -1, 2, 0}
    },

    // #7Last-Layer: Step 2/3: , Corner+Edge continued HORSEFACE- (corners very affected)
    //#1. Two Edges Solved : (Solved edge in the front & upper right)
    //#3. One Edge is Permuted : (Permuted edge in the front) this algo + then go back do the previous algo)
    {
        .num = 11,
        .algo = "r u2 R' u, r u2 R' ",
        .len = 9,
        .modby = { 0, -2, 0, -1, -2 }
    },

    // #7Last-Layer: Step 2: Edge Permutation 1+: HORSEFACE- //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
    //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others Counter-Clockwise
    // 13 moves * Repeated 5 times = Total 65 moves! (similar to 11 but preserves Corners)
    //HORSEFACE- #Algo12# and Algo#13#
    {
        .num = 12,
        .algo = "r2 U2' R2' U' , r2 U2' R2' ",
        .len = 13,
        .repeatX = 5,
        .modby = { 0, -2, 0, -1, -2 }
    },

    // #7Last-Layer: Step 2: Edge Permutation 2-: HORSEFACE+ (opposite of previous; all the "up"s get reversed)
    //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others ClockWise
    // 13 moves * Repeated 5 times = Total 65 moves!
    //HORSEFACE+ #Algo12# and Algo#13#
    {
        .num = 13,
        .algo = "r2 u2 R2' u, r2 u2 R2' ",
        .len = 13,
        .repeatX = 5,
        .modby = { 0, 2, 0, 2, 1 }
    },

    // #7Last-Layer: Step 2: Edge Permutation 3a-:  MUSHROOM- //11 o'clock to 4 o'clock, 4 o'clock to 1 o'clock, 1 o'clock to 11 o'clock (4 to 2, 2 to 3, 3 to 4)
    //Unaffecteds(2) = stay safe on Front/Left sides  = 16 moves.
    //edges cycle rotate = Counter-clockwise. corners aren't affected...
    //MUSHROOM- #Algo#14 (all) 3a- (F/L Safe) / twoAdjacentOffColors
    {
        .num = 14,
        .algo = "r u R' u, R' U' r2 U' , R' u R' u, r U2' ",
        .len = 16,
        .modby = { 0, -1, -1, 2, 0 }
    },
    // #7Last-Layer: Step 2: Edge Permutation 3b+: MUSHROOM
    //Opposite of EdgePermutation 3a-(if cube is rotated 2 turns CW, Y++).
    //DUPLICATE: manually reverse engineered from 3c+, to be equal to 3a-.
    //edges cycle rotate = clockwise . corners aren't affected...  (Rotates the right face instead of front)
    //Unaffecteds(2) = stay safe on Both+Back sides.  = 16 moves.
    {
        .num = 15,
        .algo = "R' U' r U' , r u R2' u, r U' r U' , R' u2",
        .len = 16,
        .modby = { 1, 1, 0, 0, -2 }
    },
    // #7Last-Layer: Step 2: Edge Permutation 3c+: MUSHROOM
    //Opposite of EdgePermutation 3a- (if cube is rotated 2 turns CCW, Y--).
    //POTENTIAL DUPLICATE: Identical Twin to 3b (but Rotates the front face instead of right).
    //Unaffecteds(2) = stay safe on Right/R.Back sides. = 16 moves. edges cycle rotate = clockwise
    {
        .num = 16,
        .algo = "F' U' f U' , f u F2' u, f U' f U' , F' u2",
        .len = 16,
        .modby = { -2, 0, 0, 1, 1 }
    },
    //#32=#7Last-Layer: Step 2: Edge Permutation 3d- //"LL Edge 3d- CCW Both+Backs=Safe"
    //#33= 3e

    // #7Last-Layer: Step 2: Edge Position, 5-way star cycle CW+ , Opposite Faces CW
    //10 moves * 6 = 60 moves total. copied from cube manual (turned upside down).
    {
        .num = 17,
        .algo = "L' u2 r U2' l u2 R' ",
        .len = 10,
        .repeatX = 6,
        .modby = { 2, 2, 2, 2, 2 }
    },
    // #7Last-Layer: Step 2: Edge Position, 5-way star cycles, Two Halves
    // 60 moves total. (copied from cube manual)
    {
        .num = 18,
        .algo = "R' l U' , r L' u2, R' l U2' , r L' u2 ,, R' l U2,' r L' u2 ,, R' l U2' , r L' u2 ,, R' l U2' , r L' u2 ,, R' l U2' , r L' u2 ,, R' l U2' , r L' u2 , R' l U' , r L' ",
        .len = 60,
        .modby = { -1, -2, -2, 1, -1 }
    },

    // #7Last-Layer: Step 2: Edge Position, two adjacent swaps,
    // Safe Edge = FRONT. then swap 2&3 and 4&5 (colors and corners maintained)
    // 44 moves total. (copied from manual. )
    {
        .num = 19,
        .algo = "U2' L2' U2' l2 U' L2' U2' l2 U' ,, L2' U2' l2 U' L2' U2' l2 U' ,, L2' U2' l2 U' L2' U2' l2 u",
        .len = 44,
        .modby = { 0, -1, 1, -1, 1 }
    },

    // #7Last-Layer: Step 2: Edge Position, opposite swaps & flip/Invert,  INVERT colors @ 8'/1'
    //BUNNY 2-COLOR: (OFF-COLOR OPPOSITES)
    //Safe Edge = FRONT. then swap 2&4 and 3&5 ( right/backLeft swap and left/backRight swap)
    //30 moves total. (copied from manual.), Repeat = Undo (origin in two cycles)
    {
        .num = 20,
        .algo = "R' l F2' r L' u2 R' l, F' r L' U2' R' l F2' r, L' u2 R' l F' r L' U2' ",
        .len = 30,
        .modby = {  0, -2, -2, 2, 2, }
    },
    // #7Last-Layer: Step 1, Edge Orientation, 2&3 and 4&5 Flip Colors only (Invert 4 in place), front=safe.
    //BUNNY 4-COLOR-only (with 1 solved, covers all possibilities)
    // ~57 moves total. (copied from manual.)
    //TODO: organize: similar to #20
    {
        .num = 21,
        .algo = "R' l F2' r L' u2 R' l,  F' r L' U2' R' l F2' r,  L' u2 R' l F' r L' u,  R' l F' r L' u2 R' l,  F2' r L' U2' R' l F' r,  L' u2 R' l F2' r L' ",
        .len = 57,
    },

    //moved to g_AlgoStringsLayer[] at the bottom:
    //2nd Layer Edges
    //4nd Layer Edges
    { 22, "" },
    //6th Layer Edges
    { 23, "" },
    { 24, "" },

    // #7Last Layer: Step 3 - Orient bottom Corners #1, 2, 3 CCW (DUPE) // Put the corners into their correct positions.
    //ONLY affects Corners. //3rd Repetition = Undo
    //BACK-LEFT (9:00 to 12:00) 2corners+3edges will stay the SAME.
    //the 3, 5 and 7 o clock corners will rotate with each other  (Counter-clockwise also)
    //DUPLICATE: #26 operates in the same direction, just different face of reference. dont really need both.
    //TODO: organize: dupe: not used. Dupes Should be removed. can remove.
    //NOTE: BUT shorter than #26 by two moves.
    {
        .num = 25,
        .algo = "u l U' R' , u L' U' r",
        .len = 8,
    },
    // #7Last-Layer: Step 3 - Orient rear Corners #3, 5, 4 CCW // Put the corners into their correct positions.
    //ONLY affects Corners. //3rd Repetition = Undo
    // FRONT FACE corners (1&2) @ (5'o and 7 o'clock) will stay the SAME,
    // The 3 affected corners will cycle rotate around (counter-clockwise also).
    //NOTE: ^ Algo #25 operate in the same direction, just different face of reference.
    //IMPORTANT
    {
        .num = 26,
        .algo = "u r U2' L' u2 R' U2' l u",
        .len = 12,
    },
    // #7LL: Step 3, Corners Cycle  CW  - ( Safe Area = Right )
    //IMPORTANT
    {
        .num = 27,
        .algo = "L' u2 R U2' , L u2 R' U2' ",
        .len = 12,
    },
    // #7LL: Step 3, Corners Cycle CCW  - ( Safe Area = Left )
    //TODO: organize: (already described as Algo #6/#7)
    //TODO: organize: dupe: Dupes Should be removed. already Commented out in menu
    {
        .num = 28,
        .algo = "R U2' L' u2, R' U2' L u2",
        .len = 12,
    },

    { 29, "" },
    // #7LL: Step 2, Edge Permutation 1: HORSEDGE+ CW //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
    //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others CCW Counter-Clockwise
    // 13 moves in 1 rep, CORNERS ARE AFFECTED (gray stays)
    //TODO: organize: similar to #12 (except one repetition only)
    {
        .num = 30,
        .algo = "r2 U2' R2' U' , r2 U2' R2' ",
        .len = 13,
        .modby = { 0, 2, 0, 2, 1}
    },

    // #7LL: Step 2, Edge Permutation 2: HORSEDGE- CCW (opposite of previous; all the "up"s get reversed)
    //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others  CW ClockWise
    // 13 moves in 1 rep, CORNERS ARE AFFECTED (gray stays)
    //TODO: organize: similar to #13 (except one repetition only)
    {
        .num = 31,
        .algo = "r2 u2 R2' u, r2 u2 R2' ",
        .len = 13,
        .modby = { 0, -2, 0, -1, -2 }
    },

    // #7Last-Layer: Step 2: Edge Permutation 3d- //"LL Edge 3d- CCW Both+Backs=Safe"
    //{ 14, "r u R' u,  R' U' r2 U' ,  R' u R' u,  r U2' " },
    //Copied 3a- ^ and reversed the algo. However it actually performs a reverse of 3B
    //TODO: organize: MOVE UP w/ related
    {
        .num = 32,
        .algo = "U2' r, u R' u R' , U' r2 U' R' , u R' u r",
        .len = 16,
        .modby = { -1, 2, 0, 0, -1 }
    },

    // #7Last-Layer: Step 2: Edge Permutation 3e- //"LL Edge 3e+  CW Front/Left=Safe"
    //{ 15, "R' U' r U' , r u R2' u, r U' r U' , R' u2" },
    //Therefore if i reverse 3b+ ^ we actually get an algo that performs a reverse on 3A-
    //MUSHROOM+ #Algo#33 (all)
    //TODO: organize: MOVE UP w/ related
    {
        .num = 33,
        .algo = "u2 R' , U' r U' r, u R2' u r, U' r U' R' ",
        .len = 16,
        .modby = { 0, -2, 1, 1, 0 }
    },

    { 34, "" },
    { 35, "" },

    //#7LL-2/3- Edge Permute #6, 2+2 swap, swaps Edges 2&5 + 3&4, (Color Safe) BUT AFFECTS CORNERS!
    //Swap an adjacent pair, and a non - adjacent pair of edges (U/R <-> U/L and U/BR <-> U/BL) (13 moves)
    //repeating 2x becomes a corner-permutation algo only=(corners color flip in-place by 1), 6x Repetitions = Undo, go back to origin (+3 color flips)
    //TODO: organize: MOVE UP w/ related . one iteration.
    {
        .num = 36,
        .algo = "r u R' u, r U' R' u2, r u2 R' ",
        .len = 13,
        .repeatX = 1,
        .modby = { 0, 2, -1, 1, -2 }
    },

    //#7LL-2- BEST Bunny from ELL, 2+2 swap, swaps Edges 2&5 + 3&4, (Color Safe),
    //from site https://sites.google.com/site/permuteramera/other-methods/ell
    //one rep affects corners. 5 reps maintains corners. 13 moves * 5 = 65 moves total!
    //BUNNY 2+2SWAP  #Algo#37
    // 2&5 + 3&4 (opposite horizontally)
    //TODO: organize: MOVE UP w/ related . 5 iterations.
    {
        .num = 37,
        .algo = "r U2' R' U' , r U' R' u, r U2' R' ",
        .len = 13,
        .repeatX = 5,
        .modby = { 0, 2, -1, 1, -2 }
    },

    // 5-way edge cycle CCW
    //TODO: organize: Opposite direction of #17 . 6 iterations.
    {
        .num = 38,
        .algo = "r U2' L' u2 R' U2' l",
        .len = 10,
        .repeatX = 6,
        .modby = { -2, -2, -2, -2, -2 }
    },

    //(5-way edge cycle  (32 vs 60 moves) (combines algo #14+#33)
    //TODO: organize: Shorter version of #18
    //TODO: organize: 39 = 14+33 = 18
    {
        .num = 39,
        .algo = "r u R' u, R' U' r2 U' , R' u R' u, r U2' ,, u2 L' , U' l U' l, u L2' u l, U' l U' L' ",
        .len = 32,
        .modby = { 1, -1, -1, -2, -2 }
    },

    //(5-way edge Cycle -2) (48 vs 60 moves) (repeats algo #14+#14+#14)
    //TODO: organize: (opposite is #41 below)
    //TODO: organize: Shorter version of #17
    //TODO: organize: 40 = 17 = 14+14+14, opposite = 41 ( = 38 )
    {
        .num = 40,
        .algo = "l u l' u, l' U' l2 U' , l' u l' u, l U2' ,, f u F' u, F' U' f2 U' , F' u F' u, f U2' ,, r u R' u, R' U' r2 U' , R' u R' u, r U2' ",
        .len = 48,
        .modby = { 2, 2, 2, 2, 2 }
    },

    //(5-way edge Cycle +2) (48 vs 60 moves) (repeats algo #33+#33+#33)
    //TODO: organize: (opposite of #40 above)
    //TODO: organize: shorter version of #38
    //TODO: organize: 41 = 38 = 33+33+33, opposite = 40 ( = 17 )
    {
        .num = 41,
        .algo = "u2 R' , U' r U' r, u R2' u r, U' r U' R' ,, u2 F' , U' f U' f, u F2' u f, U' f U' f' ,, u2 l' , U' l U' l, u l2' u l, U' l U' l' ",
        .len = 48,
        .modby = { -2, -2, -2, -2, -2 }
    },

    //Bunny Adjacent Edge Swap 2&3/4&5 (#33+#33) (32 moves)
    //BUNNY 2+2SWAP  #Algo#42=/=Algo#19
    // 2&3 + 4&5 (adjacent vertically)
    //TODO: organize: shorter version of #19
    //TODO: organize: 42 = 19 = 33+33 , opposite = 43 ( = 39=/=18 )
    {
        .num = 42,
        .algo = "u2 F' , U' f U' f, u F2' u f, U' f U' F' ,, u2 R' , U' r U' r, u R2' u r, U' r U' R' ",
        .len = 32,
        .modby = { -1, 1, -1, 1, 0 }
    },

    //Bunny Adjacent Edge Swap - Opposite of 42 above (32 moves)
    //TODO: organize: shorter opposite version of #39 #18
    //TODO: organize: 43 = 39=/=18 , opposite = 42=/=19
    {
        .num = 43,
        .algo = "l u L' u, L' U' l2 U' , L' u L' u, l U2' ,, u2 R' , U' r U' r, u R2' u r, U' r U' R' ",
        .len = 32,
        .modby = { 2, 2, 1, 1, -1 }
    },

    //Top star Yellow Cross First case (flips F and R edges):
    {
        .num = 44,
        .algo = "F U R , U' R' F' ",
        .len = 6
    },
    //Top star Yellow Cross Second case (flips F and B edges):
    {
        .num = 45,
        .algo = "F R U , R' U' F' ",
        .len = 6
    },

    //LL cycle edges+, Clockwise (affects corners):
    {
        .num = 46,
        .algo = "R U R' U , R U U U R' U",
        .len = 10
    },
    //LL cycle edges-, AntiClockwise (affects corners):
    {
        .num = 47,
        .algo = "U' R U2 R' , U' R U' R' ",
        .len = 9
    },

    //LL Cycle corners+, Clockwise (F,L,UL):
    {
        .num = 48,
        .algo = "L' U2 R U'2 , L U2 R' U'2",
        .len = 12
    },
    //LL Cycle corners-, Anticlockwise (F,R,UR):
    {
        .num = 49,
        .algo = "R U'2 L' U2 , R' U'2 L U2",
        .len = 12
    },
    //Last Layer Step 4: Orienting Corners (color-flipping in-place)
    //You repeat this over and over with all corners until they are all orientated correctly.
    //identity function repeats every 6x. Each gray color will take 2x cycles to colorflip. It will dis-align the R and D faces temporarily.
    //NOTE: This may SEEM like it might result in a catch-22 where the 1 last gray corner piece is color-flipped but everything else is solved,
    //  But that does not occur with an internally consistent megaminx (ie pieces are not "popped out")
    //Note2: a variation on this step is also done in the 1st White Corners stage, as 3 repetitions of this, but we can skip the 4th "dr" there (leaves temp side messy)
    //7LL: 4 Corners
    //IMPORTANT
    {
        .num = 50,
        .algo = "R' DR' r dr",
        .len = 4
    },
    //Layer 1 part 2 - Corners
    {
        .num = 51,
        .algo = "f dr F' ",
        .len = 3
    },
    //Layer 1 part 2 - Corners
    {
        .num = 52,
        .algo = "R' DR' r",
        .len = 3
    },
};
//see docs/algo-test-modBy.txt for the list of how these modify the gray face only

constexpr AlgoString g_AlgoStringsLayer[11] = {
    { 0 },
    // #2nd-Layer Edges(LEFT) =  7 o'clock to 9 o'clock:
    //Note: copied right algo then reverse engineered it myself back to the left
    {
        .num = 1,
        .algo = "dl l dl L' , dL' F' dL' f",
        .len = 8
    },
    // #2nd-Layer Edges(RIGHT) =  5 o'clock to 3 o'clock:
    //Note: Algo from QJ cube manual & (White face on top) (Exact opposite above)
    {
        .num = 2,
        .algo = "dR' R' dR' r, dr f dr F' ",
        .len = 8
    },
    // #4th-Layer Edges(LEFT), (between the middle W), fourthLayerEdgesA() // 12 o'clock to 7 o'clock
    // Left  (drop in edge) - Third layer edges and corners
    // rules: Cube must have gray face on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
    {
        .num = 3,
        .algo = "F' R' F' , F' r f",
        .len = 6
    },
    // #4th-Layer Edges(LEFT+INVERT)
    {
        .num = 4,
        .algo = "U' R' DR' f, f dr r",
        .len = 6
    },
    // #4th-Layer Edges(RIGHT+INVERT)
    {
        .num = 5,
        .algo = "u l dl F' , F' DL' L' ",
        .len = 7
    },
    // #4th-Layer Edges(RIGHT), (between the middle W), fourthLayerEdgesB() //12 o'clock to 5 o'clock.
    // Right  (drop in edge) - Third layer edges and corners
    // rules: Cube must have gray face on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
    {
        .num = 6,
        .algo = "f l f, f L' F' ",
        .len = 6
    },
    // #6th-Layer Edges(LEFT)
    //Must have Layers 1-5 solved, and 7th layer is affected.
    // swap edge from face's star at 12 o'clock to Flop in (pinned to center) To the edge @  9 o'clock
    {
        .num = 7,
        .algo = "U' L' u l, u f U' F' ",
        .len = 8
    },
    // #6th-Layer Edges(RIGHT)
    //opposite of previous, To the edge @ 3 o'clock
    {
        .num = 8,
        .algo = "u r U' R' , U' F' u f",
        .len = 8
    },
    //simple L#2-Edges - (opposite is case#2)
    //https://youtu.be/PWTISbs0AAs?t=493 og video., Insert to Left = This First, then next.
    {
        .num = 9,
        .algo = "U' L' u l",
        .len = 4
    },
    //simple L#2-Edges - (opposite is case#1)
    //same as ^ video: Insert to Right = This first, then previous.
    {
        .num = 10,
        .algo = "u r U' R' ",
        .len = 4
    },
};

constexpr AlgoString g_edgeOrientation[4]= {
    { 0 },
    //Algo #45 Above
    {
        .num = 1,
        .algo = "f, r u R' U', F' ",
        .len = 6
    },
    //Algo #44 Above
    {
        .num = 2,
        .algo = "f, u r U' R', F' ",
        .len = 6
    },
    {
        .num = 3,
        .algo = "f r u2, R2' f r F', U2' F' ",
        .len = 12
    }
};

constexpr AlgoString g_cornerOrientation[4]= {
    { 0 },
};

#endif  // __ALGORITHMS_H__
