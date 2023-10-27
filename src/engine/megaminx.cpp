/* MegaMinx v1.4.4 - 2017+2018+2019+2020+2023 - genBTC edition
 * Uses code originally from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
 * genBTC November 2017 - genbtc@gmx.com / @genr8_ / github.com/genbtc/
 * genBTC December 2018 - fixups, tweaks.
 * genBTC January 2019 - human rotate auto solve layers 1-6
   genBTC October 2019 - fixup code. started gray layer 7 solver
   genBTC February 2020 - completed gray layer 7 solver
   genBTC February 2023 - taking another look at old code
   genBTC October 2023 - fixing LD unresolved symbol errors from template linkages across TU's
 */
#include "megaminx.hpp"

// global main Megaminx object (pointer, managed)
Megaminx* megaminx;
Megaminx* shadowDom;

/**
 * \brief Megaminx main simple constructor for init.
 * \note   Setup, Solve Puzzle (aka Reset), Render
 */
Megaminx::Megaminx()
{
    g_currentFace = NULL;
    _rotatingFaceIndex = -1;
    isRotating = false;
    invisible = true;
    initEdgePieces();
    initCornerPieces();
    initFacePieces();
    renderAllPieces();
    invisible = false;
}

/**
 * \brief Init the Edge pieces.
 * \note   numEdges = 30
 */
void Megaminx::initEdgePieces()
{
    //store a list of the basic starting Edge vertexes
    double* edgeVertexList = edges[0].edgeInit();
    for (int i = 0; i < numEdges; ++i)
        edges[i].init(i, edgeVertexList);
}

/**
 * \brief Init the Corner pieces.
 * \note   numCorners = 20
 */
void Megaminx::initCornerPieces()
{
    //store a list of the basic starting Corner vertexes
    double* cornerVertexList = corners[0].cornerInit();
    for (int i = 0; i < numCorners; ++i)
        corners[i].init(i, cornerVertexList);
}

/**
 * \brief Init the Faces and All Pieces.
 * \note - Init the Centers, attach them to Faces.
 *         Set up the Axes of the faces,
 *          and attach the Edge and Corner pieces to the Faces.
 */
void Megaminx::initFacePieces()
{
    double* centerVertexList = faces[0].faceInit();
    for (int i = 0; i < numFaces; ++i) {
        centers[i].init(i);
        faces[i].attachCenter(centers + i, centerVertexList);
        faces[i].initAxis(i);
        faces[i].attachEdgePieces(this, edges[0]);
        faces[i].attachCornerPieces(this, corners[0]);
    }
}

/**
 * \brief Default Render ALL the pieces (unconditionally)
 */
void Megaminx::renderAllPieces()
{
    for (const auto &center : centers)
        center.render();
    for (const auto &edge : edges)
        edge.render();
    for (const auto &corner: corners)
        corner.render();
}

/**
 * \brief Main Render Logic function - (start handling rotation calls and sub-object render calls)
 * \note Conditionally call each OpenGL .render() func (each rotating face, center, edge, corner)
 */
