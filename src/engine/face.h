#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include <vector>
#include <variant>
#include <type_traits>

class Face : public Piece {
public:
    Face();
    virtual ~Face() = default;

    Center *center;
    Corner *corner[5];
    Edge   *edge[5];

   template <typename T>
    Piece* getFacePiece(int i) const{
        if (std::is_same<T, Edge>::value)
            return edge[i];
        else if (std::is_same<T, Corner>::value)
            return corner[i];
        return center;
    };

    void initAxis(int n);
    bool render();
    void rotate(int direction);
    bool placeParts(int direction);
    int getNum() const { return thisNum; }

    std::vector<int> defaultCorners;
    std::vector<int> defaultEdges;

    void attachCenter(Center* a, double* centerVertexBase);
    void attachCornerPieces(Corner& n);
    void attachEdgePieces(Edge& n);

    std::vector<int> findPiecesOfFace(Piece& pieceRef, int times) const;
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
    void QuadSwapPieces(int const pack[8]);
    void QuadSwapCorners(int const pack[8]);
    void QuadSwapEdges(int const pack[8]);

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
constexpr static int CCW11E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };
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
};

constexpr AlgoString g_AlgoStrings[35] = {
    {0, ""},
    //AlgoStrings
    { 1, "r u R' U'"},
    { 2, "l u L' U'"},
    { 3, "U' L' u l"},
    { 4, "u r U' R'"},
    { 5, "R' DR' R DR"},
    //{ 5, "R' DR' R DR", 2},
    //{ 5, "R' DR' R DR", 4},
    { 6, "u l U' R' u L' U' r"},
    { 7, "u r 2U' L' 2u R' 2U' l u"},                         
    { 8, "r u R' F', r  u  R' U', R' f r2 U' R'"},       //  0,1,1,-2,0
    { 9, "l r u2, L' u R', l U' r u2, L' u2 R'"},        //  4,2,0,-2,-4
    {10, "r u R' u r 2U' R'"},                           //  0,2,-1,-1,0
    {11, "r u2, R' u, r u2, R'"},                        //  0,3,0,-2,-1
    {12, "r2 U2' R2' U' r2 U2' R2'", 5},                 //  0,3,0,-2,-1
    {13, "r2 u2 R2' u r2 u2 R2'", 5},                    //  0,2,0,1,-3
    {14, "r u R' u,  R' U' r2 U',  R' u R' u,  r U2'"},  //  0,2,-1,-1,0
    {15, "r' u' r u', r u r2' u, r u' r u', r' u2"},     //  1,3,0,0,-4
    {16, "f' u' f u', f u f2' u, f u' f u', f' u2"},     //  3,0,0,1,-4
    {17, "l' u2 r u2' l u2 r'", 6},                      //  2,2,2,-3,-3
    {18, "r' l u' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u' r l'"}, // 4,-1,1,-2,-2  
    {19, "u2' l2' u2' l2 u' l2' u2' l2 u', l2' u2' l2 u' l2' u2' l2 u', l2' u2' l2 u' l2' u2' l2 u "},  // 0,1, -1, 1, -1
    {20, "r' l f2' r l' u2 r' l, f' r l' u2' r' l f2' r, l' u2 r' l f' r l' u2'"},   // 0 , 2, 2, -2, -2
    {21, "dl l dl l' dl' f' dl' f"},
    {22, "dr' r' dr' r dr f dr f'"},
    {23, "F' R', F' F', r f"},
    {24, "f l, f f, L' F'"},
    {25, "U' L' u l u f U' F'"},
    {26, "u r U' R' U' F' u f"}
};

#endif
