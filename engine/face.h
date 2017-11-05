#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include "../common_physics/camera.h"
#include <vector>

enum FaceTurnDir { Clockwise, CounterClockwise };
struct eightPack
{
    int eight[8];
};
struct fourPack
{
    int four[4];
};
struct fivePack
{
	int five[5];
};
class Face : public Piece
{
public:
	Face();
	~Face() {}
    void initEdge(Edge& n, int num);
    void initCorner(Corner& n, int num);

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

    Corner *corner[5];
	Edge *edge[5];
	Center *center;

private:
    void twoEdgesFlip(int a,int b);
    void genericFlip(int a, int b, int c, int d, fourPack pack);
    void inwardsFlip(int a, int b,int c, int d);
    void backwardsFlip(int a, int b, int c, int d);
    void forwardsFlip(int a, int b, int c, int d);
    void alternatingBackwardsFlip(int a, int b, int c, int d);
    void alternatingForwardsFlip(int a, int b, int c, int d);
    void outwardsFlip(int a, int b, int c, int d);
    void QuadSwapCorners(eightPack pack);
    void QuadSwapEdges(eightPack pack);

    int turnDir;
	bool _rotate;
	double angle;
	double axis[3];
	int thisNum;

};

#endif