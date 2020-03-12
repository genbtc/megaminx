#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include <vector>
#include <variant>
#include <type_traits>

class Megaminx; //forward declare for Attach

class Face : public Piece {
public:
    Face();
    virtual ~Face() = default;

    Center *center = {};
    Corner *corner[5] = {};
    Edge   *edge[5] = {};
    //TODO: maybe hold a pointer back to the parent megaminx.

    int getNum() const { return thisNum; }
    void initAxis(int n);
    bool render();
    void rotate(int direction);
    bool placeParts(int direction);

    std::vector<int> defaultCorners, defaultEdges;

    void attachCenter(Center* c, double* centerVertexBase);
    void attachCornerPieces(const Megaminx* megaminx, Corner& cornersPTR);
    void attachEdgePieces(const Megaminx* megaminx, Edge& edgesPTR);

   template <typename T>
    Piece* getFacePiece(int i) const {
        if (std::is_same<T, Edge>::value)
            return edge[i];
        else if (std::is_same<T, Corner>::value)
            return corner[i];
        return center;
    };
   template<typename T>
    std::vector<int> findPiecesOrder() const;
    std::vector<int> findCornersOrder() const;
    std::vector<int> findEdgesOrder() const;
   template<typename T>
    int find5PieceLoc(int pieceNum) const;
    int find5EdgeLoc(int pieceNum) const;
    int find5CornerLoc(int pieceNum) const;

   template <typename T>
    void swapPieces(int a, int b);
    void swapCorners(int a, int b);
    void swapEdges(int a, int b);

private:
    void TwoEdgesFlip(int a, int b);
    void FlipCorners(int a, int b, int c, int d, const int* pack);

   template<typename T>
    void QuadSwapPieces(const int pack[8]);
    void QuadSwapCorners(const int pack[8]);
    void QuadSwapEdges(const int pack[8]);

    int thisNum;
    int turnDir;
    bool rotating;
    double angle;
    double axis[3];
public:
    enum TurnDir { Clockwise = -1, None = 0, CounterClockwise = 1 };
    enum TurnDir2 { CW = -1, CCW = 1 };
};

struct numdir {
    int num;
    int dir;
    int algo;
};

//Named Flip Direction lists:
constexpr int FlipInwards[4] = { 0, 1, 1, 0 };
constexpr int FlipOutwards[4] = { 1, 0, 0, 1 };
constexpr int FlipBackwards[4] = { 0, 0, 1, 1 };
constexpr int FlipForwards[4] = { 1, 1, 0, 0 };
constexpr int FlipBackwardAlt[4] = { 0, 1, 0, 1 };
constexpr int FlipForwardAlt[4] = { 1, 0, 1, 0 };

