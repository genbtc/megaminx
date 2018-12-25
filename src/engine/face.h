#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include <vector>
#include <variant>
#include <type_traits>

//Named Flip Direction lists:
constexpr int FlipInwards[4] = { 0, 1, 1, 0 };
constexpr int FlipOutwards[4] = { 1, 0, 0, 1 };
constexpr int FlipBackwards[4] = { 0, 0, 1, 1 };
constexpr int FlipForwards[4] = { 1, 1, 0, 0 };
constexpr int FlipBackwardAlt[4] = { 0, 1, 0, 1 };
constexpr int FlipForwardAlt[4] = { 1, 0, 1, 0 };

class Face : public Piece {
public:
    enum TurnDir  { Clockwise = -1, None = 0, CounterClockwise = 1 };
    enum TurnDir2 { CW = -1, CCW = 1 };
public:
    Face();
    virtual ~Face() = default;

    Center *center;
    Corner *corner[5];
    Edge   *edge[5];

    std::vector<int> defaultCorners;
    std::vector<int> cornerColorPos;
    std::vector<int> defaultEdges;
    std::vector<int> edgeColorPos;

    std::vector<int> findPiece(Piece& pieceRef, int times) const;

    void attachEdgePieces(Edge& n);
    void attachCornerPieces(Corner& n);
    void attachCenter(Center* a, double* centerVertexBase);

    int Face::getNum() const { return thisNum; }
    void initAxis(int n);    
    bool render();
    void rotate(int direction);
    bool placeParts(int direction);
    void swapCorners(int a, int b);
    void swapEdges(int a, int b);

private:
    void TwoEdgesFlip(int a, int b);
    void FlipCorners(int a, int b, int c, int d, const int* pack);
    void QuadSwapCorners(int const pack[8]);
    void QuadSwapEdges(int const pack[8]);

    int thisNum;
    int turnDir;
    bool rotating;
    double angle;
    double axis[3];
};
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
constexpr static int CCW11C[8] = { 2, 4, 3, 4, 0, 1, 0, 3 };
//Flip direction lists for PlaceParts: //Clockwise CORNERS
//CW Corners
constexpr static int  CW0C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
constexpr static int  CW1C[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
constexpr static int  CW2C[8] = { 2, 4, 2, 3, 0, 2, 0, 1 };
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
#endif
