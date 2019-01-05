#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"
#include <queue>
#include <cassert>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <locale>

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
    void undoDouble();
    void scramble();
    void setCurrentFaceActive(int i);
    void flipCornerColor(int i, int x);
    void flipEdgeColor(int i, int x);
    std::vector<int> findCorners(int i);
    std::vector<int> findEdges(int i);
    void resetFace(int n);
    int resetFacesCorners(int color_n);
    int resetFacesEdges(int color_n);
    int resetFacesCorners(int color_n, const std::vector<int> &loadNewCorners, bool solve=true);
    int resetFacesEdges(int color_n, const std::vector<int> &loadNewEdges, bool solve=true);
    std::vector<int> getAllCornerPiecesColorFlipStatus();
    std::vector<int> getAllEdgePiecesColorFlipStatus();
    int LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors);
    int LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors);
    void rotateAlgo(int n, int i);
    const std::vector<numdir> ParseAlgorithmString(std::string algorithmString, colordirs loc);
    std::vector<int> findEdgeByPieceNum(const int indexes[5]);
    std::vector<int> findEdgeByPieceNum(std::vector<int> &v);
    void resetFiveEdges(const int indexes[5]);
    void resetFiveEdges(std::vector<int> &v);
    std::vector<int> findCornerByPieceNum(const int indexes[5]);
    std::vector<int> findCornerByPieceNum(std::vector<int>& v);
    void resetFiveCorners(const int indexes[5]);
    void resetFiveCorners(std::vector<int>& v);
    void secondLayerEdges();
    void fourthLayerEdges();
    void sixthLayerEdges();
    void lowYmiddleW();
    void highYmiddleW();
    void resetQueue();
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

    std::queue<numdir> rotateQueue;
    std::queue<numdir> undoQueue;
    void _rotate_internal(numdir i);
    void _rotate_internal(int num, int dir);
    void rotateBulk(std::queue<numdir> &bulkQueue);
    
};

int getCurrentFaceFromAngles(int x, int y); //defined as extern free function in megaminx.cpp for use in main.cpp
#endif
