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
void Megaminx::shadowRotate(int num, int dir)
{
    assert(num > 0 && num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    num -= 1; //Convert 1-12 Faces into array [0-11]
    shadowRotateQueue.push({ num, dir });
    faces[num].placeParts(dir);
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
//FIXED: apparently scrambles are even more random if you rotate by 2/5ths instead of 1/5th.
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
 * \brief Flip, Changes the colors of an Edge or Corner Piece
 * \param face Nth-face number (1-12)
 * \param num  Nth-piece number (1-5)
 */
template <typename T>
void Megaminx::flipPieceColor(int face, int num)
{
    assert(face > 0 && face <= numFaces);
    assert(num > 0 && num <= 5);
    Piece* piece = faces[face].getFacePiece<T>(num);
    piece->flip();
} //where T = Corner or Edge
void Megaminx::flipCornerColor(int face, int num) { return flipPieceColor<Corner>(face, num); }
void Megaminx::flipEdgeColor(int face, int num) { return flipPieceColor<Edge>(face, num); }

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

template <typename T>
std::vector<int> Megaminx::findPieces(int i)
{
    if (std::is_same<T, Corner>::value)
        return faces[i - 1].findPiecesOfFace(this->corners[0], numCorners);
    else if (std::is_same<T, Edge>::value)
        return faces[i - 1].findPiecesOfFace(this->edges[0], numEdges);
    return {};
} //where T = Corner or Edge
std::vector<int> Megaminx::findCorners(int i) { return findPieces<Corner>(i); };
std::vector<int> Megaminx::findEdges(int i) { return findPieces<Edge>(i); };

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

template <typename T>
int Megaminx::LoadNewPiecesFromVector(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors)
{
    assert(readPieces.size() == 60);
    assert(readPieceColors.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        std::vector<int> loadPieces = { readPieces[f + 0],readPieces[f + 1],readPieces[f + 2],readPieces[f + 3],readPieces[f + 4] };
        resetFacesPieces<T>(face, loadPieces, false);
        //resetFivePieces<T>(loadPieces);
    }
    for (int face = 0; face < 12; ++face) {
        int f = face * 5;
        //Pieces can be in the right place but maybe in wrong orientation, so flip the colors:
        for (int i = 0; i < 5; ++i) {
            Piece* piece = faces[face].getFacePiece<T>(i);
            while (piece->data.flipStatus != readPieceColors[f + i])
                piece->flip();
        }
    }
    return 1;
} //where T = Corner or Edge
int Megaminx::LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors) {
    return LoadNewPiecesFromVector<Corner>(readCorners, readCornerColors);
}
int Megaminx::LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors) {
    return LoadNewPiecesFromVector<Edge>(readEdges, readEdgeColors);
}

//Load a new shadow cube up with the old ones edges
void Megaminx::LoadNewEdgesFromOtherCube(Megaminx* source)
{
    for (int i = 0; i < numEdges; ++i)
        this->edges[i].data = source->edges[i].data;
}
//Load a new shadow cube up with the old ones corners
void Megaminx::LoadNewCornersFromOtherCube(Megaminx* source)
{
    for (int i = 0; i < numCorners; ++i)
        this->corners[i].data = source->corners[i].data;
}

//Generic template to reset all the pieces. simple.
template <typename T>
int Megaminx::resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve) {
    if (std::is_same<T, Edge>::value)
        return resetFacesEdges(color_n, defaultPieces, solve);
    else if (std::is_same<T, Corner>::value)
        return resetFacesCorners(color_n, defaultPieces, solve);
    return 0;
}
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

/**
 * \brief Static Free function. Takes camera position angles and tells what face
 * is most showing. Shortcut way of angle detection.
 * \param x camera_angleX
 * \param y camera_angleY
 * \return face # color-int (1-12) as result.
 */
