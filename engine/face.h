#ifndef __FACE_H__
#define __FACE_H__

#include "center.h"
#include "edge.h"
#include "corner.h"
#include <bits/alloc_traits.h>

enum FaceTurnDir { Clockwise, CounterClockwise };

class Face
{
public:
	Face();
	void initEdge(Edge*, Edge*, Edge*, Edge*, Edge*);
	void initCorner(Corner*, Corner*, Corner*, Corner*, Corner*);
	void initCenter(Center*);
    void createAxis(int n, double* target);
    //static void createAxisbyStruct(int n, double* axis);
    void initAxis(int n);
	void initNum(int);
	~Face();


    bool render();
	void rotate(int);
	void placeParts(bool right);

    void swapCorners(int, int);
    void swapEdges(int, int);


	Corner *corner[5];
	Edge *edge[5];
	Center *center;

private:
    void edgeflip(int,int);
    void cornerInsideOutFlip(int,int,int,int);
    void cornerBackwardsFlip(int a, int b, int c, int d);
    void cornerForwardsFlip(int a, int b, int c, int d);
    void alternatingReverseFlip(int a, int b, int c, int d);
    void alternatingForwardFlip(int a, int b, int c, int d);
    void InwardsFlip(int a, int b, int c, int d);
    void QuadSwapCorners(std::tuple<int, int, int, int, int, int, int, int> pack);
    void QuadSwapEdges(std::tuple<int, int, int, int, int, int, int, int> pack);

    int turnDir;
    double _vertex[5][3];

	bool _rotate;
	double angle;
	double axis[3];
	int thisNum;
};

#endif