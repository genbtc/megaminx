#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include "../common_physics/camera.h"
#include <vector>

enum FaceTurnDir { Clockwise, CounterClockwise };

class Face : public Piece
{
public:
	Face();
    void makeEdgePositionArray();
    ~Face() {}
	//void initPiece(Piece& in, int num, Piece* output);
    void attachEdgePieces(Edge& n, int numEdges);
    void makeCornerPositionArray();
    void attachCornerPieces(Corner& n, int numCorners);

    std::vector<int> returnPositionalArray(Piece*& pieceRef);
    void attachCenter(Center* a, double* centerVertexBase);
	void attachCenter(Center*);    
    void initAxis(int n);
	std::vector<int> findPiece(Piece& n, int times) const;
    bool render();
	void rotate(int direction);
    bool placeParts(int direction);

    void swapCorners(int, int);
    void swapEdges(int, int);
	double m_pos, m_radius;
    bool RayTest(const Vec3d& start, const Vec3d& end, Vec3d* pt, double* t, double epsilon=0.0001);
    bool RayPlaneIntersection(Vec3d normal, Vec3d ray);

	std::vector<int> cornerNativePos;
	std::vector<int> cornerColorPos;
	std::vector<int> edgeNativePos;
	std::vector<int> edgeColorPos;
    Corner *corner[5];	
	Edge *edge[5];
	Center *center;

private:
    void twoEdgesFlip(int a,int b);
    void Flip(int a, int b, int c, int d, std::vector<int> pack);
    void QuadSwapCorners(std::vector<int> pack);
    void QuadSwapEdges(std::vector<int> pack);

    int turnDir;
	bool _rotate;
	double angle;
	double axis[3];
	int thisNum;

};

#endif