int getCurrentFaceFromAngles(int x, int y)
{
    //Vars:
    constexpr int r = 72;       //face angles
    constexpr int d = r / 2;    //36 half of face
    constexpr int s = 60;  // or match START_ANGLE in main.cpp
    int face = 0;   //color-int (1-12) as result.
    //Angle Conditions:
    const bool y1  = y >= (s - d) && y <= (s + d);                  // 60
    const bool y1b = y >= (s + 240 - d) && y <= (s + 240 + d);      //300 (other opposite)
    const bool y2  = y >= (s + 180 - d) && y <= (s + 180 + d);      //240
    const bool y2b = y >= (s +  60 - d) && y <= (s +  60 + d);      //120 (other opposite)
    const bool y3  = y >= (s + 120 - d) && y <= (s + 120 + d);      //180
    const bool y4a = y >= (0 - d)   && y <= (0 + d);                //0
    const bool y4b = y >= (360 - d) && y <= (360 + d);              //360
    //Edited so horizontal mouse-movement isnt backwards anymore when cube is vertically inverted (white face up), reliant on fix w/ Camera.cpp@Line126
    constexpr int toplistA[5] = { 12,11,10,9,8 };
    constexpr int toplistB[5] = { 3,4,5,6,2 };
    constexpr int botlistA[5] = { 11,12,8,9,10 };
    constexpr int botlistB[5] = { 4,3,2,6,5 };
    if (y1 && x < d)
        face = LIGHT_BLUE;
    else if (y2 && x < d)
        face = DARK_BLUE;
    if (face) return face;
    for (int i = 0; i < 5; ++i) {
        if (x >= d + r * i && x < d + r * (i + 1)) {
            if (y1)
                face = toplistA[i];
            else if (y2)
                face = toplistB[i];
            if (face) return face;
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (x >= r * i && x < r * (i + 1)) {
            if (y1b)
                face = botlistA[i];
            else if (y2b)
                face = botlistB[i];
            if (face) return face;
        }
    }
    if (y3 && !face)    //Bottom {1}
        face = WHITE;
    else if ((y4a || y4b) && !face) //Top {7}
        face = GRAY;
    return face;
}

/**
 * \brief Algorithm Switcher Dispatcher. Queues multiple rotate ops to happen
 * in accordance with how a player would want to achieve certain swaps.
 * \param current_face from 1 - 12
 */
void Megaminx::rotateAlgo(int current_face, int i)
{
    assert(current_face > 0 && current_face <= numFaces);
    const colordirs &loc = g_faceNeighbors[current_face];
    switch (i) {
    case 9991:
    case 1:
        // most common one, suitable for white corners or any.
        rotateBulkAlgoString("r u R' U'");
        break;
    case 9992:
    case 2:
        // opposite pair to first one
        rotateBulkAlgoString("l u L' U'");
        break;
    case 9993:
    case 3:
        //simple L#2-Edges - (opposite is case#2)
            //https://youtu.be/PWTISbs0AAs?t=493 og video., Insert to Left = This First, then next.
        rotateBulkAlgoString("U' L' u l");
        break;
    case 9994:
    case 100:
        //simple L#2-Edges - (opposite is case#1) 
            //same as ^ video: Insert to Right = This first, then previous.
        rotateBulkAlgoString("u r U' R'");
        break;
    case 9995:
    case 7:
        //Last Layer Step 4: Orientating (color-flipping) the Corners. (gray on top)
            //You repeat this over and over with all corners until they are all orientated correctly.
            //identity function repeats every 6x. Each gray color will take 2x cycles to colorflip. It will dis-align the R and D faces temporarily.
            //NOTE: This may result in a catch-22 where the 1 last gray corner piece is color-flipped but everything else is solved. Not sure what then.
            //Note2: this step is also done in the White Corners stage by repetitions of 3
        rotateBulkAlgoString("R' DR' R DR");
        break;
    case 9996:
    case 208: //repeat two times
        for (int i = 0; i < 2; ++i)
            rotateBulkAlgoString("R' DR' R DR");
        break;
    case 9997:
    case 207: //repeat four times
        for (int i = 0; i < 4; ++i)
            rotateBulkAlgoString("R' DR' R DR");
        break;
    case 99910:
    case 5:
        //Last Layer: Step 3 - Orient bottom Corners #1,2,3 // Put the corners into their correct positions. (gray on top)
            //ONLY affects Corners. //repeat 3x = undo
            //the 3, 5 and 7 o clock corners will rotate with each other  //moves Corners from #1to2,2to3,3to1
            //left+rear-side (9:00 to 12:00) and back-left's 2corner+3edges will stay the SAME.
        rotateBulkAlgoString("u l U' R' u L' U' r");
        break;
    case 99911:
    case 6:
        //Last Layer: Step 3 - Orient rear Corners #3,5,4 // Put the corners into their correct positions. (gray on top)
            //ONLY affects Corners. //repeat 3x = undo 
            // The front face corners (1&2) at the 5 and 7 o'clock will stay same,
            // The 3 affected corners will cycle rotate around counter-clockwise.
        rotateBulkAlgoString("u r 2U' L' 2u R' 2U' l u");
        break;
    case 99912:
    case 10:
        //Last Layer: Step 2/3 - Corner+Edge Permutation 3: (gray on top)  // (5 to 1, 1 to 2, 2 to 5)
            // Front face is untouched. (front 2 corners, front/left 2 edges) Rotates chunks of 2 clockwise
            //NOTE: CORNERS ARE AFFECTED by this too.  // 3 repeats = undo
        rotateBulkAlgoString("r u R' F', r  u  R' U', R' f r2 U' R'");
        break;
    case 99913:
    case 12:
        //Last Layer: Step 2/3 - Corner+Edge Permutation 5: (gray on top)
            //(5 and 3 swap, 1 and 2 swap) //1 and 5, 4 and 2
            //NOTE: CORNERS ARE AFFECTED by this edge algo,   //1 Repeat = Undo            
        rotateBulkAlgoString("l r u2, L' u R', l U' r u2, L' u2 R'");
        break;
    case 99914:
    case 4:
        //Last Layer: Step 2/3 (gray on top): main
            // Rotates the far 3 Star/Edge pieces into their correct position + (Affects corners too)
            //The nearest 1 corner and 2 edges stay the same (Corner 1 and Edge 1/5 remain untouched)
            // (The 6 and 8 o'clock pieces will remain unaffected and in their same position)
            //The remaining 3 will rotate cyclicly in an Anti Clockwise fashion. (same color)
            //#2. Two Edges Solved: (Solved edge in the front & lower left) 
        rotateBulkAlgoString("r u R' u r 2U' R'");
        break;
    case 99915:
    case 209:
        //Last Layer: Step 2/3: (gray on top), continued
            //#1. Two Edges Solved : (Solved edge in the front & upper right)
            //#3. One Edge is Permuted : (Permuted edge in the front) this algo + then go back do the previous algo)
        rotateBulkAlgoString("r u2, R' u, r u2, R'");
        break;
    case 99916:
    case 8:
        //Last Layer: Edge Permutation 1: (gray on top) // (5 to 2, 2 to 4, 4 to 5), 8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
            //6 o'clock and 1 o'clock STAY the same. Left Star Arrow -> rotate others Counter-Clockwise
            //ONLY Affects Edges & needs 5 executions; Called on Front Face. Affects top gray face's 3 edges
        for (int i = 0; i < 5; ++i)
            rotateBulkAlgoString("r2 U2' R2' U' r2 U2' R2'");
        // 13 moves * Repeated 5 times = Total 65 moves.
        break;
    case 99917:
    case 9:
        //Last Layer: Edge Permutation 2: (gray on top) // (5 to 4, 4 to 2, 2 to 5) (opposite of previous; all the "up"s get reversed)
            //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others ClockWise
            //ONLY Affects Edges & needs 5 executions; Called on Front Face. Affects top gray face's 3 edges
        for (int i = 0; i < 5; ++i)
            rotateBulkAlgoString("r2 u2 R2' u r2 u2 R2'");
        //13 moves * Repeated 5 times = Total 65 moves.
        break;
    case 99918:
    case 11:
        //Last Layer: Edge Permutation 3a+: (gray on top) // (5 to 2, 2 to 1, 1 to 5)
            //opposite of the previous one #3 above , but corners aren't affected...
            //ONLY Affects Edges, only needs one run.
            //11 o'clock to 4 o'clock, 4 o'clock to 1 o'clock, 1 o'clock to 11 o'clock        
            // Unaffecteds(2) = stay on front/left sides        
        rotateBulkAlgoString("r u R' u,  R' U' r2 U',  R' u R' u,  r U2'");
        break;
    case 99919:
    case 201:
        //Last Layer: Edge Permutation 3b-: (gray on top)
            //Opposite of EdgePermutation4a. Reverses #4A only when 3 edges are positioned in the front row,
            // Unaffecteds(2) = stay on both/back sides. Cycles edges in the opposite rotation.
            //manually reverse engineered from 4, to be equal to #3 but without affecting corners.
            //Reverses 4a if cube is rotated 2 turns CW.
        rotateBulkAlgoString("r' u' r u', r u r2' u, r u' r u', r' u2");
        break;
    case 99920:
    case 202:
        //Last Layer: Edge Permutation 3c-: (gray on top)
            // Unaffecteds(2) = right/back side untouched. edges cycle rotate = clockwise
            //Reverses 4a if cube is rotated 2 turns CCW.
            //Identical Twin to 4b but Rotates the front face instead of right
        rotateBulkAlgoString("f' u' f u', f u f2' u, f u' f u', f' u2");
        break;
    case 99921:
    case 203:
        // #7Last-Layer: Edge 5-way star scramble, (gray on top) 
            //Opposites Clockwise, 1 to 4, 4 to 2, 2 to 5, 5 to 3, 3 to 1 //1,3,5,2,4
            //ONLY Affects Edges (all 5)
            //60 moves total. copied from cube manual (turned upside down).
        for (int i = 0; i < 6; ++i)
            rotateBulkAlgoString("l' u2 r u2' l u2 r'");
        break;
    case 99922:
    case 204:
        // #7Last-Layer: Edge 5-way star scramble, (gray on top)
            // Two halves //2,1,3,4,5   //ONLY Affects Edges (all 5)
            //60 moves total. (copied from cube manual)
        rotateBulkAlgoString("r' l u' r l' u2");
        for (int i = 0; i < 6; ++i)
            rotateBulkAlgoString("r' l u2' r l' u2");
        rotateBulkAlgoString("r' l u' r l'");
        break;
    case 99923:
    case 205:
        // #7Last-Layer: Edge two swaps, (gray on top)
            //have 1 edge solved (remains in front), then swap 2&3 and 4&5
            //44 moves total. (copied from manual. )
        rotateBulkAlgoString("u2' l2' u2' l2 u' l2' u2' l2 u'");
        rotateBulkAlgoString("    l2' u2' l2 u' l2' u2' l2 u'");
        rotateBulkAlgoString("    l2' u2' l2 u' l2' u2' l2 u ");
        break;
    case 99924:
    case 206:
        //#7Last-Layer: Edge swap & Invert, (gray on top)
            //have 1 edge solved (remains in front), then swap 2&4 and 3&5 /INVERTED.
            //right/backRight swap and left/backLeft swap // with INVERTS @ 8 o'clock and 1 o'clock <- 
            //30 moves total. (copied from manual.)
        rotateBulkAlgoString("r' l f2' r l' u2 r' l");
        rotateBulkAlgoString("f' r l' u2' r' l f2' r");
        rotateBulkAlgoString("l' u2 r' l f' r l' u2'");
        break;
    case 99925:
    case 13:
        // #2nd-Layer Edges(LEFT) =  7 o'clock to 9 o'clock:
            //reverse engineered from #14 myself (Exact opposite)
        rotateBulkAlgoString("dl l dl l' dl' f' dl' f");
        break;
    case 99926:
    case 14:
        // #2nd-Layer Edges(RIGHT) =  5 o'clock to 3 o'clock:
            //Algo from QJ cube manual & (White face on top)
        rotateBulkAlgoString("dr' r' dr' r dr f dr f'");
        break;
    case 99927:
    case 15:
        // #4th-Layer Edges(LEFT), (between the middle W), fourthLayerEdgesA() // 12 o'clock to 7 o'clock
            //Cube must have gray side on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
        rotateBulkAlgoString("F' R', F' F', r f");
        break;
    case 99928:
    case 16:
        // #4th-Layer Edges(RIGHT), (between the middle W), fourthLayerEdgesB() //12 o'clock to 5 o'clock.
            //Cube must have gray side on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
        rotateBulkAlgoString("f l, f f, L' F'");
        break;
    case 99929:
    case 17:
        // #6th-Layer Edges(LEFT) //Must have Layers 1-5 solved, and 7th layer is affected.
            // swap edge from face's star at 12 o'clock to Flop in (pinned to center) To the edge @  9 o'clock
            //https://www.youtube.com/watch?v=j4x61L5Onzk
        rotateBulkAlgoString("U' L' u l u f U' F'");
        break;
    case 99930:
    case 18:
        // #6th-Layer Edges(RIGHT) //opposite of previous, To the edge @ 3 o'clock
        rotateBulkAlgoString("u r U' R' U' F' u f");
        break;
    default:
        break;
    }
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
            if      ((word.find("dr") !=npos) ||
                    (word.find("DR") !=npos))
                op.num = loc.downr - 1;
            else if ((word.find("dl") !=npos) ||
                     (word.find("DL") !=npos))
                op.num = loc.downl - 1;
            else if ((word.find("r") !=npos) ||
                     (word.find("R") !=npos))
                op.num = loc.right - 1;
            else if ((word.find("l") !=npos) ||
                     (word.find("L") !=npos))
                op.num = loc.left - 1;
            else if ((word.find("f") !=npos) ||
                     (word.find("F") !=npos))
                op.num = loc.front - 1;
            else if ((word.find("u") !=npos) ||
                     (word.find("U") !=npos))
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

void Megaminx::secondLayerEdges() {
    constexpr int secondLayerEdges[5] = { 5, 6, 7, 8, 9 };
    resetFiveEdges(secondLayerEdges);
}
void Megaminx::fourthLayerEdges() {
    constexpr int fourthLayerEdgesA[5] = { 10, 11, 12, 13, 14 };
    resetFiveEdges(fourthLayerEdgesA);
    constexpr int fourthLayerEdgesB[5] = { 15, 16, 17, 18, 19 };
    resetFiveEdges(fourthLayerEdgesB);
}
void Megaminx::sixthLayerEdges() {
    constexpr int sixthLayerEdges[5] = { 20, 21, 22, 23, 24 };
    resetFiveEdges(sixthLayerEdges);
}
void Megaminx::lowYmiddleW() {
    constexpr int pieceListA[5] = { 5, 6, 7, 8, 9 };
    resetFiveCorners(pieceListA);
}
void Megaminx::highYmiddleW() {
    constexpr int pieceListB[5] = { 10, 11, 12, 13, 14 };
    resetFiveCorners(pieceListB);
}

bool Megaminx::shadowMultiRotate(int face, int &offby, Megaminx* shadowDom)
{
    int defaultDir = Face::CW;
    if (offby < 0) {    //negative reverses direction
        offby *= -1;
        defaultDir *= -1;
    }
    if (offby > 0) {
        //Save moves by never going past 5
        while (offby >= 5)
            offby -= 5;
        //Saves moves by going the opposite direction:
        if (offby > 2.5) {
            offby = (offby - 5) * -1;
            defaultDir *= -1;
        }
        for (int j = 0; j < offby; ++j)
            shadowDom->shadowRotate(face, defaultDir);
    }
    return (offby > 0);
}

void Megaminx::updateRotateQueueWithShadow(Megaminx* shadowDom)
{
    size_t numsize = shadowDom->shadowRotateQueue.size();
    if (numsize <= 0) return;
    undoStack.push({ -999, -999 });
        for (int q = 0; q < numsize; ++q) {
            auto op = shadowDom->shadowRotateQueue.front();
            rotateQueue.push(op);
            shadowDom->shadowRotateQueue.pop();
            undoStack.push(op);
        }
    undoStack.push({ 999, 999 });
}

//This is more complex than normal because its the most flexible
void Megaminx::DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5])
{
    int endI = startI + 5;
    std::vector<int> piecesSeenOnTop;
    //populate piecesSolved
    int numSolved = 0;
    //Find out if any applicable pieces are in the desired slots:
    for (int p = startI; p < endI; ++p) {
        int pIndex = findEdge(p);
        //make sure its a match and make sure the colors arent flipped
        if (pIndex >= startI && pIndex < endI && edges[pIndex].data.flipStatus == 0)
            piecesSeenOnTop.push_back(p- startI);
        //If we found the correct piece solved in the correct spot
        if (p == pIndex && edges[pIndex].data.flipStatus == 0) {
            piecesSolved[p- startI] = true;
            numSolved++;
        }
    }
    if (piecesSeenOnTop.size() > 1) {
        //Check if the ordering blue->red,red->green is correct,etc... even if the top is twisted vs the sides
        for (int p = startI; p < endI; ++p) {
            int pNext = (p != (endI-1)) ? p + 1 : startI; //handle the loop numbering boundary overrun
            int pIndex = findEdge(p);
            int pNextIndex = findEdge(pNext);
            if (((pIndex >= startI && pIndex < endI) && (pNextIndex >= startI && pNextIndex < endI)) &&
                edges[pIndex].data.flipStatus == 0 && edges[pNextIndex].data.flipStatus == 0 &&
                ((pNextIndex == pIndex + 1) || (pIndex == (endI-1) && pNextIndex == startI))) {
                piecesSolved[p- startI] = true;
                piecesSolved[pNext- startI] = true;
                numSolved++;
            }
        }
    }
    //Fallback to at least get first piece solved
    if ((numSolved == 0 && piecesSeenOnTop.size() > 1) || piecesSeenOnTop.size() == 1) {
        std::sort(piecesSeenOnTop.begin(), piecesSeenOnTop.end());
        piecesSolved[piecesSeenOnTop[0]] = true;
        numSolved++;
    }
}

