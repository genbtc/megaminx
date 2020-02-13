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

    Center *center;
    Corner *corner[5];
    Edge   *edge[5];
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
};

//Named Flip Direction lists:
constexpr int FlipInwards[4] = { 0, 1, 1, 0 };
constexpr int FlipOutwards[4] = { 1, 0, 0, 1 };
constexpr int FlipBackwards[4] = { 0, 0, 1, 1 };
constexpr int FlipForwards[4] = { 1, 1, 0, 0 };
constexpr int FlipBackwardAlt[4] = { 0, 1, 0, 1 };
constexpr int FlipForwardAlt[4] = { 1, 0, 1, 0 };

//These are invoked when Face::placeParts() is ran, when it's rotating.
//Called from Face::render() only, not on startup.
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

constexpr static int m_secondLayerEdges[5]  = { 5, 6, 7, 8, 9 };
constexpr static int m_fourthLayerEdgesA[5] = { 10, 11, 12, 13, 14 };
constexpr static int m_fourthLayerEdgesB[5] = { 15, 16, 17, 18, 19 };
constexpr static int m_sixthLayerEdges[5]   = { 20, 21, 22, 23, 24 };

struct AlgoString {
    int num;
    const char * algo;
    int repeatX;
};

//AlgoStrings                                       //name  //foundOrder - defaultOrder [i]
//commented description is ^ above the described command
constexpr AlgoString g_AlgoStrings[32] = {
    {0, ""},

    // most common one, suitable for white corners or any.
    { 1, "r u R' U'"},

    // opposite pair to first one
    { 2, "l u L' U'"},

    //simple L#2-Edges - (opposite is case#2)
        //https://youtu.be/PWTISbs0AAs?t=493 og video., Insert to Left = This First, then next.
    { 3, "U' L' u l"},

    //simple L#2-Edges - (opposite is case#1) 
        //same as ^ video: Insert to Right = This first, then previous.
    { 4, "u r U' R'"},

    //Last Layer Step 4: Orientating (color-flipping) the Corners. 
        //You repeat this over and over with all corners until they are all orientated correctly.
        //identity function repeats every 6x. Each gray color will take 2x cycles to colorflip. It will dis-align the R and D faces temporarily.
        //NOTE: This may result in a catch-22 where the 1 last gray corner piece is color-flipped but everything else is solved. Not sure what then.
        //Note2: this step is also done in the White Corners stage by repetitions of 3
    { 5, "R' DR' R DR"},

    //repeat two times
    //{ 5, "R' DR' R DR", 2},

    //repeat four times
    //{ 5, "R' DR' R DR", 4},

    // #7Last Layer: Step 3 - Orient bottom Corners #1,2,3 CCW // Put the corners into their correct positions. 
        //ONLY affects Corners. //repeat 3x = undo
        //left+rear-side (9:00 to 12:00) and back-left's 2corner+3edges will stay the SAME.
        //the 3, 5 and 7 o clock corners will rotate with each other  (Counter-clockwise also)
    // Algo 6 and 7 operate in the same direction, just different face of reference. dont really need both.
    { 6, "u l U' R' u L' U' r"},

    // #7Last-Layer: Step 3 - Orient rear Corners #3,5,4 CCW // Put the corners into their correct positions. 
        //ONLY affects Corners. //repeat 3x = undo 
        // Front face corners (1&2) @ (5'o and 7 o'clock) will stay the SAME,
        // The 3 affected corners will cycle rotate around (counter-clockwise also).
    // Algo 7 and 6 operate in the same direction, just different face of reference. dont really need both.
    { 7, "u r U2' L' u2 R' U2' l u"},

    // #7Last-Layer: Step 2/3 - Corner+Edge Permutation 3:   // (5 to 1, 1 to 2, 2 to 5)
        // Front face is untouched. (front 2 corners, front/left 2 edges) Rotates chunks of 2 clockwise
        //NOTE: CORNERS ARE AFFECTED by this too.  // 3 repeats = undo
    { 8, "r u R' F', r  u  R' U', R' f r2 U' R'"},       //  0,1,1,-2,0

    // #7Last-Layer: Step 2/3 - Corner+Edge Permutation 5: 
        //(5 and 3 swap, 1 and 2 swap) //1 and 5, 4 and 2
        //NOTE: CORNERS ARE AFFECTED by this edge algo,   //1 Repeat = Undo            
    { 9, "l r u2, L' u R', l U' r u2, L' u2 R'"},        //  4,2,0,-2,-4

    // #7Last-Layer: Step 2/3 : main
        // Rotates the far 3 Star/Edge pieces into their correct position + (Affects corners too)
        //The nearest 1 corner and 2 edges stay the same (Corner 1 and Edge 1/5 remain untouched)
        // (The 6 and 8 o'clock pieces will remain unaffected and in their same position)
        //The remaining 3 will rotate cyclicly in an Anti Clockwise fashion. (same color)
        //#2. Two Edges Solved: (Solved edge in the front & lower left) 
    {10, "r u R' u, r 2U' R'"},                           //  0,2,-1,-1,0

    // #7Last-Layer: Step 2/3: , continued
        //#1. Two Edges Solved : (Solved edge in the front & upper right)
        //#3. One Edge is Permuted : (Permuted edge in the front) this algo + then go back do the previous algo)
    {11, "r u2, R' u, r u2, R'"},                        //  0,3,0,-2,-1

    // #7Last-Layer: Edge Permutation 1+:  //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
        //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others Counter-Clockwise
        // 13 moves * Repeated 5 times = Total 65 moves.
    {12, "r2 U2' R2' U' r2 U2' R2'", 5},                 //  0,3,0,-2,-1

    // #7Last-Layer: Edge Permutation 2-: (opposite of previous; all the "up"s get reversed)
        //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others ClockWise
        // 13 moves * Repeated 5 times = Total 65 moves.
    {13, "r2 u2 R2' u r2 u2 R2'", 5},                    //  0,2,0,1,-3

    // #7Last-Layer: Edge Permutation 3a-:  //11 o'clock to 4 o'clock, 4 o'clock to 1 o'clock, 1 o'clock to 11 o'clock        
        // opposite of the previous one #3 above , but corners aren't affected...
        //Unaffecteds(2) = stay on front/left sides       
    {14, "r u R' u,  R' U' r2 U',  R' u R' u,  r U2'"},  //  0,2,-1,-1,0

    // #7Last-Layer: Edge Permutation 3b+: 
        //Opposite of EdgePermutation 3a-(if cube is rotated 2 turns CW). Reverses #3a- only when 3 edges are positioned in the front row.
        // manually reverse engineered from 4, to be equal to #3 but without affecting corners.
        //Unaffecteds(2) = stay on both/back sides. Cycles edges in the opposite rotation.
    {15, "r' u' r u', r u r2' u, r u' r u', r' u2"},     //  1,3,0,0,-4

    // #7Last-Layer: Edge Permutation 3c+: 
        // Reverses 3a if cube is rotated 2 turns CCW. //Identical Twin to 3b but Rotates the front face instead of right
        //Unaffecteds(2) = right/back side untouched. edges cycle rotate = clockwise
    {16, "f' u' f u', f u f2' u, f u' f u', f' u2"},     //  3,0,0,1,-4

    // #7Last-Layer: Edge 5-way star cycle+ all,  
        //Opposites Clockwise, 1 to 4, 4 to 2, 2 to 5, 5 to 3, 3 to 1 //1,3,5,2,4
        //60 moves total. copied from cube manual (turned upside down).
    {17, "l' u2 r u2' l u2 r' ", 6},                      //  2,2,2,-3,-3

    // #7Last-Layer: Edge 5-way star cycle- all,  // Two halves //2,1,3,4,5
        //60 moves total. (copied from cube manual)
    {18, "r' l u' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u' r l'"}, // 4,-1,1,-2,-2

    // #7Last-Layer: Edge two adjacent swaps, 
        //Safe Edge = FRONT. then swap 2&3 and 4&5
        //44 moves total. (copied from manual. )
    {19, "u2' l2' u2' l2 u' l2' u2' l2 u', l2' u2' l2 u' l2' u2' l2 u', l2' u2' l2 u' l2' u2' l2 u "},  // 0,1, -1, 1, -1

    // #7Last-Layer: Edge opposite swap & Invert, 
        //Safe Edge = FRONT. then swap 2&4 and 3&5=INVERTED.
        //right/backLeft swap and left/backRight swap + and INVERTS @ 8 o'clock and 1 o'clock <-- 
        //30 moves total. (copied from manual.)
    {20, "r' l f2' r l' u2 r' l, f' r l' u2' r' l f2' r, l' u2 r' l f' r l' u2'"},   // 0 , 2, 2, -2, -2

    // #2nd-Layer Edges(LEFT) =  7 o'clock to 9 o'clock:
        //copied right algo then reverse engineered it myself back to the left
    {21, "dl l dl l' dl' f' dl' f"},

    // #2nd-Layer Edges(RIGHT) =  5 o'clock to 3 o'clock:
        //Algo from QJ cube manual & (White face on top) (Exact opposite above)
    {22, "dr' r' dr' r dr f dr f'"},

    // #4th-Layer Edges(LEFT), (between the middle W), fourthLayerEdgesA() // 12 o'clock to 7 o'clock
        //Cube must have gray face on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
    {23, "F' R', F' F', r f"},

    // #4th-Layer Edges(RIGHT), (between the middle W), fourthLayerEdgesB() //12 o'clock to 5 o'clock.
        //Cube must have gray face on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
    {24, "f l, f f, L' F'"},

    // #6th-Layer Edges(LEFT) //Must have Layers 1-5 solved, and 7th layer is affected.
        // swap edge from face's star at 12 o'clock to Flop in (pinned to center) To the edge @  9 o'clock
    {25, "U' L' u l u f U' F'"},

    // #6th-Layer Edges(RIGHT) //opposite of previous, To the edge @ 3 o'clock
    {26, "u r U' R' U' F' u f"},

    // #7LL Clockwise Cycle Corners (very necessary, didnt have) Safe Area = Right
//TODO Should be moved higher in the list and dupes removed.
    {27, "L' u2 R U'2 , L u2 R' U'2"},

    // #7LL CounterClockwise Cycle Corners (already described as Algo #6/#7) Safe Area = Left
    {28, "R U'2 L' u2 , R' U'2 L u2"},

    // #7LL: Edge two swaps (Invert 4 in place),front=safe.
        //(~57 moves total.) (copied from manual.) similar to #20
    {29, "r' l f2' r l' u2 r' l,  f' r l' u2' r' l f2' r,  l' u2 r' l f' r l' u,  r' l f' r l' u2 r' l,  f2' r l' u2' r' l f' r,  l' u2 r' l f2' r l' "},

    // #7LL: Edge Permutation 1:  //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
        //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others Counter-Clockwise
        // 13 moves * Repeated 5 times = Total 65 moves.
    { 30, "r2 U2' R2' U' r2 U2' R2'", 1 },                 //  0,3,0,-2,-1

    // #7LL: Edge Permutation 2: (opposite of previous; all the "up"s get reversed)
        //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others ClockWise
        // 13 moves * Repeated 5 times = Total 65 moves.
    { 31, "r2 u2 R2' u r2 u2 R2'", 1 },                    //  0,2,0,1,-3

};

#endif
