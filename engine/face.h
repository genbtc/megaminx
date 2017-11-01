#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include "../common_physics/input.h"

enum FaceTurnDir { Clockwise, CounterClockwise };
struct eightPack
{
    int eight[8];
};
struct fourPack
{
    int four[4];
};
class Face
{
public:
	Face();
    void initEdge(Edge& n);
    void initEdge(Edge* n, int a, int b, int c, int d, int e);
    void initCorner(Corner* n, int a, int b, int c, int d, int e);
    void initCenter(Center*);
    void initAxis(int n);
	void initNum(int);

    bool render();
	void rotate(int);
    bool placeParts(int right);

    void swapCorners(int, int);
    void swapEdges(int, int);

	/** test ray collision against points, returns true when found collision and
	 * in "id" there is id of the point (point wth rad represents little sphere) that collides with the ray.
	 * if more than one collision is found then "id" points to the closest to "start" point */
	double m_pos, m_radius;
    bool RayTest(const Vec3d &start, const Vec3d &end, const Face *pt, double *t, double epsilon=0) const
    {
	    pt = this;return true;
	}
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
    double _vertex[5][3];

	bool _rotate;
	double angle;
	double axis[3];
	int thisNum;
};

#endif