//Generic template way to detect if pieces are solved in their correct locations with correct colors
template <typename T>
void Megaminx::DetectSolvedPieces(int startI, bool piecesSolved[5])
{
    int endI = startI + 5;
    //Find out if any applicable startI-endI pieces are exactly in their slots.
    for (int p = startI; p < endI; ++p) {
        int pIndex = findPiece<T>(p);
        Piece* piece = getPieceArray<T>(pIndex);
        //make sure its startI-endI and make sure the colors arent flipped
        if (pIndex >= startI && pIndex < endI && p == pIndex && piece->data.flipStatus == 0)
            piecesSolved[p - startI] = true;
    }
} //where T = Corner or Edge
void Megaminx::DetectSolvedCorners(int startI, bool piecesSolved[5]) {
    DetectSolvedPieces<Corner>(startI, &piecesSolved[0]);
}
void Megaminx::DetectSolvedEdges(int startI, bool piecesSolved[5]) {
    DetectSolvedPieces<Edge>(startI,  &piecesSolved[0]);
}

class LayerAssist {
public:
    int sourceEdgeIndex;
    colorpiece edgeFaceNeighbors;
    int edgeFaceLocA;
    int edgeFaceLocB;
    int dirToWhiteA;
    int dirToWhiteB;
    Edge* EdgeItselfA;
    Edge* EdgeItselfB;
    int whichcolorEdgeA;
    int whichcolorEdgeB;
    int edgeHalfColorA;
    int edgeHalfColorB;
    bool colormatchA;
    bool colormatchB;
    bool isOnRow1;
    bool isOnRow2;
    bool isOnRow3;
    bool isOnRow4;
    bool isOnRow5;
    bool isOnRow6;
    bool isOnRow7;
    bool ontopA;
    bool ontopB;

