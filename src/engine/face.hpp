#ifndef __FACE_H__
#define __FACE_H__

#include "center.hpp"
#include "edge.hpp"
#include "corner.hpp"
#include <vector>
#include <type_traits>

class Megaminx; //forward declare for Attach

class Face : public Piece {
public:
    Face();
    virtual ~Face() = default;
\
    Center *center = {};
    Corner *corner[5] = {};
    Edge   *edge[5] = {};

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
    }
   template<typename T>
    std::vector<int> findPiecesOrder() const;
    std::vector<int> findCornersOrder() const;
    std::vector<int> findEdgesOrder() const;
   template<typename T>
    std::vector<int> findPiecesColorFlipStatus() const;
    std::vector<int> findCornersColorFlipStatus() const;
    std::vector<int> findEdgesColorFlipStatus() const;

   template<typename T>
    int find5PiecePresent(int pieceNum) const;
    int find5EdgePresent(int pieceNum) const;
    int find5CornerPresent(int pieceNum) const;

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
    //hold a pointer back to the parent megaminx.
    Megaminx *megaminx;
public:
    enum TurnDir { Clockwise = -1, None = 0, CounterClockwise = 1 };
    enum TurnDir2 { CW = -1, CCW = 1 };
};

//Named Flip Direction lists:
constexpr int FlipInwards[4] =     { 0, 1, 1, 0 };
constexpr int FlipOutwards[4] =    { 1, 0, 0, 1 };
constexpr int FlipBackwards[4] =   { 0, 0, 1, 1 };
constexpr int FlipForwards[4]   =  { 1, 1, 0, 0 };
constexpr int FlipBackwardAlt[4] = { 0, 1, 0, 1 };
constexpr int FlipForwardAlt[4]  = { 1, 0, 1, 0 };

//These are invoked when Face::placeParts() is ran, when it's rotating.
//Called from Face::render(), only when something is moved, NEVER on startup.
//Flip direction lists for PlaceParts: //CounterClockwise CORNERS
//CCW Corners
constexpr int  CCW0C[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr int  CCW1C[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
constexpr int  CCW2C[8] = { 0, 1, 0, 2, 2, 3, 2, 4 };
constexpr int  CCW3C[8] = { 3, 4, 1, 3, 1, 2, 0, 1 };
constexpr int  CCW4C[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr int  CCW5C[8] = { 1, 3, 1, 4, 1, 2, 0, 1 };
constexpr int  CCW6C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr int  CCW7C[8] = { 1, 3, 3, 4, 4, 2, 2, 0 };
constexpr int  CCW8C[8] = { 4, 3, 4, 2, 4, 0, 4, 1 };
constexpr int  CCW9C[8] = { 4, 3, 4, 2, 4, 0, 4, 1 };
constexpr int CCW10C[8] = { 4, 3, 4, 2, 4, 0, 4, 1 };
constexpr int CCW11C[8] = { 4, 2, 4, 3, 3, 1, 1, 0 };
//Flip direction lists for PlaceParts: //Clockwise CORNERS
//CW Corners
constexpr int  CW0C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr int  CW1C[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr int  CW2C[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
constexpr int  CW3C[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
constexpr int  CW4C[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
constexpr int  CW5C[8] = { 0, 1, 1, 2, 1, 4, 1, 3 };
constexpr int  CW6C[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr int  CW7C[8] = { 2, 0, 4, 2, 3, 4, 1, 3 };
constexpr int  CW8C[8] = { 4, 1, 4, 0, 4, 2, 4, 3 };
constexpr int  CW9C[8] = { 4, 1, 4, 0, 4, 2, 4, 3 };
constexpr int CW10C[8] = { 4, 1, 4, 0, 4, 2, 4, 3 };
constexpr int CW11C[8] = { 1, 0, 3, 1, 4, 3, 2, 4 };
//Flip direction lists for PlaceParts: //CounterClockwise Edges
//CCW Edges
constexpr int  CCW0E[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr int  CCW1E[8] = { 4, 1, 1, 3, 0, 1, 0, 2 };
constexpr int  CCW2E[8] = { 1, 0, 1, 2, 1, 3, 3, 4 };
constexpr int  CCW3E[8] = { 3, 2, 4, 3, 0, 1, 1, 2 };
constexpr int  CCW4E[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
constexpr int  CCW5E[8] = { 2, 4, 2, 3, 0, 2, 0, 1 };
constexpr int  CCW6E[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr int  CCW7E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };
constexpr int  CCW8E[8] = { 0, 1, 1, 2, 2, 4, 3, 4 };
constexpr int  CCW9E[8] = { 0, 1, 1, 2, 2, 4, 3, 4 };
constexpr int CCW10E[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
constexpr int CCW11E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };
//Flip direction lists for PlaceParts: //Clockwise Edges
//CW Edges
constexpr int  CW0E[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr int  CW1E[8] = { 0, 2, 0, 1, 1, 3, 4, 1 };
constexpr int  CW2E[8] = { 3, 4, 1, 3, 1, 2, 1, 0 };
constexpr int  CW3E[8] = { 1, 2, 0, 1, 4, 3, 3, 2 };
constexpr int  CW4E[8] = { 3, 4, 1, 3, 1, 2, 0, 1 };
constexpr int  CW5E[8] = { 0, 1, 0, 2, 2, 3, 2, 4 };
constexpr int  CW6E[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
constexpr int  CW7E[8] = { 0, 1, 0, 2, 0, 4, 0, 3 };
constexpr int  CW8E[8] = { 3, 4, 2, 4, 1, 2, 0, 1 };
constexpr int  CW9E[8] = { 3, 4, 2, 4, 1, 2, 0, 1 };
constexpr int CW10E[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr int CW11E[8] = { 0, 1, 0, 2, 0, 4, 0, 3 };

//Algorithms AlgoString moved to algorithms.cpp !
#endif
