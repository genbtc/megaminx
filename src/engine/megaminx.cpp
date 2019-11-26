/* MegaMinx2 v1.34 - 2017+2018+2019 - genBTC mod
 * Uses code from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
 * genBTC November 2017 - genbtc@gmx.com / @genr8_ / github.com/genbtc/
 * genBTC December 2018 - fixups, tweaks.
 * genBTC January 2019 - human rotate auto solve layers 1-6
   genBTC October 2019 - fixup code. making gray layer 7
 */
#include "megaminx.h"
#include <algorithm>

//simple constructor. 
Megaminx::Megaminx()
{
    solve();
    renderAllPieces();
}

//Solve Puzzle, aka Reset, aka the real constructor.
void Megaminx::solve()
{
    _rotatingFaceIndex = -1;
    isRotating = false;
    initEdgePieces();
    initCornerPieces();
    initFacePieces();
}

//Init the Edge pieces.
void Megaminx::initEdgePieces()
{
    //store a list of the basic starting vertexes
    double* edgeVertexList = edges[0].edgeInit();
    for (int i = 0; i < numEdges; ++i) {
        edges[i].init(i, edgeVertexList);
    }
}

//Init the Corner pieces.
void Megaminx::initCornerPieces()
{
    double* cornerVertexList = corners[0].cornerInit();
    for (int i = 0; i < numCorners; ++i) {
        corners[i].init(i, cornerVertexList);
    }
}

//Init the Faces: Init the Centers, attach them to Faces. Set up the Axes
// of the faces, and attach the Edge and Corner pieces to the Faces.
void Megaminx::initFacePieces()
{
    double* centerVertexList = faces[0].faceInit();
    for (int i = 0; i < numFaces; ++i) {
        centers[i].init(i);
        faces[i].attachCenter(centers + i, centerVertexList);
        faces[i].initAxis(i);
        faces[i].attachEdgePieces(edges[0]);
        faces[i].attachCornerPieces(corners[0]);
    }
}

//Need to render all the pieces unconditionally. (once at the start)
void Megaminx::renderAllPieces()
{
    for (const auto &center : centers)
        center.render();
    for (const auto &edge : edges)
        edge.render();
    for (const auto &corner: corners)
        corner.render();
    //for (auto& face : faces)
    //    face.render();
}

//Display render function for OpenGL
void Megaminx::render()
{
    if (invisible) return;
    //Start the face rotation Queue for multiple ops.
    if (!rotateQueue.empty()) {
        const auto op = rotateQueue.front();
        isRotating = true;
        _rotatingFaceIndex = op.num;
        faces[_rotatingFaceIndex].rotate(op.dir);
    }

    //Process all pieces that are NOT part of a rotating face.
    for (int i = 0; i < numFaces; ++i) {
        if (&centers[i] != faces[_rotatingFaceIndex].center)
            centers[i].render();
    }
    for (int i = 0, k = 0; i < numEdges; ++i) {
        if (&edges[i] != faces[_rotatingFaceIndex].edge[k])
            edges[i].render();
        else
            k++;
    }
    for (int i = 0, k = 0; i < numCorners; ++i) {
        if (&corners[i] != faces[_rotatingFaceIndex].corner[k])
            corners[i].render();
        else
            k++;
    }
    if (_rotatingFaceIndex == -1) return;
    //call .RENDER() and find out if successful
    const bool isRotaFullyRendered = faces[_rotatingFaceIndex].render();
    //If yes, then Finish the Rotation & advance the Queue
    if (isRotaFullyRendered && isRotating) {
        rotateQueue.pop();
        isRotating = false;
    }
}

/**
 * \brief Rotate a face. Public function (Input Validated).
 * \param num  Nth-face number color (1-12)
 * \param dir  -1 CW, 1 CCW
 */
