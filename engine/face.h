#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include <vector>

//Named Flip Direction lists:
constexpr int FlipInwards[4] = { 0, 1, 1, 0 };
constexpr int FlipOutwards[4] = { 1, 0, 0, 1 };
constexpr int FlipBackwards[4] = { 0, 0, 1, 1 };
constexpr int FlipForwards[4] = { 1, 1, 0, 0 };
constexpr int FlipAlternatingBackwards[4] = { 0, 1, 0, 1 };
constexpr int FlipAlternatingForwards[4] = { 1, 0, 1, 0 };

class Face : public Piece
{
public: enum TurnDir  { Clockwise = -1, None = 0, CounterClockwise = 1 };
public: enum TurnDir2 { CW = -1, CCW = 1 };
public:
	Face();
    ~Face() override {}
    int thisNum;
    int turnDir;
    bool rotating;
    Center *center;
    Corner *corner[5];
    Edge   *edge[5];

    std::vector<int> cornerNativePos;
    std::vector<int> cornerColorPos;
    std::vector<int> edgeNativePos;
    std::vector<int> edgeColorPos;
    void makeEdgePositionArray();
    void makeCornerPositionArray();	
    std::vector<int> findPiece(Piece& pieceRef, int times) const;
    void attachEdgePieces(Edge& n, int numEdges);
    void attachCornerPieces(Corner& n, int numCorners);
    void attachCenter(Center* a, double* centerVertexBase);
	void attachCenter(Center*);    

    void initAxis(int n);
    bool render();
	void rotate(int direction);
    bool placeParts(int direction);

    void swapCorners(int, int);
    void swapEdges(int, int);

private:
    void twoEdgesFlip(int a,int b);
    void Flip(int a, int b, int c, int d, const int* pack);
    void QuadSwapCorners(std::vector<int> pack);
    void QuadSwapEdges(std::vector<int> pack);

	double angle;
	double axis[3];
};

#endif