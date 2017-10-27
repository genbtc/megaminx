#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"

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
	void initEdge(Edge*, Edge*, Edge*, Edge*, Edge*);
	void initCorner(Corner*, Corner*, Corner*, Corner*, Corner*);
	void initCenter(Center*);
    void initAxis(int n);
	void initNum(int);

    bool render();
	void rotate(int);
	void placeParts(bool right);

    void swapCorners(int, int);
    void swapEdges(int, int);


	Corner *corner[5];
	Edge *edge[5];
	Center *center;

private:
    void twoEdgeFlips(int a,int b);
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