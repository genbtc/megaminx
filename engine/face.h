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
	~Face() {}
	void initEdge(Edge& n, int numEdges);
	void initCorner(Corner& n, int numCorners);
    //void initPiece(Piece& in, int num, Piece* output);

    std::vector<int> findPiece(Piece& n, int times) const;
    void initCenter(Center*);
    void initCenter(Center* a, double* centerVertexBase);
    void initAxis(int n);

    bool render();
	void rotate(int);
    bool placeParts(int right);

    void swapCorners(int, int);
    void swapEdges(int, int);
	double m_pos, m_radius;
    bool RayTest(const Vec3d& start, const Vec3d& end, Vec3d* pt, double* t, double epsilon=0.0001);
    bool RayPlaneIntersection(Vec3d normal, Vec3d ray);

	std::vector<int> cornerNativePos;
	std::vector<int> edgeNativePos;
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