#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"

class Megaminx
{
public:
    Megaminx();
    void iterateAllPieces();
    ~Megaminx() = default;
    void solve();
    void initFacePieces();
    void render();
    void rotate(int, int);
    void undo();
    void scramble();
    void swapOneCorner(int i, int x);
    void swapOneEdge(int i, int x);
    void setCurrentFaceActive(int i);
    int resetFace(int n);
    std::vector<int> findEdges(int i);
    std::vector<int> findCorners(int i);
    int resetFacesEdges(int n);
    int resetFacesCorners(int n);
    /* y axis */
	double y;
    /* x axis */
	double x;
	bool rotating;
	/* pointer */
    Face* g_currentFace;
	/* number by reflection*/
    unsigned numEdges;
    unsigned numCorners;
    unsigned numFaces;
private:
	int _rSide;
	int undoCache[2];

	Face faces[12];
	Center centers[12];
	Edge edges[30];
	Corner corners[20];
};

#endif