void Megaminx::rotate(int num, int dir)
{
    assert(num > 0 && num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    num -= 1; //Convert 1-12 Faces into array [0-11]
    rotateQueue.push({ num, dir });
    undoStack.push({ num, dir });
}

//Adds entire vector of numdirs to the Rotate queue one by one.
void Megaminx::rotateBulkAlgoVector(std::vector<numdir> &bulk)
{
    undoStack.push({ -999,-999 });
    for (auto one : bulk) {
        rotateQueue.push(one);
        undoStack.push(one);
    }
    undoStack.push({ 999, 999 });
}
//Takes an Algo String and parses it, vectorizes it, then rotates it. (based off current-face)
void Megaminx::rotateBulkAlgoString(std::string algoString)
{
    const colordirs &loc = g_faceNeighbors[g_currentFace->getNum()+1];
    std::vector<numdir> bulk = ParseAlgorithmString(algoString, loc);
    rotateBulkAlgoVector(bulk);
}
//Takes an Algo String and parses it, vectorizes it, then rotates it. (loc is passed in)
void Megaminx::rotateBulkAlgoString(std::string algoString, const colordirs& loc)
{
    std::vector<numdir> bulk = ParseAlgorithmString(algoString, loc);
    rotateBulkAlgoVector(bulk);
}

//An unlimited Undo LIFO stack
void Megaminx::undo()
{
    if (undoStack.empty()) return;
    auto op = undoStack.top();
    op.dir *= -1;
    rotateQueue.push(op);
    undoStack.pop();
}
void Megaminx::undoDouble()
{
    if (undoStack.size() < 2) return;
    undo(); undo();
}
void Megaminx::undoQuad()
{
    if (undoStack.size() < 4) return;
    undoDouble(); undoDouble();
}
void Megaminx::undoBulk()
{
    if (undoStack.empty()) return;
    auto end = undoStack.top();
    if (end.num == 999 && end.dir == 999) {
        undoStack.pop();
        while (!undoStack.empty()) {
            auto begin = undoStack.top();
            if (begin.num == -999 && begin.dir == -999) {
                undoStack.pop();
                break;
            }
            begin.dir *= -1;
            rotateQueue.push(begin);
            undoStack.pop();
        }
    }
}

//Clear the Queue and stop any repeated rotating actions.
void Megaminx::resetQueue()
{
    isRotating = false;
    rotateQueue = std::queue<numdir>();
    //TODO: at this point, the other Undo-Queue now has extra actions in it.
    //    : find how many we skipped in the "cancel" and pop those off the undo queue too.
}

//Scramble by Rotating 1400-2700 times (137 x 12 x 2?)
//NOTE: apparently scrambles are even more random if you rotate by 2/5ths instead of 1/5th.
void Megaminx::scramble()
{
    resetQueue();
    //Do 137 iterations of scrambling (like a human)
    for (int q = 0; q < 137; q++) {
        int t = 0;
        //numFaces faces - turn one each, randomizing direction
        for (int i = 0; i < numFaces; i++) {
            const int r = rand() % 2 * 2 - 1; //generates -1 or 1
            faces[i].placeParts(r);
            if (t == 0) {
                faces[i].placeParts(r); //FIXED: do a second rotation. 
                t++;
            }
            else
                t = 0;  //alternate or not
        }
    }
}

/**
 * \brief Makes a pointer to g_currentFace
 * \param i Nth-face number color (1-12)
 */
void Megaminx::setCurrentFaceActive(int i)
{
    assert(i > 0 && i <= numFaces);
    i -= 1;     //Convert 1-numFaces Faces into array 0-11
    g_currentFace = &faces[i];
    assert(g_currentFace->getNum() == i);    //double check.
}

/**
 * \brief Reset all the pieces on a face and set it to active.
 * \param n Nth-face number color (1-12)
 */
void Megaminx::resetFace(int n)
{
    assert(n > 0 && n <= numFaces);
    resetFacesEdges(n);
    resetFacesCorners(n);
    setCurrentFaceActive(n);
}

/**
 * \brief Returns a list of pieces on [Face], (either Edge or Corner piece)
 * \param face Nth-face number (1-12)
 */
template <typename T>
std::vector<int> Megaminx::findPieces(int face)
{
    if (std::is_same<T, Corner>::value)
        return faces[face - 1].findPiecesOfFace(this->corners[0], numCorners);
    else if (std::is_same<T, Edge>::value)
        return faces[face - 1].findPiecesOfFace(this->edges[0], numEdges);
    return {};
} //where T = Corner or Edge
std::vector<int> Megaminx::findCorners(int face) { return findPieces<Corner>(face); };
std::vector<int> Megaminx::findEdges(int face) { return findPieces<Edge>(face); };

/**
 * \brief Flip, Changes the colors of a piece (either Edge or Corner piece)
 * \param face Nth-face number (1-12)
 * \param num  Nth-piece number (1-5)
 */
template <typename T>
void Megaminx::flipPieceColor(int face, int num)
{
    assert(face > 0 && face <= numFaces);
    assert(num > 0 && num <= 5);
    Piece* piece = faces[face - 1].getFacePiece<T>(num-1);
    piece->flip();
} //where T = Corner or Edge
void Megaminx::flipCornerColor(int face, int num) { return flipPieceColor<Corner>(face, num); }
void Megaminx::flipEdgeColor(int face, int num) { return flipPieceColor<Edge>(face, num); }

/**
 * \brief Get a list of all pieces Color status (either Edge or Corner Piece)
 */
template <typename T>
std::vector<int> Megaminx::getAllPiecesColorFlipStatus()
{
    std::vector<int> allPiecesPos;
    for (int face = 0; face < 12; ++face)
        for (int r = 0; r < 5; ++r) {
            Piece* piece = faces[face].getFacePiece<T>(r);
            allPiecesPos.push_back(piece->data.flipStatus);
        }
    return allPiecesPos;
} //where T = Corner or Edge
std::vector<int> Megaminx::getAllCornerPiecesColorFlipStatus() { return getAllPiecesColorFlipStatus<Corner>(); }
std::vector<int> Megaminx::getAllEdgePiecesColorFlipStatus() { return getAllPiecesColorFlipStatus<Edge>(); }


/**
 * \brief Revert all the edge pieces on the Nth colored face back to normal.
 *          To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number (1-12)
 * \return success (1)
 */
int Megaminx::resetFacesEdges(int color_n) {
    assert(color_n > 0 && color_n <= numFaces);
    const auto& defaultEdges = faces[(color_n - 1)].defaultEdges;
    return resetFacesEdges(color_n, defaultEdges);
}
int Megaminx::resetFacesEdges(int color_n, const std::vector<int> &defaultEdges, bool solve)
{
    assert(color_n > 0 && color_n <= numFaces);
    for (int j = 0; j < 5; ++j) {
        std::vector<int> foundEdges = findPieces<Edge>(color_n);
        std::vector<int> wrongEdges;        
        std::set_difference(foundEdges.begin(), foundEdges.end(), defaultEdges.begin(), defaultEdges.end(),
            std::inserter(wrongEdges, wrongEdges.begin()));
        std::vector<int> nextDefault;
        std::set_difference(defaultEdges.begin(), defaultEdges.end(), foundEdges.begin(), foundEdges.end(),
            std::inserter(nextDefault, nextDefault.begin()));
        for (int k = 0; k < wrongEdges.size(), k < nextDefault.size(); ++k) {
            edges[wrongEdges[k]].swapdata(edges[nextDefault[k]].data);
        }
    }
    if (!solve)
        return 0;
    auto activeFace = faces[(color_n - 1)];
    auto foundEdges2 = findPieces<Edge>(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundEdges2 == defaultEdges);
    assert(foundEdges2.size() == 5);
    for (int j = 0; j < 5; ++j) {
        //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
        while (activeFace.edge[j]->data.flipStatus != 0)
            activeFace.edge[j]->flip();
        //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
        auto &pIndex = activeFace.edge[j]->data.pieceNum;
        if (pIndex != defaultEdges[j]) {
            edges[pIndex].swapdata(edges[defaultEdges[j]].data);
            j = -1;
        }
    }
    return 1;
}

/**
 * \brief Revert all the Corners pieces on the Nth colored face back to normal.
 *          To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number (1-12)
 * \return success (1)
 */
int Megaminx::resetFacesCorners(int color_n) {
    assert(color_n > 0 && color_n <= numFaces);
    const auto& defaultCorners = faces[(color_n - 1)].defaultCorners;
    return resetFacesCorners(color_n, defaultCorners);
}
int Megaminx::resetFacesCorners(int color_n, const std::vector<int> &defaultCorners, bool solve)
{
    assert(color_n > 0 && color_n <= numFaces);
    for (int j = 0; j < 5; ++j) {
        std::vector<int> foundCorners = findPieces<Corner>(color_n);
        std::vector<int> wrongCorners;        
        std::set_difference(foundCorners.begin(), foundCorners.end(), defaultCorners.begin(), defaultCorners.end(),
            std::inserter(wrongCorners, wrongCorners.begin()));
        std::vector<int> nextDefault;
        std::set_difference(defaultCorners.begin(), defaultCorners.end(), foundCorners.begin(), foundCorners.end(),
            std::inserter(nextDefault, nextDefault.begin()));
        for (int k = 0; k < wrongCorners.size(), k < nextDefault.size(); ++k) {
            corners[wrongCorners[k]].swapdata(corners[nextDefault[k]].data);
        }
    }
    if (!solve)
        return 0;
    auto activeFace = faces[(color_n - 1)];
    auto foundCorners2 = findPieces<Corner>(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundCorners2 == defaultCorners);
    assert(foundCorners2.size() == 5);
    for (int j = 0; j < 5; ++j) {
        //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
        while (activeFace.corner[j]->data.flipStatus != 0)
            activeFace.corner[j]->flip();
        //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
        auto &pIndex = activeFace.corner[j]->data.pieceNum;
        if (pIndex != defaultCorners[j]) {
            corners[pIndex].swapdata(corners[defaultCorners[j]].data);
            j = -1;
        }
    }
    return 1;
}

template <typename T>
int Megaminx::findPiece(int pieceNum)
{
    Piece* piece = getPieceArray<T>(0);
    for (int i = 0; i < getMaxNumberOfPieces<T>(); ++i)
        if (piece[i].data.pieceNum == pieceNum)
            return i;
    return -1;
} //where T = Corner or Edge
int Megaminx::findEdge(int pieceNum){ return findPiece<Edge>(pieceNum); }
int Megaminx::findCorner(int pieceNum) { return findPiece<Corner>(pieceNum); }

template <typename T>
std::vector<int> Megaminx::findFivePieces(const int pieceNums[5])
{
    std::vector<int> pieceList;
    for (int i = 0; i < 5; i++)
        pieceList.push_back(findPiece<T>(pieceNums[i]));
    return pieceList;
} //where T = Corner or Edge
std::vector<int> Megaminx::findEdgePieces(const int pieceNums[5]) { return findFivePieces<Edge>(pieceNums); }
std::vector<int> Megaminx::findCornerPieces(const int pieceNums[5]) { return findFivePieces<Corner>(pieceNums); }

template <typename T>
std::vector<int> Megaminx::findFivePieces(std::vector<int> &v)
{
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    return findFivePieces<T>(vecPieceNums);
} //where T = Corner or Edge
std::vector<int> Megaminx::findEdgePieces(std::vector<int> &v) { return findFivePieces<Edge>(v); }
std::vector<int> Megaminx::findCornerPieces(std::vector<int> &v) { return findFivePieces<Corner>(v); }

template <typename T>
void Megaminx::resetFivePieces(const int indexes[5]) {
    Piece* pieces = getPieceArray<T>(0);
    for (int i = 0; i < 5; ++i) {
        if (pieces[indexes[i]].data.pieceNum != indexes[i]) {
            int whereAreTheyNow = findPiece<T>(indexes[i]);
            pieces[indexes[i]].swapdata(pieces[whereAreTheyNow].data);
            i = -1;
        }
    }
    //Pieces are in the right place but maybe wrong orientation, so flip the colors:
    for (int i = 0; i < 5; ++i) {
        while (pieces[indexes[i]].data.flipStatus != 0)
            pieces[indexes[i]].flip();
    }
} //where T = Corner or Edge
void Megaminx::resetFiveEdges(const int indexes[5]) { resetFivePieces<Edge>(indexes); }
void Megaminx::resetFiveCorners(const int indexes[5]) { resetFivePieces<Corner>(indexes); }

template <typename T>
void Megaminx::resetFivePieces(std::vector<int> &v) {
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    resetFivePieces<T>(vecPieceNums);
} //where T = Corner or Edge
void Megaminx::resetFiveEdges(std::vector<int> &v) { resetFivePieces<Edge>(v); }
void Megaminx::resetFiveCorners(std::vector<int> &v) { resetFivePieces<Corner>(v); }

/**
 * \brief Algorithm Switcher Dispatcher. Queues multiple rotate ops to happen
 * in accordance with how a player would want to achieve certain swaps.
 * \param face from 1 - 12
 */
void Megaminx::rotateAlgo(int face, int id)
{
    assert(face > 0 && face <= numFaces);
    const colordirs &loc = g_faceNeighbors[face];
    rotateBulkAlgoString(g_AlgoStrings[id].algo, loc);
}

//A letter/notation parser to manipulate the cube with algo strings
const std::vector<numdir> Megaminx::ParseAlgorithmString(std::string algorithmString, colordirs loc)
{
    std::vector<numdir> readVector;
    std::stringstream ss(algorithmString); // create a stringstream to iterate over
    auto npos = std::string::npos;
    while (ss) {                           // while the stream is good
        std::string word;                  // parse first word
        numdir op = { -1, Face::Clockwise };
        if (ss >> word) {
            if (word.find("'") != npos)
                op.dir *= -1;
            if ((word.find("dr") != npos) ||
                (word.find("DR") != npos))
                op.num = loc.downr - 1;
            else if ((word.find("dl") != npos) ||
                (word.find("DL") != npos))
                op.num = loc.downl - 1;
            else if ((word.find("r") != npos) ||
                (word.find("R") != npos))
                op.num = loc.right - 1;
            else if ((word.find("l") != npos) ||
                (word.find("L") != npos))
                op.num = loc.left - 1;
            else if ((word.find("f") != npos) ||
                (word.find("F") != npos))
                op.num = loc.front - 1;
            else if ((word.find("u") != npos) ||
                (word.find("U") != npos))
                op.num = loc.up - 1;
            if (op.num > -1)
                readVector.push_back(op);
            if (word.find("2") != npos) {
                assert(readVector.size() > 0);
                readVector.push_back(readVector.back());
            }
        }
    }
    return readVector;
}
