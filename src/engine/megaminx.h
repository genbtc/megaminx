#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.h"
#include <vector>
#include <stack>
#include <queue>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Megaminx {
public:
    Megaminx();
    ~Megaminx() = default;
    void solve();
    void initCornerPieces();
    void initEdgePieces();
    void initFacePieces();
    void renderAllPieces();
    void toggleInvisibility() {  invisible = !invisible;  };
    bool isInvisible() { return invisible; };
    void render();
    void undo();
    void undoDouble();
    void undoQuad();
    void scramble();
    void setCurrentFaceActive(int i);
    void flipCornerColor(int i, int x);
    void flipEdgeColor(int i, int x);
    std::vector<int> findCorners(int i);
    std::vector<int> findEdges(int i);
    void rotate(int, int);
    bool isRotating;
    void rotateAlgo(int n, int i);
    void rotateBulkAlgoVector(std::vector<numdir> &bulk);
    void rotateBulkAlgoString(std::string algoString);
    void rotateBulkAlgoString(std::string algoString, const colordirs & loc);
    void resetQueue();
    void resetFace(int n);
    int resetFacesCorners(int color_n);
    int resetFacesEdges(int color_n);
    int resetFacesCorners(int color_n, const std::vector<int> &loadNewCorners, bool solve=true);
    int resetFacesEdges(int color_n, const std::vector<int> &loadNewEdges, bool solve=true);
    std::vector<int> getAllCornerPiecesColorFlipStatus();
    std::vector<int> getAllEdgePiecesColorFlipStatus();
    int LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors);
    int LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors);
    void LoadNewCornersFromOtherCube(Megaminx* source);
    void LoadNewEdgesFromOtherCube(Megaminx* source);
    const std::vector<numdir> ParseAlgorithmString(std::string algorithmString, colordirs loc);
    int findEdge(int pieceNum);
    std::vector<int> findEdgePieces(const int pieceNums[5]);
    std::vector<int> findEdgePieces(std::vector<int> &v);
    void resetFiveEdges(const int indexes[5]);
    void resetFiveEdges(std::vector<int> &v);
    int findCorner(int pieceNum);
    std::vector<int> findCornerPieces(const int pieceNums[5]);
    std::vector<int> findCornerPieces(std::vector<int> &v);
    void resetFiveCorners(const int indexes[5]);
    void resetFiveCorners(std::vector<int> &v);
    void secondLayerEdges();
    void fourthLayerEdges();
    void sixthLayerEdges();
    void lowYmiddleW();
    void highYmiddleW();
    void DetectSolvedWhiteEdgesUnOrdered(bool piecesSolved[5]);
    void DetectSolvedWhiteCornersUnOrdered(bool piecesSolved[5]);
    void rotateSolveWhiteEdges(Megaminx* shadowDom);
    bool shadowMultiRotate(int face, int &offby, Megaminx* shadowDom);
    void rotateSolveWhiteCorners(Megaminx * shadowDom);
    void rotateSolveLayer2Edges(Megaminx * shadowDom);
    void movePieceByRotatingIt(int source, int dest, bool corner=false);    
    
    Face* g_currentFace;    //tracks active face, set by setCurrentFaceActive()
    static const int numFaces = 12;
    static const int numCorners = 20;
    static const int numEdges = 30;    
    
private:
    Face   faces[numFaces];
    Center centers[numFaces];
    Corner corners[numCorners];
    Edge   edges[numEdges];

    int _rotatingFaceIndex;
    bool invisible = false;
    std::queue<numdir> rotateQueue;
    std::stack<numdir> undoStack;
    void _rotate_internal(numdir i);
    void shadowRotate(int num, int dir);
    std::queue<numdir> shadowRotateQueue;
};

int getCurrentFaceFromAngles(int x, int y); //defined as extern free function in megaminx.cpp for use in main.cpp
static int seenEdges[30] = { 0 };
static int seenCorners[20] = { 0 };
static int EdgeSwapCount = 0;
static int CornerSwapCount = 0;
void serializeVectorInt(std::vector<int> list1, std::string filename);
#endif
