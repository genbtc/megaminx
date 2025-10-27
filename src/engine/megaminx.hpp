#include "face.hpp"
#include "algorithms.hpp"
#include <vector>
#include <stack>
#include <queue>
#include <cassert>
#include <algorithm>
#include <memory>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <array>

class Megaminx {
public:
    static const int numFaces = 12;
    static const int numCorners = 20;
    static const int numEdges = 30;
    Megaminx();
    ~Megaminx() = default;
    void initCornerPieces();
    void initEdgePieces();
    void initFacePieces();
private:
    Face   faces[numFaces];
    Center centers[numFaces];
    Corner corners[numCorners];
    Edge   edges[numEdges];
public:
    void renderAllPieces();
    void render();
    void undo();
    void undoDouble();
    void undoQuad();
    void undoBulk();
    void resetQueue();
    void scramble();
    void setCurrentFaceActive(int i);
    Face* g_currentFace;    //tracks active face, set by setCurrentFaceActive()
    bool invisible = false;
    void toggleInvisibility() {  invisible = !invisible;  } //unused
    bool isInvisible() const { return invisible; }          //unused
    //Rotate
    int _rotatingFaceIndex;
    bool isRotating;
    void rotate(int num, int dir);
    void rotateAlgo(int algoID, int face=-1);
    void rotateBulkAlgoVector(const std::vector<numdir> &bulk);
    void rotateBulkAlgoString(std::string algoString, const colordirs &loc, int algoID);
    //Reset
    void resetFace(int n);
    template <typename T> //used by deprecated save loader
    int resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve = false);
    int resetFacesPiecesEdge(int color_n, const std::vector<int> &defaultPieces, bool solve = false);
    int resetFacesPiecesCorner(int color_n, const std::vector<int> &defaultPieces, bool solve = false);
    int resetFacesCorners(int color_n, const std::vector<int> &loadNewCorners, bool solve = true);
    int resetFacesEdges(int color_n, const std::vector<int> &loadNewEdges, bool solve = true);
    int resetFaceCorners(int color_n);
    int resetFaceEdges(int color_n);
    //Flip
    template <typename T>
    void flipPieceColor(int face, int num);
    void flipCornerColor(int face, int num);
    void flipEdgeColor(int face, int num);
    //Get Query
    template <typename T>
    std::vector<int> getAllPiecesPosition();
    std::vector<int> getAllCornerPiecesPosition();
    std::vector<int> getAllEdgePiecesPosition();
    template <typename T>
    std::vector<int> getAllPiecesColorFlipStatus();
    std::vector<int> getAllCornerPiecesColorFlipStatus();
    std::vector<int> getAllEdgePiecesColorFlipStatus();
    //Parse
    const std::vector<numdir> ParseAlgorithmString(std::string algorithmString, const colordirs &loc, int algo=0);
    const std::vector<numdir> ParseAlgorithmString(AlgoString algorithm, const colordirs &loc);
    const std::vector<numdir> ParseAlgorithmID(int algoID, int startLoc);
    //Find Piece Functions  (see bottom of file for definition)
    int findPiece(int pieceNum);    //Find 1, a single numbered piece (by int 1-60)
    int findEdge(int pieceNum) { return findPiece<Edge>(pieceNum); }
    int findCorner(int pieceNum) { return findPiece<Corner>(pieceNum); }
    //Face Find Piece Functions
    std::vector<int> findPiecesOfFace(int face, Piece &pieceRef, int times) const;
   template <typename T>
    std::vector<int> findFacePiecesOrder(int face);
    std::vector<int> findFaceEdgesOrder(int face);
    std::vector<int> findFaceCornersOrder(int face);
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
    //Reset5 Functions
   template<typename T>
    void resetFivePieces(const int indexes[5]);
    void resetFiveEdges(const int indexes[5]);
    void resetFiveCorners(const int indexes[5]);
   template <typename T>
    void resetFivePieces(const std::vector<int> &v);
    void resetFiveEdges(const std::vector<int> &v);
    void resetFiveCorners(const std::vector<int> &v);
    //Well-defined piece lists - Edge layers
    constexpr static int m_firstLayerEdges[5]   = {  0, 1, 2, 3, 4 };
    constexpr static int m_secondLayerEdges[5]  = {  5, 6, 7, 8, 9 };
    constexpr static int m_fourthLayerEdgesA[5] = { 10, 11, 12, 13, 14 };
    constexpr static int m_fourthLayerEdgesB[5] = { 15, 16, 17, 18, 19 };
    constexpr static int m_sixthLayerEdges[5]   = { 20, 21, 22, 23, 24 };
    constexpr static int m_seventhLayerEdges[5] = { 25, 26, 27, 28, 29 };
    //Well-defined piece lists - Corner layers
    constexpr static int m_firstLayerCorners[5]   = {  0, 1, 2, 3, 4 };
    constexpr static int m_thirdLayerCorners[5]   = {  5, 6, 7, 8, 9 };
    constexpr static int m_fifthLayerCorners[5]   = { 10, 11, 12, 13, 14 };
    constexpr static int m_seventhLayerCorners[5] = { 15, 16, 17, 18, 19 };
    //Reset Layer functions
    void resetSecondLayerEdges()    { resetFiveEdges(m_secondLayerEdges);    }
    void resetFourthLayerEdges()    { resetFiveEdges(m_fourthLayerEdgesA);
                                      resetFiveEdges(m_fourthLayerEdgesB);   }
    void resetSixthLayerEdges()     { resetFiveEdges(m_sixthLayerEdges);     }
    void resetlowYmiddleWCorners()  { resetFiveCorners(m_thirdLayerCorners); }
    void resethighYmiddleWCorners() { resetFiveCorners(m_fifthLayerCorners); }

    //In Solve.cpp
    void DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5]);
    void DetectSolvedEdgesTwisted(int startI, bool piecesSolved[5]);
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
    void shadowBulkRotate(const std::vector<numdir> &bulk);
    void shadowRotate(int num, int dir);
    void shadowRotateND(numdir op);
    bool shadowMultiRotate(int face, int &offby);
    int getRotateQueueNum() const { return (int)rotateQueue.size(); }
    //current savefile algo
   template <typename T>
    int createMegaMinxFromShadowVec(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors, Megaminx* shadowDom);
    int LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors, Megaminx* shadowDom);
    int LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors, Megaminx* shadowDom);
    //old deprecated savefile algo
   template <typename T>
    int LoadNewPiecesFromVector(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors);
    int LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors);
    int LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors);
   template <typename T>
    constexpr int getMaxNumberOfPieces() const {
        if (std::is_same<T, Edge>::value)
            return numEdges;
        else if (std::is_same<T, Corner>::value)
            return numCorners;
        return numFaces;
    }

   template <typename T>
    Piece* getPieceArray(int i) {
        if (std::is_same<T, Edge>::value)
            return &edges[i];
        else if (std::is_same<T, Corner>::value)
            return &corners[i];
        return &centers[i];
    }

    //declaring here fixes LD unresolved symbol errors with findPiece<Edge> / findPiece<Corner>
   template <typename T>
    int findPiece(int pieceNum) {
        const Piece* piece = getPieceArray<T>(0);
        const int maxpcs = getMaxNumberOfPieces<T>();
        for (int i = 0; i < maxpcs; ++i)
            if (piece[i].data.pieceNum == pieceNum)
                return i;
        return -1;
    } //where T = Corner or Edge

   template <typename T>
    bool DetectIfAllSolved()
    {
        int allSolved = 0;
        const Piece* piece = getPieceArray<T>(0);
        const int maxpcs = getMaxNumberOfPieces<T>();
        for (int i = 0; i < maxpcs; ++i) {
            if (piece[i].data.pieceNum == i && piece[i].data.flipStatus == 0)
                allSolved++;
        } //if count is the same, solved true
        return (maxpcs==allSolved);
    } //where T = Corner or Edge

    //simple Function call to find out if the puzzle is fully solved.
    bool isFullySolved() { return (DetectIfAllSolved<Edge>() && DetectIfAllSolved<Corner>()); }

    //Generic template way to detect if pieces are solved, in their correct locations with correct colors, on one face
    template <typename T>
    void DetectSolvedPieces(int startI, bool piecesSolved[5])
    {
        const int endI = startI + 5;
        //Find out if any applicable startI-endI pieces are exactly in their slots.
        for (int p = startI; p < endI; ++p) {
            const int pIndex = findPiece<T>(p);
            const Piece* piece = getPieceArray<T>(pIndex);
            //make sure its startI-endI and make sure the colors arent flipped
            if (pIndex >= startI && pIndex < endI && p == pIndex && piece->data.flipStatus == 0)
                piecesSolved[p - startI] = true;
        }
    } //where T = Corner or Edge
    void DetectSolvedCorners(int startI, bool piecesSolved[5]) { DetectSolvedPieces<Corner>(startI, piecesSolved); }
    void DetectSolvedEdges(int startI, bool piecesSolved[5]) { DetectSolvedPieces<Edge>(startI, piecesSolved); }

    static bool checkPieceMatches(const std::vector<int> &pieces, int a, int b, int c, int d, int e) {
        return (pieces[0] == a && pieces[1] == b && pieces[2] == c && pieces[3] == d && pieces[4] == e);
    }

    //match 7th layer edge pieces to modby stat for algorithms, and rotate by startingFace
    bool faceToModBy(const std::vector<int> &pieces, const int modby[5], int startingFace = 0);

  private:
    std::queue<numdir> rotateQueue;
    std::stack<numdir> undoStack;
    std::queue<numdir> shadowRotateQueue;
    friend class EdgeLayerAssist;
    friend class CornerLayerAssist;
};

// global main Megaminx object (pointer, managed)
extern Megaminx* megaminx;
extern Megaminx* shadowDom;

//lambdas for over-rotation
static auto MM5 = [](int &over) { while (over >= 5) over -= 5; };
static auto rM3Mr = [](int &over) { while (over <= -3) over += 5; while (over >= 3) over -= 5; };
static auto MMMg = [](int &over, megaminxColor stop) { while (over > (int)stop) over -= 5; };
static auto MMmin = [](megaminxColor x, megaminxColor y) {
  if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))  return BEIGE;
  else  return std::min(x,y);
};