void Megaminx::render()
{
    //Skip everything if its invisible
    if (invisible)
        return;

    //Start the face rotation Queue for multiple ops.
    if (!rotateQueue.empty()) {
        isRotating = true;
        const auto &op = rotateQueue.front();
        _rotatingFaceIndex = op.num;    //this is set only here
        assert(_rotatingFaceIndex != -1);   //ensure safety
        faces[_rotatingFaceIndex].rotate(op.dir);
    } else if (_rotatingFaceIndex == -1) {
        renderAllPieces();
        return; //rest of function can be skipped to avoid array[-1] error
    }
    // Full Re-render all if non-rotating or early startup
    // (starts up with _rotatingFaceIndex is -1)
    //else
    std::cout << _rotatingFaceIndex << std::endl;
    //Conditionally Process all pieces that are NOT part of a rotating face.
    for (int i = 0; i < numFaces; ++i) {
        if (&centers[i] != faces[_rotatingFaceIndex].center)
            centers[i].render();
    }
    for (int i = 0, k = 0; i < numEdges && k < 5; ++i) {
        if (&edges[i] != faces[_rotatingFaceIndex].edge[k])
            edges[i].render();
        else
            k++;
    }
    for (int i = 0, k = 0; i < numCorners && k < 5; ++i) {
        if (&corners[i] != faces[_rotatingFaceIndex].corner[k])
            corners[i].render();
        else
            k++;
    }

    //call .RENDER() and find out if successful
    if (faces[_rotatingFaceIndex].render() && isRotating) {
        //If yes, then Finish the Rotation & advance the Queue
        rotateQueue.pop();
        isRotating = false; _rotatingFaceIndex = -1;
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

/**
 * \brief rotateAlgorithm Switcher Dispatcher. Queues multiple rotate ops
 * \param face from 1 - 12
 * \param id Index into g_AlgoStrings[] struct
 */
void Megaminx::rotateAlgo(int algoID, int face)
{
    if (face == -1)
        face = g_currentFace->getNum()+1;
    assert(face > 0 && face <= numFaces);
    //Either rotate 1 time, or however many repeatX says, if it exists.
    int repeat = g_AlgoStrings[algoID].repeatX ? g_AlgoStrings[algoID].repeatX : 1;
    for (int n = 0; n < repeat; ++n) {
        rotateBulkAlgoString(g_AlgoStrings[algoID].algo, g_faceNeighbors[face], algoID);
    }
}

/**
 * \brief Takes an Algo String and parses it, vectorizes it, then rotates it. (Algo ID is tracked)
 */
void Megaminx::rotateBulkAlgoString(std::string algoString, const colordirs &loc, int algoID)
{
    std::vector<numdir> bulk = ParseAlgorithmString(algoString, loc, algoID);
    rotateBulkAlgoVector(bulk);
}
/**
 * \brief Adds entire vector of numdirs to the Rotate queue one by one.
 */
void Megaminx::rotateBulkAlgoVector(const std::vector<numdir> &bulk)
{
    undoStack.push({ -999,-999 });  //begin sentinel value flag
    for (auto one : bulk) {
        rotateQueue.push(one);
        undoStack.push(one);
    }
    undoStack.push({ 999, 999 });   //end sentinel value flag
}

/**
 * \brief An unlimited Undo LIFO stack. undo one move.
 */
void Megaminx::undo()
{
    if (undoStack.empty()) return;
    auto &op = undoStack.top();
    //skip any of the sentinel value flags
    if (op.num == 999 || op.dir == 999 || op.num == -999 || op.dir == -999) {
        undoStack.pop();
        return;
    }
    op.dir *= -1;
    rotateQueue.push(op);
    undoStack.pop();
}
/**
 * \brief Undo twice in a row
 */
void Megaminx::undoDouble()
{
    if (undoStack.size() < 2) return;
    undo(); undo();
}
/**
 * \brief Undo four times in a row
 */
void Megaminx::undoQuad()
{
    if (undoStack.size() < 4) return;
    undoDouble(); undoDouble();
}
/**
 * \brief Undo a bulk whole Chunk of moves at once
 * \note   (sequences like these come from rotateBulkAlgoVector() above )
 * \remark  (this is why we have the -999/999 flag values)
 */
void Megaminx::undoBulk()
{
    if (undoStack.empty()) return;
    const auto &end = undoStack.top();
    if (end.num == 999 && end.dir == 999) {
        undoStack.pop();
        while (!undoStack.empty()) {
            auto &begin = undoStack.top();
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

/**
 * \brief Clear Rotation Queue and stop any repeated rotating actions.
 */
void Megaminx::resetQueue()
{
    isRotating = { };
    rotateQueue = { };
    undoStack = { };
}

/**
 * \brief Scramble by Rotating 1400-2700 times (137 x 12 x 2?)
 * \note  (apparently scrambles are even more random if you rotate by 2/5ths instead of 1/5th)
 */
void Megaminx::scramble()
{
    resetQueue();
    //Do 137 iterations of scrambling (like a good human would...)
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
 * \brief setCurrentFaceActive - helper for GUI. Makes global pointer to g_currentFace
 * \param i Nth-face number (1-12)
 */
inline void Megaminx::setCurrentFaceActive(int i)
{
    assert(i > 0 && i <= numFaces);          //require valid input
    i -= 1;     //Convert 1-numFaces Faces into array 0-11 (safe)
    g_currentFace = &faces[i];
    assert(g_currentFace->getNum() == i);    //internal consistency check.
}

/**
 * \brief Reset all the pieces on [Face] and set it to active.
 * \param n Nth-face number (1-12)
 */
void Megaminx::resetFace(int n)
{
    assert(n > 0 && n <= numFaces);
    resetFacesEdges(n);
    resetFacesCorners(n);
    setCurrentFaceActive(n);
}

/**
 * \brief Change Piece COLOR, internally FLIPS the piece, <template> (either Edge or Corner piece)
 * \param face  Nth-face number (1-12)
 * \param num   Nth-piece number (1-5)
 */
template <typename T>
void Megaminx::flipPieceColor(int face, int num)
{
    assert(face > 0 && face <= numFaces);
    assert(num > 0 && num <= 5);      //ensure safe access
    Piece* piece = faces[face - 1].getFacePiece<T>(num-1);
    piece->flip();
} //where T = Corner or Edge
void Megaminx::flipCornerColor(int face, int num) { return flipPieceColor<Corner>(face, num); }
void Megaminx::flipEdgeColor(int face, int num) { return flipPieceColor<Edge>(face, num); }

/**
 * \brief Get all pieces POSITION status, <template> (either Edge or Corner piece)
 * \note   (used by load.cpp Save/Restore Cube to capture state)
 */
template <typename T>
std::vector<int> Megaminx::getAllPiecesPosition()
{
    const int maxpcs = getMaxNumberOfPieces<T>();
    std::vector<int> allPiecesPos(maxpcs);
    for (int r = 0; r < maxpcs; ++r) {
        allPiecesPos[r] = getPieceArray<T>(0)[r].data.pieceNum;
    }
    //std::for_each();
    return allPiecesPos;
} //where T = Corner or Edge
std::vector<int> Megaminx::getAllCornerPiecesPosition()  { return getAllPiecesPosition<Corner>(); }
std::vector<int> Megaminx::getAllEdgePiecesPosition()  { return getAllPiecesPosition<Edge>(); }

/**
 * \brief Get all pieces COLOR status, <template> (either Edge or Corner piece)
 * \note   (used by load.cpp Save/Restore Cube to capture state)
 */
template <typename T>
std::vector<int> Megaminx::getAllPiecesColorFlipStatus()
{
    const int maxpcs = getMaxNumberOfPieces<T>();
    std::vector<int> allPiecesPos(maxpcs);
    for (int r = 0; r < maxpcs; ++r) {
        allPiecesPos[r] = getPieceArray<T>(0)[r].data.flipStatus;
    }
    return allPiecesPos;
} //where T = Corner or Edge
std::vector<int> Megaminx::getAllCornerPiecesColorFlipStatus()  { return getAllPiecesColorFlipStatus<Corner>(); }
std::vector<int> Megaminx::getAllEdgePiecesColorFlipStatus()  { return getAllPiecesColorFlipStatus<Edge>(); }

/**
 * \brief Find A Face Piece Order List, <template> (either Edge or Corner piece)
 * \param face Nth-face number (1-12)
 * \return Returns an EXACT ORDER list of pieces on FACE
 */
template <typename T>
std::vector<int> Megaminx::findFacePiecesOrder(int face)
{
    return faces[face - 1].findPiecesOrder<T>();
} //where T = Corner or Edge
std::vector<int> Megaminx::findFaceCornersOrder(int face) { return faces[face - 1].findEdgesOrder(); }
std::vector<int> Megaminx::findFaceEdgesOrder(int face) { return faces[face - 1].findCornersOrder(); }

/**
 * \brief Finds the colored center that is perma-attached to a face, and then
 *         iterates the entire list of pieces to find when the colors match, and outputs a list.
 * \param face Nth-face number (1-12)
 * \param pieceRef Takes a reference to the [0]th member of Pointer_array of (either Corner/Edge's)
 * \param times how many times to iterate over the ref'd array
 * \return Returns the list of 5 positions where the starting face's pieces have ended up at.
 * \note    NOTE: Finds pieces BEFORE they are attached to a face.
 */
std::vector<int> Megaminx::findPiecesOfFace(int face, Piece &pieceRef, int times) const
{
    std::vector<int> pieceList;
    const int color = faces[face - 1].center->data._colorNum[0];
    assert(face == color);
    for (int i = 0; i < times && pieceList.size() < 5; ++i) {
        const bool result = (&pieceRef)[i].matchesColor(color);
        if (result)
            pieceList.push_back(i);
    }
    return pieceList;
}
/**
 * \brief Find Pieces, <template> (either Edge or Corner piece)
 * \param face Nth-face number (1-12)
 * \return Returns a SORTED list of pieces on FACE
 */
template <typename T>
std::vector<int> Megaminx::findPieces(int face)
{
    if (std::is_same<T, Corner>::value)
        return findPiecesOfFace(face, this->corners[0], numCorners);
    else if (std::is_same<T, Edge>::value)
        return findPiecesOfFace(face, this->edges[0], numEdges);
    return {};
} //where T = Corner or Edge
std::vector<int> Megaminx::findCorners(int face) { return findPieces<Corner>(face); }
std::vector<int> Megaminx::findEdges(int face) { return findPieces<Edge>(face); }

//Find5s------------------------------------------------------------------------------------------------------//
/**
 * \brief Find 5, a specific five-piece chunk (by INDEX ARRAY)
 */
template <typename T>
std::vector<int> Megaminx::findFivePieces(const int pieceNums[5])
{
    std::vector<int> pieceList(5);
    for (int i = 0; i < 5; i++)
        pieceList[i] = findPiece<T>(pieceNums[i]);
    return pieceList;
} //where T = Corner or Edge
/** \brief Find 5, a specific five-piece chunk (by VECTOR) */
template <typename T>
std::vector<int> Megaminx::findFivePieces(const std::vector<int> &v)
{
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    return findFivePieces<T>(vecPieceNums);
} //where T = Corner or Edge
std::vector<int> Megaminx::findEdgePieces(const int pieceNums[5]) { return findFivePieces<Edge>(pieceNums); }
std::vector<int> Megaminx::findCornerPieces(const int pieceNums[5]) { return findFivePieces<Corner>(pieceNums); }
std::vector<int> Megaminx::findEdgePieces(const std::vector<int> &v) { return findFivePieces<Edge>(v); }
std::vector<int> Megaminx::findCornerPieces(const std::vector<int> &v) { return findFivePieces<Corner>(v); }

//Reset5Defaults------------------------------------------------------------------------------------------------------//
/**
 * \brief Reset any five-piece chunk to defaults (by INDEX ARRAY)
 * \note cant be const, swaps & flips the received piece
 */
template <typename T>
void Megaminx::resetFivePieces(const int indexes[5]) {
    Piece* pieces = getPieceArray<T>(0);
    for (int i = 0; i < 5; ++i) {
        auto &destpiece = pieces[indexes[i]];
        auto &sourcepiece = pieces[findPiece<T>(indexes[i])];
        if (sourcepiece.data.pieceNum == destpiece._defaultPieceNum)
            destpiece.swapdata(sourcepiece.data);
    }
    //Pieces are in the right place but maybe wrong orientation, so flip the colors:
    for (int i = 0; i < 5; ++i) {
        while (pieces[indexes[i]].data.flipStatus != 0)
            pieces[indexes[i]].flip();
    }
} //where T = Corner or Edge
void Megaminx::resetFiveEdges(const int indexes[5]) { resetFivePieces<Edge>(indexes); }
void Megaminx::resetFiveCorners(const int indexes[5]) { resetFivePieces<Corner>(indexes); }

/**
 * \brief Reset any five-piece chunk to defaults (by VECTOR)
 */
template <typename T>
void Megaminx::resetFivePiecesV(std::vector<int> &v) {
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    resetFivePieces<T>(vecPieceNums);
} //where T = Corner or Edge
void Megaminx::resetFiveEdgesV(std::vector<int> &v) { resetFivePiecesV<Edge>(v); }  //unused
void Megaminx::resetFiveCornersV(std::vector<int> &v) { resetFivePiecesV<Corner>(v); }  //unused

//Resets--------------------------------------------------------------------------------------------------------------//
/**
 * \brief Generic <template> to Reset all the Face pieces to their default value.
 * \param color_n N'th Face/Color Number (1-12)
 * \return 1 if anything moved, 0 if not
 * \deprecated Not Currently Used But Has Good Template is_same Machinery
 */
template <typename T>
int Megaminx::resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve)
{
    if (std::is_same<T, Edge>::value)
        return resetFacesEdges(color_n, defaultPieces, solve);
    else if (std::is_same<T, Corner>::value)
        return resetFacesCorners(color_n, defaultPieces, solve);
    return 0;
} //where T = Corner or Edge
int Megaminx::resetFacesPiecesEdges(int color_n, const std::vector<int> &defaultPieces, bool solve) { return resetFacesPieces<Edge>(color_n, defaultPieces, solve); }   //unused
int Megaminx::resetFacesPiecesCorners(int color_n, const std::vector<int> &defaultPieces, bool solve) { return resetFacesPieces<Corner>(color_n, defaultPieces, solve); }   //unused

/**
 * \brief Revert all the edge pieces on the Nth colored face back to normal.
 *          To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number (1-12)
 * \return success (1)
 */
int Megaminx::resetFacesEdges(int color_n) {
    assert(color_n > 0 && color_n <= numFaces);
    const auto &defaultEdges = faces[(color_n - 1)].defaultEdges;
    return resetFacesEdges(color_n, defaultEdges);
}
int Megaminx::resetFacesEdges(int color_n, const std::vector<int> &defaultEdges, bool solve)
{
    assert(color_n > 0 && color_n <= numFaces);
    for (int j = 0; j < 5; ++j) {
        std::vector<int> foundEdges = findPieces<Edge>(color_n); //(unordered)
        //std::vector<int> foundEdges = faces[(color_n - 1)].findPiecesOrder<Edge>(); //(ordered)
        std::vector<int> wrongEdges;
        std::set_difference(foundEdges.begin(), foundEdges.end(), defaultEdges.begin(), defaultEdges.end(),
            std::inserter(wrongEdges, wrongEdges.begin()));
        std::vector<int> nextDefault;
        std::set_difference(defaultEdges.begin(), defaultEdges.end(), foundEdges.begin(), foundEdges.end(),
            std::inserter(nextDefault, nextDefault.begin()));
        for (int k = 0; k < wrongEdges.size() && k < nextDefault.size(); ++k) {
            edges[wrongEdges[k]].swapdata(edges[nextDefault[k]].data);
        }
    }
    if (!solve)
        return 0;
    auto &activeFace = faces[(color_n - 1)];
    auto foundEdges2 = findPieces<Edge>(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundEdges2 == defaultEdges);
    assert(foundEdges2.size() == 5);
    for (int j = 0; j < 5; ++j) {
        //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
        while (activeFace.edge[j]->data.flipStatus != 0)
            activeFace.edge[j]->flip();
        //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
        int pIndex = activeFace.edge[j]->data.pieceNum;
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
    const auto &defaultCorners = faces[(color_n - 1)].defaultCorners;
    return resetFacesCorners(color_n, defaultCorners);
}
int Megaminx::resetFacesCorners(int color_n, const std::vector<int> &defaultCorners, bool solve)
{
    assert(color_n > 0 && color_n <= numFaces);
    for (int j = 0; j < 5; ++j) {
        std::vector<int> foundCorners = findPieces<Corner>(color_n);    //unordered
        //std::vector<int> foundCorners = faces[(color_n - 1)].findPiecesOrder<Corner>();   //ordered
        std::vector<int> wrongCorners;
        std::set_difference(foundCorners.begin(), foundCorners.end(), defaultCorners.begin(), defaultCorners.end(),
            std::inserter(wrongCorners, wrongCorners.begin()));
        std::vector<int> nextDefault;
        std::set_difference(defaultCorners.begin(), defaultCorners.end(), foundCorners.begin(), foundCorners.end(),
            std::inserter(nextDefault, nextDefault.begin()));
        for (int k = 0; k < wrongCorners.size() && k < nextDefault.size(); ++k) {
            corners[wrongCorners[k]].swapdata(corners[nextDefault[k]].data);
        }
    }
    if (!solve)
        return 0;
    auto &activeFace = faces[(color_n - 1)];
    auto foundCorners2 = findPieces<Corner>(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundCorners2 == defaultCorners);
    assert(foundCorners2.size() == 5);
    for (int j = 0; j < 5; ++j) {
        //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
        while (activeFace.corner[j]->data.flipStatus != 0)
            activeFace.corner[j]->flip();
        //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
        int pIndex = activeFace.corner[j]->data.pieceNum;
        if (pIndex != defaultCorners[j]) {
            corners[pIndex].swapdata(corners[defaultCorners[j]].data);
            j = -1;
        }
    }
    return 1;
}

//Parse Algorithm ID -> lettered String -> numdir notation -----------------------------------------------------------//

//NOTE: debug print is good but might as well be permanently on
bool debugAlgorithmString = true;
/**
 * \brief An algorithm parser to manipulate the cube (by lettered algostring notation) + print
 * \note   (input: sister function below takes a numerical Algo ID -> converts to -> algostring notation)
 * \return vector of numdir rotation items
 */
const std::vector<numdir> Megaminx::ParseAlgorithmString(std::string algorithmString, const colordirs &loc, int algoID)
{
    if (debugAlgorithmString)
        std::cout << "ParseAlgorithmString: # " << algoID << " : " << algorithmString \
                  << " @ Face: " << g_colorRGBs[loc.front].name << std::endl;
    std::vector<numdir> readVector;
    std::stringstream ss(algorithmString); // create a stringstream to iterate over
    auto npos = std::string::npos;
    while (ss) {                           // while the stream is good
        std::string word;                  // parse first word
        numdir op = { -1, Face::Clockwise, algoID };
        if (ss >> word) {
            if (word.find("'") != npos)    //reverse direction if its a ' Prime
                op.dir *= -1;
            if ((word.find("dr") != npos) ||    //lowercase means normal direction.
                (word.find("dR") != npos) ||    //parser doesnt care about capitals, humans do tho.
                (word.find("DR") != npos))      //capital means ' Prime because we use SHIFT key
                op.num = loc.downr - 1;
            else if ((word.find("dl") != npos) ||
                     (word.find("dL") != npos) ||
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
            else if ((word.find("b") != npos) ||
                     (word.find("B") != npos))
                op.num = loc.bottom - 1;
            if (op.num > -1)
                readVector.push_back(op);
            if (word.find("2") != npos) {
                assert(readVector.size() > 0);
                readVector.push_back(readVector.back());
            }
        }
    }
    //NOTE: repeatX not needed anymore after we explicitly defined every algorithm verbosely
    //if (algorithmString.repeatX) {
    //    const auto &old_count = readVector.size();
    //    readVector.resize(algorithmString.repeatX * old_count);
    //    std::copy_n(readVector.begin(), old_count, (readVector.begin() + old_count));
    //}
    return readVector;
}

/**
 * \brief An algorithm parser to manipulate the cube (by algo ID#) converts to AlgoString - print?
 */
const std::vector<numdir> Megaminx::ParseAlgorithmID(int algoID, int startLoc)
{
    const std::string algorithmString = g_AlgoStrings[algoID].algo;
    const colordirs loc = g_faceNeighbors[startLoc];
    //std::cout << "ParseAlgorithmString2: # " << algo << " : " << algorithmString << std::endl;
    return ParseAlgorithmString(algorithmString, loc, algoID);
}
