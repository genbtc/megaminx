#ifndef __MEGAMINX_H__
#define __MEGAMINX_H__

#include "face.hpp"
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
    void undoBulk();
    void scramble();
    void setCurrentFaceActive(int i);
    Face* g_currentFace;    //tracks active face, set by setCurrentFaceActive()
    bool isRotating;
    void rotate(int num, int dir);
    void rotateAlgo(int face, int id);
    void rotateBulkAlgoVector(const std::vector<numdir> &bulk);
    void rotateBulkAlgoString(std::string algoString);
    void rotateBulkAlgoString(std::string algoString, const colordirs &loc);
    void resetQueue();
    void resetFace(int n);
   template <typename T>
    int resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve = true);
    int resetFacesPiecesEdges(int color_n, const std::vector<int> &defaultPieces, bool solve);
    int resetFacesPiecesCorners(int color_n, const std::vector<int> &defaultPieces, bool solve);
    int resetFacesCorners(int color_n, const std::vector<int> &loadNewCorners, bool solve = true);
    int resetFacesEdges(int color_n, const std::vector<int> &loadNewEdges, bool solve = true);
    int resetFacesCorners(int color_n);
    int resetFacesEdges(int color_n);
   template <typename T>
    void flipPieceColor(int face, int num);
    void flipCornerColor(int face, int num);
    void flipEdgeColor(int face, int num);
   template <typename T>
    std::vector<int> getAllPiecesPosition();
    std::vector<int> getAllCornerPiecesPosition();
    std::vector<int> getAllEdgePiecesPosition();
   template <typename T>
    std::vector<int> getAllPiecesColorFlipStatus();
    std::vector<int> getAllCornerPiecesColorFlipStatus();
    std::vector<int> getAllEdgePiecesColorFlipStatus();
    const std::vector<numdir> ParseAlgorithmString(std::string algorithmString, const colordirs &loc, int algo=0);
    const std::vector<numdir> ParseAlgorithmString(AlgoString algorithm, const colordirs &loc);
    const std::vector<numdir> ParseAlgorithmString(int algo, int startLoc);
   template <typename T>
    int findPiece(int pieceNum);
    int findEdge(int pieceNum);
    int findCorner(int pieceNum);
    std::vector<int> findPiecesOfFace(int face, Piece &pieceRef, int times) const;
   template <typename T>
    std::vector<int> findFacePiecesOrder(int face) const;
    std::vector<int> findFaceCornersOrder(int face) const;
    std::vector<int> findFaceEdgesOrder(int face) const;
   template <typename T>
    std::vector<int> findPieces(int face);
    std::vector<int> findCorners(int face);
    std::vector<int> findEdges(int face);
   template <typename T>
    std::vector<int> findFivePieces(const int pieceNums[5]);
    std::vector<int> findEdgePieces(const int pieceNums[5]);
    std::vector<int> findCornerPieces(const int pieceNums[5]);
   template <typename T>
    std::vector<int> findFivePieces(const std::vector<int> &v);
    std::vector<int> findEdgePieces(const std::vector<int> &v);
    std::vector<int> findCornerPieces(const std::vector<int> &v);
   template<typename T>
    void resetFivePieces(const int indexes[5]);
    void resetFiveEdges(const int indexes[5]);
    void resetFiveCorners(const int indexes[5]);
   template <typename T>
    void resetFivePiecesV(std::vector<int> &v);
    void resetFiveEdgesV(std::vector<int> &v);
    void resetFiveCornersV(std::vector<int> &v);

    constexpr static int m_firstLayerEdges[5]   = { 0, 1, 2, 3, 4 };
    constexpr static int m_secondLayerEdges[5]  = { 5, 6, 7, 8, 9 };
    constexpr static int m_fourthLayerEdgesA[5] = { 10, 11, 12, 13, 14 };
    constexpr static int m_fourthLayerEdgesB[5] = { 15, 16, 17, 18, 19 };
    constexpr static int m_sixthLayerEdges[5]   = { 20, 21, 22, 23, 24 };
    constexpr static int m_seventhLayerEdges[5] = { 25, 26, 27, 28, 29 };

    void secondLayerEdges() {
        resetFiveEdges(m_secondLayerEdges);
    }
    void fourthLayerEdges() {
        resetFiveEdges(m_fourthLayerEdgesA);
        resetFiveEdges(m_fourthLayerEdgesB);
    }
    void sixthLayerEdges() {
        resetFiveEdges(m_sixthLayerEdges);
    }
    void lowYmiddleW() {
        resetFiveCorners(m_secondLayerEdges);
    }
    void highYmiddleW() {
        resetFiveCorners(m_fourthLayerEdgesA);
    }
    //In Shadow.cpp
   template <typename T>
    int createMegaMinxFromShadowVec(const std::vector<int>& readPieces, const std::vector<int>& readPieceColors, Megaminx* shadowDom);
    int LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors, Megaminx* shadowDom);
    int LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors, Megaminx* shadowDom);

    //In Solve.cpp
    void DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5]);
    bool isFullySolved();
   template<typename T>
    bool DetectIfAllSolved();
   template <typename T>
    void DetectSolvedPieces(int startI, bool piecesSolved[5]);
    void DetectSolvedCorners(int startI, bool piecesSolved[5]);
    void DetectSolvedEdges(int startI, bool piecesSolved[5]);
    bool checkPieceMatches(const std::vector<int> &pieces, int a, int b, int c, int d, int e) const {
        return (pieces[0] == a && pieces[1] == b && pieces[2] == c && pieces[3] == d && pieces[4] == e);
    };
    void rotateSolveWhiteEdges(Megaminx* shadowDom);
    void rotateSolveWhiteCorners(Megaminx* shadowDom);
    void rotateSolveLayer2Edges(Megaminx* shadowDom);
    void rotateSolve3rdLayerCorners(Megaminx* shadowDom);
    void rotateSolveLayer4Edges(Megaminx* shadowDom);
    void rotateSolve5thLayerCorners(Megaminx* shadowDom);
    void rotateSolveLayer6Edges(Megaminx* shadowDom);
    void rotateSolveLayer7Edges(Megaminx* shadowDom);
    void rotateSolve7thLayerCorners(Megaminx* shadowDom);
    void testingAlgostrings(Megaminx* shadowDom);

    //in Shadow.CPP
    void LoadNewEdgesFromOtherCube(Megaminx* source);
    void LoadNewCornersFromOtherCube(Megaminx* source);
    void updateRotateQueueWithShadow(Megaminx* shadowDom);
    void bulkShadowRotate(std::vector<numdir> bulk);
    void shadowRotate(int num, int dir);
    void shadowRotate(numdir op);
    bool shadowMultiRotate(int face, int &offby);
    int getRotateQueueNum() const { return (int)rotateQueue.size(); }

    static const int numFaces = 12;
    static const int numCorners = 20;
    static const int numEdges = 30;

   template <typename T>
    const int getMaxNumberOfPieces() const {
        if (std::is_same<T, Edge>::value)
            return numEdges;
        else if (std::is_same<T, Corner>::value)
            return numCorners;
        return numFaces;
    };

   template <typename T>
    Piece* getPieceArray(int i) {
        if (std::is_same<T, Edge>::value)
            return &edges[i];
        else if (std::is_same<T, Corner>::value)
            return &corners[i];
        return &centers[i];
    };

private:
    Face   faces[numFaces];
    Center centers[numFaces];
    Corner corners[numCorners];
    Edge   edges[numEdges];

    int _rotatingFaceIndex;
    bool invisible = false;
    std::queue<numdir> rotateQueue;
    std::stack<numdir> undoStack;
    std::queue<numdir> shadowRotateQueue;
    friend class EdgeLayerAssist;
    friend class CornerLayerAssist;
};
static auto MM = [](int &over) { while (over >= 5) over -= 5; };
static auto MMge = [](int &over, megaminxColor stop) { while (over >= (int)stop) over -= 5; };
static auto MMg = [](int &over, megaminxColor stop) { while (over > (int)stop) over -= 5; };
static auto MMgeI = [](int &over, int stop) { while (over >= stop) over -= 5; };

#endif