    LayerAssist(Megaminx* shadowDom, int i) {
        sourceEdgeIndex = shadowDom->findEdge(i);
        auto currentPiece = shadowDom->getPieceArray<Edge>(sourceEdgeIndex);
        auto currentpieceColor = currentPiece->data._colorNum[0];
        //Determine which two faces the edge belongs to:
        edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, from 0-4
        edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        EdgeItselfB = shadowDom->faces[edgeFaceNeighbors.b - 1].edge[edgeFaceLocB];
        assert(EdgeItselfA == EdgeItselfB); //sanity check.        
        //Determine which direction those faces need to rotate to land the Edge on the white
        dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        //Use reference table to check edge internal color data struct-order.
        whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        assert(whichcolorEdgeA + whichcolorEdgeB == 1); //just makes sure the reference table is accurate, it is.        
        //Determine which color half-edge is on each face
        edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        edgeHalfColorB = EdgeItselfB->data._colorNum[whichcolorEdgeB];
        colormatchA = edgeHalfColorA != WHITE;
        colormatchB = edgeHalfColorB != WHITE;
        assert(colormatchA != colormatchB); //sanity check.
        //Line up things that are solved on the top face.
        isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        isOnRow4 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20);
        isOnRow5 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        //Check if the faces we have are considered as the Top half of the cube.
        ontopA = (edgeFaceNeighbors.a > 1 && edgeFaceNeighbors.a < 7);
        ontopB = (edgeFaceNeighbors.b > 1 && edgeFaceNeighbors.b < 7);
    }
};