//These are invoked when Face::placeParts() is ran, when it's rotating.
//Called from Face::render(), only when something is moved, NEVER on startup.
//Flip direction lists for PlaceParts: //CounterClockwise CORNERS
//CCW Corners
constexpr static int  CCW0C[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr static int  CCW1C[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
constexpr static int  CCW2C[8] = { 0, 1, 0, 2, 2, 3, 2, 4 };
constexpr static int  CCW3C[8] = { 3, 4, 1, 3, 1, 2, 0, 1 };
constexpr static int  CCW4C[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr static int  CCW5C[8] = { 1, 3, 1, 4, 1, 2, 0, 1 };
constexpr static int  CCW6C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr static int  CCW7C[8] = { 1, 3, 3, 4, 4, 2, 2, 0 };
constexpr static int  CCW8C[8] = { 4, 3, 4, 2, 4, 0, 4, 1 };
constexpr static int  CCW9C[8] = { 4, 3, 4, 2, 4, 0, 4, 1 };
constexpr static int CCW10C[8] = { 4, 3, 4, 2, 4, 0, 4, 1 };
constexpr static int CCW11C[8] = { 4, 2, 4, 3, 3, 1, 1, 0 };
//Flip direction lists for PlaceParts: //Clockwise CORNERS
//CW Corners
constexpr static int  CW0C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr static int  CW1C[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr static int  CW2C[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
constexpr static int  CW3C[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
constexpr static int  CW4C[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
constexpr static int  CW5C[8] = { 0, 1, 1, 2, 1, 4, 1, 3 };
constexpr static int  CW6C[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr static int  CW7C[8] = { 2, 0, 4, 2, 3, 4, 1, 3 };
constexpr static int  CW8C[8] = { 4, 1, 4, 0, 4, 2, 4, 3 };
constexpr static int  CW9C[8] = { 4, 1, 4, 0, 4, 2, 4, 3 };
constexpr static int CW10C[8] = { 4, 1, 4, 0, 4, 2, 4, 3 };
constexpr static int CW11C[8] = { 1, 0, 3, 1, 4, 3, 2, 4 };
//Flip direction lists for PlaceParts: //CounterClockwise Edges
//CCW Edges
constexpr static int  CCW0E[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr static int  CCW1E[8] = { 4, 1, 1, 3, 0, 1, 0, 2 };
constexpr static int  CCW2E[8] = { 1, 0, 1, 2, 1, 3, 3, 4 };
constexpr static int  CCW3E[8] = { 3, 2, 4, 3, 0, 1, 1, 2 };
constexpr static int  CCW4E[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
constexpr static int  CCW5E[8] = { 2, 4, 2, 3, 0, 2, 0, 1 };
constexpr static int  CCW6E[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr static int  CCW7E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };
constexpr static int  CCW8E[8] = { 0, 1, 1, 2, 2, 4, 3, 4 };
constexpr static int  CCW9E[8] = { 0, 1, 1, 2, 2, 4, 3, 4 };
constexpr static int CCW10E[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
constexpr static int CCW11E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };    // = 12403
//Flip direction lists for PlaceParts: //Clockwise Edges
//CW Edges
constexpr static int  CW0E[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr static int  CW1E[8] = { 0, 2, 0, 1, 1, 3, 4, 1 };
constexpr static int  CW2E[8] = { 3, 4, 1, 3, 1, 2, 1, 0 };
constexpr static int  CW3E[8] = { 1, 2, 0, 1, 4, 3, 3, 2 };
constexpr static int  CW4E[8] = { 3, 4, 1, 3, 1, 2, 0, 1 };
constexpr static int  CW5E[8] = { 0, 1, 0, 2, 2, 3, 2, 4 };
constexpr static int  CW6E[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr static int  CW7E[8] = { 0, 1, 0, 2, 0, 4, 0, 3 };
constexpr static int  CW8E[8] = { 3, 4, 2, 4, 1, 2, 0, 1 };
constexpr static int  CW9E[8] = { 3, 4, 2, 4, 1, 2, 0, 1 };
constexpr static int CW10E[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr static int CW11E[8] = { 0, 1, 0, 2, 0, 4, 0, 3 };


struct AlgoString {
    int num;
    const char * algo;
    int repeatX;
    std::vector<numdir> *bulk;
};

//AlgoStrings                                       //name  //foundOrder - defaultOrder [i]
//commented description is ^ above the described command
constexpr AlgoString g_AlgoStrings[45] = {
    {0, ""},

    // most common one, suitable for white corners or any.
    { 1, "r u R' U' "},

    // opposite pair to first one
    { 2, "l u L' U' "},

    //simple L#2-Edges - (opposite is case#2)
        //https://youtu.be/PWTISbs0AAs?t=493 og video., Insert to Left = This First, then next.
    { 3, "U' L' u l"},

    //simple L#2-Edges - (opposite is case#1) 
        //same as ^ video: Insert to Right = This first, then previous.
    { 4, "u r U' R' "},

    //Last Layer Step 4: Orienting Corners (color-flipping in-place)
        //You repeat this over and over with all corners until they are all orientated correctly.
        //identity function repeats every 6x. Each gray color will take 2x cycles to colorflip. It will dis-align the R and D faces temporarily.
        //NOTE: This may SEEM like it might result in a catch-22 where the 1 last gray corner piece is color-flipped but everything else is solved,
        //  But that does not occur with an internally consistent megaminx (ie pieces are not "popped out")
        //Note2: a variation on this step is also done in the 1st White Corners stage, as 3 repetitions of this, but we can skip the 4th "dr" there (leaves temp side messy)
    { 5, "R' DR' r dr"},

    //repeat two times
    //{ 5, "R' DR' r dr", 2},

    //repeat four times
    //{ 5, "R' DR' r dr", 4},

    // #7Last Layer: Step 3 - Orient bottom Corners #1,2,3 CCW (DUPE) // Put the corners into their correct positions. 
        //ONLY affects Corners. //3rd Repetition = Undo
        //BACK-LEFT (9:00 to 12:00) 2corners+3edges will stay the SAME.
        //the 3, 5 and 7 o clock corners will rotate with each other  (Counter-clockwise also)
    //DUPLICATE: Algo 6 and 7 operate in the same direction, just different face of reference. dont really need both. can remove.
    { 6, "u l U' R', u L' U' r"},

    // #7Last-Layer: Step 3 - Orient rear Corners #3,5,4 CCW // Put the corners into their correct positions. 
        //ONLY affects Corners. //3rd Repetition = Undo
        // FRONT FACE corners (1&2) @ (5'o and 7 o'clock) will stay the SAME,
        // The 3 affected corners will cycle rotate around (counter-clockwise also).
    //NOTE: Algo 7 and 6 operate in the same direction, just different face of reference. dont really need both. rather remove other.
    { 7, "u r U2' L' u2 R' U2' l u"},

    // #7Last-Layer: Step 2/3 - Corner+Edge Permutation 3: // Entire Front Line = Safe. untouched is C1,C2,E1,E5
        // (5 to 1, 1 to 2, 2 to 5) = (front 2 corners, front/left 2 edges) Rotates chunks of 2 clockwise
        //NOTE: CORNERS ARE AFFECTED by this as well as EDGES too, but solved gray tops never change  //3rd Repetition = Undo
    { 8, "r u R' F', r  u  R' U', R' f r2 U' R' "},       //(mod existing by @   0,1,1,-2,0

    // #7Last-Layer: Step 2/3 - Corner+Edge Permutation 5: //R.Back=Safe
        //(Corners 5 & 3 swap + 1 & 2 swap) && (Edges 1 & 5 swap + 4 & 2 swap)
        //NOTE: CORNERS ARE AFFECTED by this as well as EDGES too, but solved gray tops never change  //2nd Repetition = Undo
    { 9, "l r u2, L' u R', l U' r u2, L' u2 R' "},        //(mod existing by @   -1,2,0,-2,1

    // #7Last-Layer: Step 2/3 : main - SUNE
        // Rotates the far 3 Star/Edge pieces into their correct position + (Affects corners too)
        //The nearest 1 corner and 2 edges stay the same (Corner 1 and Edge 1/5 remain untouched)
        // (The 6 and 8 o'clock pieces will remain unaffected and in their same position)
        //The remaining 3 will rotate cyclicly in an Anti Clockwise fashion. (same color)
        //#2. Two Edges Solved: (Solved edge in the front & lower left) 
    {10, "r u R' u, r U2' R' "},                           //(mod existing by @   0,2,-1,-1,0

    // #7Last-Layer: Step 2/3: , continued
        //#1. Two Edges Solved : (Solved edge in the front & upper right)
        //#3. One Edge is Permuted : (Permuted edge in the front) this algo + then go back do the previous algo)
    {11, "r u2 R' u, r u2 R' "},                           //(mod existing by @   0,-2,0,-2,-1

    // #7Last-Layer: Step 2: Edge Permutation 1+: HORSEFACE- //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
        //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others Counter-Clockwise
        // 13 moves * Repeated 5 times = Total 65 moves.
    {12, "r2 U2' R2' U', r2 U2' R2' ", 5},                 //(mod existing by @   0,-2,0,-2,-1

    // #7Last-Layer: Step 2: Edge Permutation 2-: HORSEFACE+ (opposite of previous; all the "up"s get reversed)
        //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others ClockWise
        // 13 moves * Repeated 5 times = Total 65 moves.
    {13, "r2 u2 R2' u, r2 u2 R2' ", 5},                    //(mod existing by @   0,2,0,1,2

    // #7Last-Layer: Step 2: Edge Permutation 3a-:  MUSHROOM- //11 o'clock to 4 o'clock, 4 o'clock to 1 o'clock, 1 o'clock to 11 o'clock (4 to 2, 2 to 3, 3 to 4)
        //Unaffecteds(2) = stay safe on Front/Left sides  = 16 moves. //edges cycle rotate = Counter-clockwise. corners aren't affected...  
    {14, "r u R' u, R' U' r2 U', R' u R' u, r U2' "},      //(mod existing by @   0,2,-1,-1,0

    // #7Last-Layer: Step 2: Edge Permutation 3b+: MUSHROOM+
        //Opposite of EdgePermutation 3a-(if cube is rotated 2 turns CW, Y++).
        //DUPLICATE: manually reverse engineered from 3c+, to be equal to 3a-. corners aren't affected...  (Rotates the right face instead of front)
        //Unaffecteds(2) = stay safe on Both+Back sides.  = 16 moves. //edges cycle rotate = clockwise
    {15, "R' U' r U', r u R2' u, r U' r U', R' u2"},       //(mod existing by @   1,-2,0,0,1

    // #7Last-Layer: Step 2: Edge Permutation 3c+: MUSHROOM+
        //Opposite of EdgePermutation 3a- (if cube is rotated 2 turns CCW, Y--).
        //NOTE: Identical Twin to 3b (but Rotates the front face instead of right)
        //Unaffecteds(2) = stay safe on Right/R.Back sides. = 16 moves. edges cycle rotate = clockwise
    {16, "F' U' f U', f u F2' u, f U' f U', F' u2"},       //(mod existing by @   -2,0,0,1,1

    // #7Last-Layer: Step 2: Edge 5-way star cycle CW+ , // -2,-2,-2,-2,-2)
        //Opposite Faces CW, 1 to 4, 4 to 2, 2 to 5, 5 to 3, 3 to 1 - //1,3,5,2,4 (new pieces @ 4,2,5,3,1)
        //60 moves total. copied from cube manual (turned upside down).
    {17, "L' u2 r U2' l u2 R' ", 6},

    // #7Last-Layer: Step 2: Edge 5-way star cycles , // 1,2,-1,2,1
        // Two halves, 1 to 2, 2 to 4, 4 to 3, 3 to 5, 5 to 1 - //1,5,3,4,2  (new pieces @ 2,4,5,3,1)
        //60 moves total. (copied from cube manual)
    {18, "R' l U' r L' u2, R' l U2' r L' u2, R' l U2' r L' u2, R' l U2' r L' u2, R' l U2' r L' u2, R' l U2' r L' u2, R' l U2' r L' u2, R' l U' r L' "}, // 1,-1,1,-2,-2

    // #7Last-Layer: Step 2: Edge two adjacent swaps,          //(mod existing by @  0,1,-1,1,-1
        //Safe Edge = FRONT. then swap 2&3 and 4&5 (colors and corners maintained)
        //44 moves total. (copied from manual. )
    {19, "U2' L2' U2' l2 U' L2' U2' l2 U', L2' U2' l2 U' L2' U2' l2 U', L2' U2' l2 U' L2' U2' l2 u"},

    // #7Last-Layer: Step 2: Edge opposite swaps & flip/Invert, //(mod existing by @  0,2,2,-2,-2
        //Safe Edge = FRONT. then swap 2&4 and 3&5 ( right/backLeft swap and left/backRight swap) + and INVERTS @ 8 o'clock and 1 o'clock <--* 
        //30 moves total. (copied from manual.), Repeat = Undo (origin in two cycles)
    {20, "R' l F2' r L' u2 R' l, F' r L' U2' R' l F2' r, L' u2 R' l F' r L' U2' "},

    // #2nd-Layer Edges(LEFT) =  7 o'clock to 9 o'clock:
        //Note: copied right algo then reverse engineered it myself back to the left
    {21, "dl l dl L', dL' F' dL' f"},

    // #2nd-Layer Edges(RIGHT) =  5 o'clock to 3 o'clock:
        //Note: Algo from QJ cube manual & (White face on top) (Exact opposite above)
    {22, "dR' R' dR' r, dr f dr F' "},

    // #4th-Layer Edges(LEFT), (between the middle W), fourthLayerEdgesA() // 12 o'clock to 7 o'clock
        //Obviously: Cube must have gray face on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
    {23, "F' R' F', F' r f"},

    // #4th-Layer Edges(RIGHT), (between the middle W), fourthLayerEdgesB() //12 o'clock to 5 o'clock.
        //Obviously: Cube must have gray face on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
    {24, "f l f, f L' F' "},

    // #6th-Layer Edges(LEFT) //Must have Layers 1-5 solved, and 7th layer is affected.
        // swap edge from face's star at 12 o'clock to Flop in (pinned to center) To the edge @  9 o'clock
    {25, "U' L' u l, u f U' F' "},

    // #6th-Layer Edges(RIGHT) //opposite of previous, To the edge @ 3 o'clock
    {26, "u r U' R', U' F' u f"},

    // #7LL: Step 3, Clockwise Cycle Corners (very necessary, didnt have) Safe Area = Right
//TODO Should be moved higher in the list, Important.
    {27, "L' u2 R U2', L u2 R' U2' "},

    // #7LL: Step 3, CounterClockwise Cycle Corners (already described as Algo #6/#7) Safe Area = Left
//TODO Dupes Should be removed. Commented out in menu
    {28, "R U2' L' u2, R' U2' L u2"},

    // #7LL: Step 1, Edge Orientation, Flip Colors only (Invert 4 in place), front=safe.
        //(~57 moves total.) (copied from manual.) similar to #20
    {29, "R' l F2' r L' u2 R' l,  F' r L' U2' R' l F2' r,  L' u2 R' l F' r L' u,  R' l F' r L' u2 R' l,  F2' r L' U2' R' l F' r,  L' u2 R' l F2' r L' "},

    // #7LL: Step 2, Edge Permutation 1:  //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
        //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others Counter-Clockwise
        //clone of #12 - except one repetition only    // 13 moves in 1 rep, CORNERS ARE AFFECTED
    {30, "r2 U2' R2' U', r2 U2' R2' ", 1 },                 //(mod existing by @   0,-2,0,-2,-1

    // #7LL: Step 2, Edge Permutation 2: (opposite of previous; all the "up"s get reversed)
        //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others ClockWise
        //clone of #13 - except one repetition only    // 13 moves in 1 rep, CORNERS ARE AFFECTED
    {31, "r2 u2 R2' u, r2 u2 R2' ", 1 },                    //(mod existing by @   0,2,0,1,2

    // #7Last-Layer: Step 2: Edge Permutation 3d- //"LL Edge 3d- CCW Both+Backs=Safe"
//TODO:  MOVE UP & Organize with related 
    //{ 14, "r u R' u,  R' U' r2 U',  R' u R' u,  r U2' " },  //(mod existing by @   0,2,-1,-1,0
    //Copied 3a- ^ and reversed the algo. However it actually performs a reverse of 3B+
    {32, "U2' r, u R' u R', U' r2 U' R', u R' u r" },

    // #7Last-Layer: Step 2: Edge Permutation 3e- //"LL Edge 3e+  CW Front/Left=Safe"
//TODO:  MOVE UP & Organize with related 
    //{ 15, "R' U' r U', r u R2' u, r U' r U', R' u2" },     //(mod existing by @   1,-2,0,0,1
    //Therefore if i reverse 3b+ ^ we actually get an algo that performs a reverse on 3A-
    {33, "u2 R', U' r U' r, u R2' u r, U' r U' R' " },

    // #4th-Layer Edges(LEFT+INVERT)
//TODO:  MOVE UP & Organize with related 
    {34, "U' R' DR' f, f dr r" },

    // #4th-Layer Edges(RIGHT+INVERT)
//TODO:  MOVE UP & Organize with related 
    {35, "u l dl F', F' DL' L' " },

    //#7LL-2/3- Edge Permute #6, 2+2 swap, swaps Edges 2&5 + 3&4, (Color Safe) BUT AFFECTS CORNERS!
    //Swap an adjacent pair, and a non - adjacent pair of edges (U/R <-> U/L and U/BR <-> U/BL)
    //(R+ U+) (R- U+) (R+ U-) (R- U++) (R+ U++ R-)
    //repeating 2x becomes a corner-permutation algo only=(corners color flip in-place by 1), 6x Repetitions = Undo, go back to origin (+3 color flips)
//TODO:  MOVE UP & Organize with related 
    {36, "r u R' u, r U' R' u2, r u2 R' ", 1},

    //#7LL-2- BEST Bunny from ELL, 2+2 swap, swaps Edges 2&5 + 3&4, (Color Safe), https://sites.google.com/site/permuteramera/other-methods/ell
    //one rep affects corners. 5 reps maintains corners. 13 moves * 5 = 65 moves total.
//TODO:  MOVE UP & Organize with related 
    {37, "r U2' R' U', r U' R' u, r U2' R' ", 5},

    //Opposite direction of #17, 5-way edge cycle CCW (+2,+2,+2,+2,+2)
    {38, "r U2' L' u2 R' U2' l", 6 },

    //Shorter version of #18 (5-way edge cycle 1,2,-1,2,1) (32m vs 60m) (#14+#33) (applicable face is changed by -2)
    {39, "r u R' u, R' U' r2 U', R' u R' u, r U2'  ,, u2 L', U' l U' l, u L2' u l, U' l U' L' " },

    //Shorter version of #17 (5-way edge Cycle -2) (48m vs 60m) (#14+#14+#14)
    {40, "l u l' u, l' U' l2 U', l' u l' u, l U2'  ,, f u F' u, F' U' f2 U', F' u F' u, f U2' ,, r u R' u, R' U' r2 U', R' u R' u, r U2' "},

    //shorter verison of #38 (5-way edge Cycle +2) (48m vs 60m) (#33+#33+#33) (opposite of #40 above)
    {41, "u2 R', U' r U' r, u R2' u r, U' r U' R'  ,, u2 F', U' f U' f, u F2' u f, U' f U' f' ,, u2 l', U' l U' l, u l2' u l, U' l U' l' "},

    //shorter version of #19 Bunny Adj. Edge Swap 2&3/4&5 (#33+#33) (32m) (applicable face is changed by -2)
    {42, "u2 F', U' f U' f, u F2' u f, U' f U' F'  ,, u2 R', U' r U' r, u R2' u r, U' r U' R' " },

    //Shorter Opposite version of #39 (#18) (cycle -1,-2,1,-2,-1) (32m)  (applicable face is changed by -2)
    { 43, "l u L' u, L' U' l2 U', L' u L' u, l U2' ,, u2 R', U' r U' r, u R2' u r, U' r U' R' " },
};

#endif
