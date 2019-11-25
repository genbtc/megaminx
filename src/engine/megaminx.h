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
    void undoBulk();
    void scramble();
    void setCurrentFaceActive(int i);
    Face* g_currentFace;    //tracks active face, set by setCurrentFaceActive()
    bool isRotating;
    void rotate(int num, int dir);    
    void rotateAlgo(int n, int i);
    void rotateBulkAlgoVector(std::vector<numdir> &bulk);
    void rotateBulkAlgoString(std::string algoString);
    void rotateBulkAlgoString(std::string algoString, const colordirs & loc);
    void resetQueue();
    void resetFace(int n);
   template <typename T>
    int resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve = true);
    int resetFacesCorners(int color_n, const std::vector<int> &loadNewCorners, bool solve = true);
    int resetFacesEdges(int color_n, const std::vector<int> &loadNewEdges, bool solve = true);
    int resetFacesCorners(int color_n);
    int resetFacesEdges(int color_n);
   template <typename T>
    void flipPieceColor(int face, int num);
    void flipCornerColor(int face, int num);
    void flipEdgeColor(int face, int num);
   template <typename T>
    std::vector<int> getAllPiecesColorFlipStatus();
    std::vector<int> getAllCornerPiecesColorFlipStatus();
    std::vector<int> getAllEdgePiecesColorFlipStatus();
   template <typename T>
    int LoadNewPiecesFromVector(const std::vector<int>& readPieces, const std::vector<int>& readPieceColors);
    int LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors);
    int LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors);
    void LoadNewCornersFromOtherCube(Megaminx* source);
    void LoadNewEdgesFromOtherCube(Megaminx* source);
    const std::vector<numdir> ParseAlgorithmString(std::string algorithmString, colordirs loc);
   template <typename T>
    int findPiece(int pieceNum);
    int findEdge(int pieceNum);
    int findCorner(int pieceNum);
   template <typename T>
    std::vector<int> findPieces(int i);
    std::vector<int> findCorners(int i);
    std::vector<int> findEdges(int i);
   template <typename T>
    std::vector<int> findFivePieces(const int pieceNums[5]);
    std::vector<int> findEdgePieces(const int pieceNums[5]);
    std::vector<int> findCornerPieces(const int pieceNums[5]);
   template <typename T>
    std::vector<int> findFivePieces(std::vector<int> &v);
    std::vector<int> findEdgePieces(std::vector<int> &v);
    std::vector<int> findCornerPieces(std::vector<int> &v);
   template<typename T>
    void resetFivePieces(const int indexes[5]);
    void resetFiveEdges(const int indexes[5]);
    void resetFiveCorners(const int indexes[5]);
   template <typename T>
    void resetFivePieces(std::vector<int> &v);
    void resetFiveEdges(std::vector<int> &v);
    void resetFiveCorners(std::vector<int> &v);

    void Megaminx::secondLayerEdges() {        
        resetFiveEdges(m_secondLayerEdges);
    }
    void Megaminx::fourthLayerEdges() {        
        resetFiveEdges(m_fourthLayerEdgesA);        
        resetFiveEdges(m_fourthLayerEdgesB);
    }
    void Megaminx::sixthLayerEdges() {        
        resetFiveEdges(m_sixthLayerEdges);
    }
    void Megaminx::lowYmiddleW() {        
        resetFiveCorners(m_secondLayerEdges);
    }
    void Megaminx::highYmiddleW() {        
        resetFiveCorners(m_fourthLayerEdgesA);
    }

    void DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5]);
   template <typename T>
    void DetectSolvedPieces(int startI, bool piecesSolved[5]);
    void DetectSolvedCorners(int startI, bool piecesSolved[5]);
    void DetectSolvedEdges(int startI, bool piecesSolved[5]);
    void rotateSolveWhiteEdges(Megaminx* shadowDom);
    void rotateSolveWhiteCorners(Megaminx* shadowDom);
    void rotateSolveLayer2Edges(Megaminx* shadowDom);
    void rotateSolve3rdLayerCorners(Megaminx* shadowDom);
    void rotateSolveLayer4Edges(Megaminx* shadowDom);
    void rotateSolve5thLayerCorners(Megaminx* shadowDom);
    void rotateSolveLayer6Edges(Megaminx* shadowDom);
    void rotateSolveLayer7Edges(Megaminx * shadowDom);
    void testingAlgostrings(Megaminx* shadowDom);
    void updateRotateQueueWithShadow(Megaminx* shadowDom);
    bool shadowMultiRotate(int face, int &offby, Megaminx* shadowDom);
    int getRotateQueueNum() { return (int)rotateQueue.size(); }

    static const int numFaces = 12;
    static const int numCorners = 20;
    static const int numEdges = 30;

   template <typename T>
    const int getMaxNumberOfPieces() {
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
    void shadowRotate(int num, int dir);
    friend class LayerAssist;
    
};

extern int getCurrentFaceFromAngles(int x, int y); //defined as extern free function in megaminx.cpp for use in main.cpp
extern void serializeVectorInt(std::vector<int> list1, std::string filename);

#endif
