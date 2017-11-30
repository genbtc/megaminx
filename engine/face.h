#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include <vector>
#include "../common_physics/utils_math.h"

//Named Flip Direction lists:
constexpr int FlipInwards[4] = { 0, 1, 1, 0 };
constexpr int FlipOutwards[4] = { 1, 0, 0, 1 };
constexpr int FlipBackwards[4] = { 0, 0, 1, 1 };
constexpr int FlipForwards[4] = { 1, 1, 0, 0 };
constexpr int FlipAlternatingBackwards[4] = { 0, 1, 0, 1 };
constexpr int FlipAlternatingForwards[4] = { 1, 0, 1, 0 };

class Face : public Piece
{
public: enum { Clockwise = -1, None, CounterClockwise = 1 };
public: enum { CW = -1, CCW = 1 };
public:
	Face();
    ~Face() {}
    void makeEdgePositionArray();
    void makeCornerPositionArray();	
    void attachEdgePieces(Edge& n, int numEdges);
    void attachCornerPieces(Corner& n, int numCorners);
    std::vector<int> findPiece(Piece& pieceRef, int times) const;

    void attachCenter(Center* a, double* centerVertexBase);
	void attachCenter(Center*);    
    void initAxis(int n);
    bool render();
	void rotate(int direction);
    bool placeParts(int direction);

    void swapCorners(int, int);
    void swapEdges(int, int);
	double m_pos, m_radius;
    bool RayTest(const Vec3d& start, const Vec3d& end, Vec3d* pt, double* t, double epsilon=0.0001) const;
    bool RayPlaneIntersection(Vec3d normal, Vec3d ray);

	std::vector<int> cornerNativePos;
	std::vector<int> cornerColorPos;
	std::vector<int> edgeNativePos;
	std::vector<int> edgeColorPos;
    Corner *corner[5];	
	Edge *edge[5];
	Center *center;
	int thisNum;
    int turnDir;
    bool rotating;
private:
    void twoEdgesFlip(int a,int b);
    void Flip(int a, int b, int c, int d, const int* pack);
    void QuadSwapCorners(std::vector<int> pack);
    void QuadSwapEdges(std::vector<int> pack);

	double angle;
	double axis[3];
};

#endif