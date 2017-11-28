#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"


class Megaminx
{
public:
    Megaminx();
    ~Megaminx() = default;
    void solve();
    void initEdgeAndCornerPieces();    
    void initFacePieces();
    void renderAllPieces();
    void render();
    void rotate(int, int);
    void undo();
    void scramble();
    void swapOneCorner(int i, int x);
    void swapOneEdge(int i, int x);
    void setCurrentFaceActive(int i);
    void resetFace(int n);
    std::vector<int> findEdges(int i);
    std::vector<int> findCorners(int i);
    int resetFacesEdges(int n);
    int resetFacesCorners(int n);
    int getCurrentFaceFromAngles(int x, int y) const;
    void rotateAlgo(int current_face, int i);
    /* y axis */
	double y;
    /* x axis */
	double x;
	bool rotating;
	/* pointer */
    Face* g_currentFace;
	/* static numbers at compile time*/
	static const int numEdges = 30;
	static const int numCorners = 20;
	static const int numFaces = 12;
private:
	int _rSide;
    struct numdir
    {
        int num;
        int dir;
    };
    std::queue<numdir> rotateQueue;
    std::queue<numdir> undoQueue;

	Face faces[numFaces];
	Center centers[numFaces];
	Edge edges[numEdges];
	Corner corners[numCorners];
    void _rotate_internal(int num, int dir);
};

#endif