//Layer 1 part 1 - White Edges
void Megaminx::rotateSolveWhiteEdges(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    int i = 0;
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {        
        //temporary overflow protection:
        if (loopcount > 101)
            break;
        bool facesSolved[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
        bool piecesSolved[5] = { false, false, false, false, false };
        int firstSolvedPiece = -1;
        shadowDom->DetectSolvedEdgesUnOrdered(0, piecesSolved);
        for (int a = 0; a < 5; ++a) {
            facesSolved[1 + a] = piecesSolved[a];
            if (firstSolvedPiece == -1 && piecesSolved[a] == true)
                firstSolvedPiece = a;
        }
        while (i < 5 && piecesSolved[i] == true)
            i++;
        if (i >= 5) {
            allSolved = true;
            break;
        }
        
        //Rotates the white face to its solved position, first solved edge matches up to its face.
        if (firstSolvedPiece != -1) {
            //NOTE: Doing this over and over is wasting moves solving the partial-solved top every time.
            //TODO: This means we need a plan for any 5-9 edge to get moved into any 0-4 slot even when top isn't solved.
            int findIfPieceSolved = shadowDom->findEdge(firstSolvedPiece); //always piece 0
            int offby = findIfPieceSolved - firstSolvedPiece;    //example: piece 0, 5 - 5 - 0 = 0, so no correction.
            if (findIfPieceSolved > 0 && findIfPieceSolved < 5 && offby > 0) {
                offby *= -1;
                shadowMultiRotate(WHITE, offby, shadowDom);
                continue;
            }
        }
        LayerAssist l{ shadowDom,i };
        //Any matching pieces that end up on its matching face can be spun in with just 2 or 1 moves.
        if (l.ontopA && ((l.isOnRow4 && l.colormatchA) || l.isOnRow2)) {
            int offby = l.colormatchA ? (l.edgeFaceNeighbors.a - l.edgeHalfColorA) : (l.edgeFaceNeighbors.b - l.edgeHalfColorB);
            //Set up Rotated White top to be in-line with the face we want to spin in.
            shadowMultiRotate(WHITE, offby, shadowDom);
            if (l.isOnRow4 && l.colormatchA) {
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            }
            else if (l.isOnRow2 && l.colormatchA)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            else if (l.isOnRow2 && l.colormatchB)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        //Locates any straggler pieces on the bottom and bubbles them up to the top layers, as long as the face isnt protected by facesSolved pieces
        else if (l.isOnRow4 && l.dirToWhiteA != 0 && ((l.edgeFaceNeighbors.a < GRAY && !facesSolved[l.edgeFaceNeighbors.a - 1]) || l.edgeFaceNeighbors.a >= GRAY)) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        }
        else if (l.isOnRow4 && l.dirToWhiteB != 0 && ((l.edgeFaceNeighbors.b < GRAY && !facesSolved[l.edgeFaceNeighbors.b - 1]) || l.edgeFaceNeighbors.b >= GRAY)) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
            //REFRESH
            int sourceEdgeIndexNext = shadowDom->findEdge(i);
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == l.edgeFaceNeighbors.b)
                //check if the next neighbor face is Solved aka blocked ?
                if (facesSolved[edgeFaceNeighborsNext.a - 1])
                    //make sure its the move going to the right
                    if (edgeFaceNeighborsNext.a == g_faceNeighbors[l.edgeFaceNeighbors.a].right)
                        shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        else if (l.isOnRow6 || l.isOnRow7 && l.dirToWhiteA != 0)
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        else if (l.isOnRow6 || l.isOnRow7 && l.dirToWhiteB != 0)
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        //These can still trigger if the top pieces are in the top row but the wrong slot (or are color-flipped)
        else if (l.isOnRow1 && l.dirToWhiteA != 0 && !facesSolved[l.edgeFaceNeighbors.a - 1])
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        else if (l.isOnRow1 && l.dirToWhiteB != 0 && !facesSolved[l.edgeFaceNeighbors.b - 1])
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //If its solved, get top white face spun oriented back to normal
    if (allSolved) {
        int findIfPieceSolved = shadowDom->findEdge(0); //always piece 0
        if (findIfPieceSolved > 0 && findIfPieceSolved < 5) {
            findIfPieceSolved *= -1;
            shadowMultiRotate(WHITE, findIfPieceSolved, shadowDom);
        }
    }
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 1 part 2 - White Corners
void Megaminx::rotateSolveWhiteCorners(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //temporary overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedCorners(0, piecesSolved);
        int i = 0; //the starting piece
        while (i < 5 && piecesSolved[i] == true)
            i++;
        if (i >= 5) {
            allSolved = true;
            continue;
        }
        int sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        colorpiece cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the corners are positioned, 0-4
        int cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        int cornerFaceLocB = shadowDom->faces[cornerFaceNeighbors.b - 1].find5CornerLoc(i);
        assert(cornerFaceLocB != -1); //should not happen
        int cornerFaceLocC = shadowDom->faces[cornerFaceNeighbors.c - 1].find5CornerLoc(i);
        assert(cornerFaceLocC != -1); //should not happen
        Corner* CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceCornerIndex >= 0 && sourceCornerIndex < 5);
        bool isOnRow2 = (sourceCornerIndex >= 5 && sourceCornerIndex < 10);
        bool isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        bool isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        //New breakthrough idea, any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        bool ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        bool ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        bool ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);        
        //Rotates the white face to its solved position, first solved EDGE matches up to its face.
        //Edges should already be solved, if not, get top white face spun oriented back to normal
        int edgesOffBy = shadowDom->findEdge(0); //always piece 0
        if (edgesOffBy > 0 && edgesOffBy < 5) {
            edgesOffBy *= -1;
            shadowMultiRotate(WHITE, edgesOffBy, shadowDom);
        }
        //Move incorrect corners out of the 0-4 slots moves them right down with the algo
        else if (isOnRow1 && ((i != sourceCornerIndex) || (i == sourceCornerIndex && CornerItselfA->data.flipStatus != 0)) && piecesSolved[i] == false) {
            int offby = RED;
            if (cornerFaceNeighbors.a == WHITE)
                offby += cornerFaceLocA;
            else if (cornerFaceNeighbors.b == WHITE)
                offby += cornerFaceLocB;
            else if (cornerFaceNeighbors.c == WHITE)
                offby += cornerFaceLocC;
            if (offby > 6)
                offby -= 5;            
            if (offby >= 2) {
                const colordirs &loc = g_faceNeighbors[offby];
                std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("R' DR' R DR", loc);
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num+1, op.dir);
            }
        }
        //Move correct corners straight up and in from 5-9 to 0-4
        else if (isOnRow2 && sourceCornerIndex - i - 5 == 0) {
            int offby = sourceCornerIndex - 5 + RED;
            if (offby > 6)
                offby -= 5;
            if (offby >= 2) {
                const colordirs &loc = g_faceNeighbors[offby];
                std::vector<numdir> bulk;
                if (CornerItselfA->data.flipStatus == 2)
                    bulk = shadowDom->ParseAlgorithmString("f dr F'", loc);
                else
                    bulk = shadowDom->ParseAlgorithmString("R' DR' R", loc);
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num+1, op.dir);
            }
        }
        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        else if (isOnRow2) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 5;
            megaminxColor turnface;
            //2nd rotation puts them on the gray face.
            if (ontopA && ontopB)
                turnface = cornerFaceNeighbors.c;
            else if (ontopA && ontopC)
                turnface = cornerFaceNeighbors.b;
            else if (ontopB && ontopC)
                turnface = cornerFaceNeighbors.a;
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
        }
        //Row 3 pieces go to gray face as temporary holding (1 CW turn) (ends up on row4)
        else if (isOnRow3) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 10;
            int x, y;
            if (ontopA) {
                x = cornerFaceNeighbors.b;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopB) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopC) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.b;
            }
            int result = max(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = LIGHT_BLUE;
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2
        else if (isOnRow4) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - i - 18;
            shadowMultiRotate(GRAY, offby, shadowDom);
            int x = PINK + (i);
            if (x > BEIGE)
                x -= 5;
            shadowDom->shadowRotate(x, defaultDir);
            shadowDom->shadowRotate(x, defaultDir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 2 - Edges
void Megaminx::rotateSolveLayer2Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedEdges(5, piecesSolved);
        int i = 5; //the starting piece
        while (i < 10 && piecesSolved[i - 5] == true)
            i++;
        if (i >= 10) {
            allSolved = true;
            break;
        }
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        //Mark where edge-halves have neighbor faces belonging to the top-half of cube.
        bool ontopA = (edgeFaceNeighbors.a > 1 && edgeFaceNeighbors.a < 7);
        bool ontopB = (edgeFaceNeighbors.b > 1 && edgeFaceNeighbors.b < 7);
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        bool isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        bool isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        bool isOnRow34 =(sourceEdgeIndex >= 10 && sourceEdgeIndex < 20); //Middle W
        bool isOnRow4 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);

        if ((isOnRow2 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow34 && ontopA && edgeFaceLocA == 4 && edgeFaceNeighbors.a == edgeHalfColorA)) {
            const colordirs &loc = g_faceNeighbors[edgeFaceNeighbors.a];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("dl l dl l' dl' f' dl' f", loc); // left
            for (auto op : bulk)    //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else if (isOnRow34 && ontopA && edgeFaceLocA == 3 && edgeFaceNeighbors.a == edgeHalfColorA) {
            const colordirs &loc = g_faceNeighbors[edgeFaceNeighbors.a];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("dr' r' dr' r dr f dr f'", loc); // right
            for (auto op : bulk)    //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        //BUG?: Pieces still go the long-way around from 7->6->3->(skip)4->6->7->6->4 then algo....
        //TODO: Need to detect when the face matches up then go backwards.
        else if (isOnRow34 && dirToWhiteB != 0 && edgeFaceNeighbors.b >= GRAY) {
            shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
            int sourceEdgeIndexNext = shadowDom->findEdge(i);   //REFRESH
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == edgeFaceNeighbors.b)
                //check the solved for the next neighbor (vs what if we just check the original, face.loc.right) ?
                //make sure its the move going to the right
                if (edgeFaceNeighborsNext.a == g_faceNeighbors[edgeFaceNeighbors.a].right)
                    shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
        }
        //Layer 6 pieces are moved down to gray layer 7
        else if (isOnRow6) {
            if (dirToWhiteA != 0)
                shadowDom->shadowRotate(edgeFaceNeighbors.a, -1 * dirToWhiteA);
            else if (dirToWhiteB != 0)
                shadowDom->shadowRotate(edgeFaceNeighbors.b, -1 * dirToWhiteB);
        }
        //Layer 7 is an intermediate buffer
        else if (isOnRow7) {
            int row7 = sourceEdgeIndex - 28 - i;
            bool moved = shadowMultiRotate(GRAY, row7, shadowDom);
            //Align the GRAY layer 7 to be directly underneath the intended solve area
            if (moved == false) {
                if (dirToWhiteA != 0) {
                    shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
                    shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
                }
                else if (dirToWhiteB != 0) {
                    shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
                    shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
                }
            }
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 3 - Corners
void Megaminx::rotateSolve3rdLayerCorners(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedCorners(5, piecesSolved);
        int i = 5; //the starting piece
        while (i < 10 && piecesSolved[i - 5] == true)
            i++;
        if (i >= 10) {
            allSolved = true;
            break;
        }
        int sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        colorpiece cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the corners are positioned, 0-4
        int cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        Corner* CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceCornerIndex >= 0 && sourceCornerIndex < 5);
        bool isOnRow2 = (sourceCornerIndex >= 5 && sourceCornerIndex < 10);
        bool isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        bool isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        //New breakthrough idea, any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        bool ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        bool ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        bool ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);

        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        if (isOnRow2) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 5;
            megaminxColor turnface;
            //2nd rotation puts them on the gray face.
            if (ontopA && ontopB)
                turnface = cornerFaceNeighbors.c;
            else if (ontopA && ontopC)
                turnface = cornerFaceNeighbors.b;
            else if (ontopB && ontopC)
                turnface = cornerFaceNeighbors.a;
            //If the piece is solved positioned, but color flipped wrong:
            if (sourceCornerIndex == i)
                defaultDir *= -1;
            // turn opposite direction row2->row3->row4 to use gray face then repeat
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
        }
        //Row 3 pieces go to gray face as temporary holding (1 CCW turn) (ends up on row4)
        else if (isOnRow3) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - i - 10;
            int x, y;
            if (ontopA) {
                x = cornerFaceNeighbors.b;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopB) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopC) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.b;
            }
            //The loop point crosses over at the Pink 
            if (x < PINK)
                x += 5;
            if (y < PINK)
                y += 5;
            int result = max(x, y);
            if (result > BEIGE)
                result -= 5;
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2 Solved
        else if (isOnRow4) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - (i - 5) - 18;
            shadowMultiRotate(GRAY, offby, shadowDom);
            int x = PINK + (i - 5);
            if (x > BEIGE)
                x -= 5;            
            shadowDom->shadowRotate(x, defaultDir);
            shadowDom->shadowRotate(x, defaultDir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 4 - Edges
//Cube must have gray side on top, layer 1+2+3 Solved and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer4Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[10] = { false, false, false, false, false, false, false, false, false, false };
        shadowDom->DetectSolvedEdges(10, piecesSolved);
        shadowDom->DetectSolvedEdges(15, &piecesSolved[5]);
        int i = 10; //the piece
        while (i < 20 && piecesSolved[i - 10] == true)
            i++;
        if (i >= 20) {
            allSolved = true;
            break;
        }
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        int edgeHalfColorB = EdgeItselfA->data._colorNum[whichcolorEdgeB];
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        bool isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        bool isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        bool isOnRow34 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20); //Middle W
        bool isOnRow4 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        bool graymatchA = edgeFaceNeighbors.a == GRAY;
        bool graymatchB = edgeFaceNeighbors.b == GRAY;
        //drop-in directions: left is row 4, right is row 3 (determined from original I, not sourceEdgeIndex)
        bool isRight = (i >= 10 && i < 15);
        bool isLeft  = (i >= 15 && i < 20);
        //Get ready for algorithms
        if ((isOnRow34 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow7)) {
            if (isOnRow7) {
                //Align GRAY top to the exact position for pre-drop-in.
                int offby = sourceEdgeIndex - 27 - i;
                if (isLeft) //row 4B's B-half is +1 from row4A's B-half (DARK_BLUE,LIGHT_GREEN) vs (DARK_BLUE,PINK)
                    offby -= 1;
                bool moved = shadowMultiRotate(GRAY, offby, shadowDom);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
            }
            //obtain the non-gray face neighbor we need to be rotating
            colordirs loc;
            if (isOnRow7)
                loc = (graymatchA) ? g_faceNeighbors[edgeFaceNeighbors.b] : g_faceNeighbors[edgeFaceNeighbors.a];
            else if(isOnRow34)
                loc = g_faceNeighbors[max(edgeFaceNeighbors.b,edgeFaceNeighbors.a)];
            //works to insert pieces from row7 to 3/4 and also pops wrong pieces out from 3/4 to 6
            std::vector<numdir> bulk;
            if ((isOnRow7 && isLeft) || (isOnRow4)) {
                if (EdgeItselfA->data.flipStatus == 0 || (isOnRow4))
                    bulk = shadowDom->ParseAlgorithmString("F' R', F' F', r f", loc);  // left
                else
                    bulk = shadowDom->ParseAlgorithmString("U' R' DR' F F DR R", loc); //l+inverted
            }
            else if ((isOnRow7 && isRight) || (isOnRow3)) {
                if (EdgeItselfA->data.flipStatus == 0 || (isOnRow3))
                    bulk = shadowDom->ParseAlgorithmString("f l, f f, L' F'", loc);    // right
                else
                    bulk = shadowDom->ParseAlgorithmString("u l dl F' F' DL' L'", loc); //r+inverted
            }
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        //Row 6 - flip piece up to GRAY, then return the moved faces to unharm the low corners.
        else if (isOnRow6) {
            if (dirToWhiteA != 0) {
                shadowDom->shadowRotate(edgeFaceNeighbors.a, -1 * dirToWhiteA);
                shadowDom->shadowRotate(GRAY, dirToWhiteA);
                shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
            }
            else if (dirToWhiteB != 0) {
                shadowDom->shadowRotate(edgeFaceNeighbors.b, -1 * dirToWhiteB);
                shadowDom->shadowRotate(GRAY, dirToWhiteB);
                shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
            }
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 5 - Corners
void Megaminx::rotateSolve5thLayerCorners(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedCorners(10, piecesSolved);
        int i = 10; //the starting piece
        while (i < 15 && piecesSolved[i - 10] == true)
            i++;
        if (i >= 15) {
            allSolved = true;
            break;
        }
        int sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        colorpiece cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Determine where on those faces the corners are positioned, 0-4
        int cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        Corner* CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        bool isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        bool isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        bool ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        bool ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        bool ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);
        //Get the Pieces to drop-in ready on Row 4 (gray layer) solved into Row3
        //Any Row 3 pieces that are mis-solved use same algo to go up to gray layer (ends up on row4)
        if (isOnRow3) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - i;
            int x, y;
            if (ontopA) {
                x = cornerFaceNeighbors.b;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopB) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopC) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.b;
            }
            int result = min(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = BEIGE;
            colordirs loc = g_faceNeighbors[result];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("r u R' U'", loc);  //right
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else if (isOnRow4) {
            //Orient Gray Top layer (index goes in reverse)
            int offby = sourceCornerIndex + (i - 10) - 20;
            shadowMultiRotate(GRAY, offby, shadowDom);
            //quick shortcut to know which face we're working on.
            int front = BEIGE - (i - 10);
            colordirs loc = g_faceNeighbors[front];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("r u R' U'", loc);  //right
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 6 - Edges
//Cube must have gray side on top, layer 1+2+3+4+5 Solved, and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer6Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedEdges(20, piecesSolved);
        int i = 20; //the starting piece
        while (i < 25 && piecesSolved[i - 20] == true)
            i++;
        if (i >= 25) {
            allSolved = true;
            break;
        }
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        int edgeHalfColorB = EdgeItselfA->data._colorNum[whichcolorEdgeB];
        //Only care about row6 and 7
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        //on Row7, One of these will be gray, so the other letter is the colored face target.
        bool graymatchA = edgeFaceNeighbors.a == GRAY;
        bool graymatchB = edgeFaceNeighbors.b == GRAY;
        //Get ready for algorithms
        if ((isOnRow6 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow7)) {
            if (isOnRow7) {
                assert(dirToWhiteA == 0);
                int offby = graymatchA ? (edgeFaceNeighbors.b - edgeHalfColorB) : (edgeFaceNeighbors.a - edgeHalfColorA);
                //Align GRAY top to the exact position for pre-drop-in.
                bool moved = shadowMultiRotate(GRAY, offby, shadowDom);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
            }
            //obtain the non-gray face neighbor we need to be oriented to
            int result = 0;
            int x = edgeFaceNeighbors.a;
            int y = edgeFaceNeighbors.b;
            if (isOnRow7)
                result = (graymatchA) ? y : x;
            else if (isOnRow6) {
                result = min(x, y);
                if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                    result = BEIGE;
            }
            colordirs loc = g_faceNeighbors[result]; //loc.front
            //Check left/right faces for which direction to drop-in
            bool isLeft =              isOnRow7 && (loc.left == edgeHalfColorA);
            bool isRight = isOnRow6 || isOnRow7 && (loc.right == edgeHalfColorA);
            //works to insert pieces from row7 to 6 and also pops wrong pieces out from 6 to 7
            std::vector<numdir> bulk;
            if (isLeft)
                bulk = shadowDom->ParseAlgorithmString("U' L' u l u f U' F'", loc); //left
            else if (isRight)
                bulk = shadowDom->ParseAlgorithmString("u r U' R' U' F' u f", loc); //right
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//TODO: Last Layer
//Layer 7 - Last Layer Edges (GRAY top)
//Cube must have gray side on top, layer 1+2+3+4+5+6 Solved
void Megaminx::rotateSolveLayer7Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube()
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        bool piecesSolvedStrict[5] = { false, false, false, false, false };
        int i = 25; //the starting piece
        shadowDom->DetectSolvedEdgesUnOrdered(i, &piecesSolved[0]);
        shadowDom->DetectSolvedEdges(i, &piecesSolvedStrict[0]);
        while (i < 30 && piecesSolved[i - 25] == true)
            i++;
        if (i >= 30) {
            allSolved = true;
            break;
        }
        //LayerAssist l{ shadowDom,i };
        int solvedCount = 0; 
        int firstSolvedPiece = -1;
        int firstUnsolvedPiece = -1;
        for (int k = 0; k < 5; ++k) {
            if (piecesSolved[k]) {
                solvedCount++;
                if (firstSolvedPiece == -1)
                    firstSolvedPiece = k;
            }
            if (piecesSolved[k] == false && firstUnsolvedPiece == -1)
                firstUnsolvedPiece = k;
        }
        //Rotates the GRAY face to any solved position, first out of order but solved EDGE rotates to match up to its face.
        if (piecesSolvedStrict[0] == false && firstSolvedPiece != -1 && piecesSolved[i-25] == false && solvedCount > (i-25)) {
            //NOTE: Doing this over and over is wasting moves potentially
            int findIfPieceSolved = shadowDom->findEdge(i+firstSolvedPiece);
            int offby = findIfPieceSolved - i + firstSolvedPiece;
            if (findIfPieceSolved >= 25 && findIfPieceSolved < 30 && offby != 0) {
                offby += 1; offby *= -1;
                shadowMultiRotate(GRAY, offby, shadowDom);
                break;
            }
            break;
        }
        //first and second piece solved for sure. 
        else if (solvedCount == 2 && piecesSolvedStrict[0] && piecesSolvedStrict[1]) {
            int sourceEdgeIndex = shadowDom->findEdge(i);
            int offby = sourceEdgeIndex - i;
            if (offby == 2) {
                colordirs loc = g_faceNeighbors[ORANGE];    //algo #11
                std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("r u R' u,  R' U' r2 U',  R' u R' u,  r U2'", loc);
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num + 1, op.dir);
                break;
            }
            else
                break;
            break;
        }
        else if (solvedCount >= 2 && piecesSolvedStrict[0] && piecesSolvedStrict[1] && i == shadowDom->findEdge(i)) {
            colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //algo #206
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("r' l f2' r l' u2 r' l f' r l' u2' r' l f2' r l' u2 r' l f' r l' u2'", loc);
            for (auto op : bulk)    //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
            break;
        }
        else
            break;
        //PSUEDOCODE
        /*----------
        Last Layer includes both sides of the edge and all corners.
        EDGES:
        Start finding out which Edges are OK
        OK means multiple correct pieces in order, or  1 piece's color facing correctly (gray),
        Does it Need to be rotated? must be considered...
        if 0/5, find how many colors are gray,
        If solved 1/5, find out which piece and try to solve the adjacent piece to it next.
        If solved 2/5 find out if they are attached. NO: If not, why? are they 3 away? Choose a three-way-opposite Algo
                                                     YES: Choose a three-ina-row Edge+/Corner algo
        If solved >=3/5 theres a problem. It goes from 2/5 to 5/5 ideally.
        CORNERS:
        Corners must keep the edges in place, we will know solved rotation by having the offby=0 as a query to solved findEdges above.
        If solved 1/5, determine if better to skip and start at 0 or continue with this.
         1pt2: check next piece, find out if its capable of being moved in 1 move, or if its 2 moves.
        If solved 2/5, find out if they are attached: NO: If not, why? are they 3 away? we have no three-way-opposite algo, must give up, go back to 1.
                                                     YES: Choose right three-in-a-row corner-only algo
        If solved 3/5, find out if they are attached: NO: If not, proceed to next.
                                                     YES: Choose right three-way corner-only algo
        */
        loopcount++;
    } while (!allSolved);

    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}
