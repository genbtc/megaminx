#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"

class Megaminx
{
public:
    Megaminx();
	~Megaminx() = default;
    void solve();
    void initFacePieces();
    void render();
    void rotate(int, int);
    void undo();
    void scramble();
    void swapOneCorner(int i, int x);
    void swapOneEdge(int i, int x);
    void setCurrentFace(int i);
    int resetFace(int n);
    std::vector<int> findEdges(int i);
    std::vector<int> findCorners(int i);
    int grayEdges(int n);
    int grayCorners(int n);

    /** test ray collision against points, returns true when found collision and
	 * in "id" there is id of the point (point wth rad represents little sphere) that collides with the ray.
	 * if more than one collision is found then "id" points to the closest to "start" point */
    bool RayTest(const Vec3d& start, const Vec3d& end, unsigned* id, double* t, double epsilon);
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