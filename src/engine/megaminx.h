#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"
#include <queue>

class Megaminx {
public:
    Megaminx();
    ~Megaminx() = default;
    void solve();
    void initCornerPieces();
    void initEdgePieces();
    void initFacePieces();
    void renderAllPieces();
    void render();
    void rotate(int, int);
    void undo();
    void scramble();
    void setCurrentFaceActive(int i);
    void swapOneCorner(int i, int x);
    void swapOneEdge(int i, int x);
    std::vector<int> findCorners(int i);
    std::vector<int> findEdges(int i);
    void resetFace(int n);
    int resetFacesCorners(int n);
    int resetFacesEdges(int n);    
    void rotateAlgo(int n, int i);
    /* face busy rotating when true */
    bool isRotating;
    /* tracks active face, set by setCurrentFaceActive() */
    Face* g_currentFace;
    /* static numbers at compile time*/
    static const int numFaces = 12;
    static const int numCorners = 20;
    static const int numEdges = 30;    
    
private:
    Face   faces[numFaces];
    Center centers[numFaces];
    Corner corners[numCorners];
    Edge   edges[numEdges];

    int _rotatingFaceIndex;
    struct numdir {
        int num;
        int dir;
    };
    std::queue<numdir> rotateQueue;
    std::queue<numdir> undoQueue;
    void _rotate_internal(numdir i);
    void _rotate_internal(int num, int dir);
};

int getCurrentFaceFromAngles(int x, int y); //defined as extern free function in megaminx.cpp for use in main.cpp
#endif
