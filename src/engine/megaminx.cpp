/* MegaMinx2 v1.34 - 2017+2018+2019 - genBTC mod
 * Uses code from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
 * genBTC November 2017 - genbtc@gmx.com / @genr8_ / github.com/genbtc/
 * genBTC December 2018 - fixups, tweaks.
 * genBTC January 2019 - human rotate auto solve layers 1-6
 */
#include "megaminx.h"

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
    for (auto& center : centers)
        center.render();
    for (auto& edge : edges)
        edge.render();
    for (auto& corner: corners)
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
    _rotate_internal({ num, dir });
}
//The most important rotate function - no validation. (internal,private)
void Megaminx::_rotate_internal(numdir i)
{
    rotateQueue.push({ i.num, i.dir });
    undoStack.push({ i.num, i.dir });
    //TODO: use undoStack as the written log history.
}
void Megaminx::shadowRotate(int num, int dir)
{
    assert(num > 0 && num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    num -= 1; //Convert 1-12 Faces into array [0-11]
    //_rotate_internal({ num, dir });
    shadowRotateQueue.push({ num, dir });
    faces[num].placeParts(dir);
}
//Adds entire vector of numdirs to the Rotate queue one by one.
void Megaminx::rotateBulkAlgoVector(std::vector<numdir> &bulk)
{
    for (auto one : bulk) {
        rotateQueue.push(one);
        undoStack.push(one);
    }
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
 * \brief Toggle the colors of a single Corner piece
 * * If called externally make sure its color 1-12.
 * \param i Nth-face's number (color) [0-11]
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::flipCornerColor(int i, int x)
{
    assert(i > 0 && i <= numFaces);
    assert(x > 0 && x <= 5);
    faces[i-1].corner[x-1]->flip();
}
/**
 * \brief Toggle the colors of a single Edge piece
 * * If called externally make sure its color 1-12.
 * \param i Nth-face's number (color) [0-11]
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::flipEdgeColor(int i, int x)
{
    assert(i > 0 && i <= numFaces);
    assert(x > 0 && x <= 5);
    faces[i-1].edge[x-1]->flip();
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
 * \return n same thing.
 */
void Megaminx::resetFace(int n)
{
    assert(n > 0 && n <= numFaces);
    resetFacesEdges(n);
    resetFacesCorners(n);
    setCurrentFaceActive(n);
}

/**
 * \brief Find all edge pieces.
 * \param i Nth-face number color (1-12)
 * \return std::vector, 5 long of index positions of found edges
 */
std::vector<int> Megaminx::findEdges(int i)
{
    assert(i > 0 && i <= numFaces);
    //Convert 1-numFaces Faces into array 0-11
    return faces[i-1].findPiecesOfFace(this->edges[0], numEdges);
}

/**
 * \brief Find all corner pieces.
 * \param i Nth-face number color (1-12)
 * \return std::vector, 5 long of index positions of found corners
 */
std::vector<int> Megaminx::findCorners(int i)
{
    assert(i > 0 && i <= numFaces);
    //Convert 1-numFaces Faces into array 0-11
    return faces[i-1].findPiecesOfFace(this->corners[0], numCorners);
}

std::vector<int> Megaminx::getAllCornerPiecesColorFlipStatus()
{
    std::vector<int> allCornerPos;
    for (int face = 0; face < 12; ++face)
        for (int r = 0; r < 5; ++r)
            allCornerPos.push_back(faces[face].corner[r]->data.flipStatus);
    return allCornerPos;
}
std::vector<int> Megaminx::getAllEdgePiecesColorFlipStatus()
{
    std::vector<int> allEdgePos;
    for (int face = 0; face < 12; ++face)
        for (int r = 0; r < 5; ++r)
            allEdgePos.push_back(faces[face].edge[r]->data.flipStatus);
    return allEdgePos;
}

int Megaminx::LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors)
{
    assert(readEdges.size() == 60);
    assert(readEdgeColors.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        std::vector<int> loadEdges = { readEdges[f + 0],readEdges[f + 1],readEdges[f + 2],readEdges[f + 3],readEdges[f + 4] };
        resetFacesEdges(face, loadEdges, false);
    }
    for (int face = 0; face < 12; ++face) {
        int f = face * 5;
        //Pieces are in the right place but maybe wrong orientation, so flip the colors:
        for (int i = 0; i < 5; ++i) {
            while (faces[face].edge[i]->data.flipStatus != readEdgeColors[f+i])
                faces[face].edge[i]->flip();
        }
    }
    return 1;
}
int Megaminx::LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors)
{
    assert(readCorners.size() == 60);
    assert(readCornerColors.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        std::vector<int> loadCorners = { readCorners[f + 0],readCorners[f + 1],readCorners[f + 2],readCorners[f + 3],readCorners[f + 4] };
        resetFacesCorners(face, loadCorners, false);
    }
    for (int face = 0; face < 12; ++face) {
        int f = face * 5;
        //Pieces are in the right place but maybe wrong orientation, so flip the colors:
        for (int i = 0; i < 5; ++i) {
            while (faces[face].corner[i]->data.flipStatus != readCornerColors[f + i])
                faces[face].corner[i]->flip();
        }
    }
    return 1;
}
void Megaminx::LoadNewEdgesFromOtherCube(Megaminx* source)
{
    for (int i = 0; i < numEdges; ++i)
        this->edges[i].data = source->edges[i].data;
}
void Megaminx::LoadNewCornersFromOtherCube(Megaminx* source)
{
    for (int i = 0; i < numCorners; ++i)
        this->corners[i].data = source->corners[i].data;
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
        std::vector<int> foundEdges = findEdges(color_n);
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
    auto foundEdges2 = findEdges(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundEdges2 == defaultEdges);
    assert(foundEdges2.size() == 5);
    for (int j = 0; j < 5; ++j) {
        //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
        while (activeFace.edge[j]->data.flipStatus != 0)
            activeFace.edge[j]->flip();
        //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
        auto &pIndex = activeFace.edge[j]->data.pieceIndex;
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
        std::vector<int> foundCorners = findCorners(color_n);
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
    auto foundCorners2 = findCorners(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundCorners2 == defaultCorners);
    assert(foundCorners2.size() == 5);
    for (int j = 0; j < 5; ++j) {
        //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
        while (activeFace.corner[j]->data.flipStatus != 0)
            activeFace.corner[j]->flip();
        //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
        auto &pIndex = activeFace.corner[j]->data.pieceIndex;
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
extern int getCurrentFaceFromAngles(int x, int y)
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
 * \param i op # from 1 - 18
 */
void Megaminx::rotateAlgo(int current_face, int i)
{
    assert(current_face > 0 && current_face <= numFaces);
    //assert(i > 0 && i <= 14);
    const colordirs &loc = g_faceNeighbors[current_face];
    switch (i) {
    case 1:
    // r u R' U' , 51 - most common one, suitable for white corners or any.
        rotateBulkAlgoString("r u R' U'");
        break;    
    case 2:
    // l u L' U' , 52 - brother to #1
        rotateBulkAlgoString("l u L' U'");
        break;
    case 3:
    // U' L' u l , 53 //(opposite is case#2)
        //https://youtu.be/PWTISbs0AAs?t=493 og video. //L#2-Edges, Insert to Left = This First, then next.
        rotateBulkAlgoString("U' L' u l");
        break;
    case 100:
    // u r U' R' , 53 (opposite is case#1) re video: //L#2-Edges, Insert to Right = This first, then previous.
        rotateBulkAlgoString("u r U' R'");
        break;
    case 4:
    // r u R' u r 2U' R' , 54
        //Last Layer Step 2:
        // Rotating the Star/Edge pieces into their correct position:
        //NOTE: Affects corners too. //only 1 corner and 2 edges remain untouched: Corner 1 and Edge 1/5
        //The 6 and 8 o'clock pieces will remain unaffected and in their same position.
        //The remaining 3 will rotate in an Anti Clockwise fashion. Repeat until all correctly positioned.
        //#2. Two Edges Solved: (Solved edge in the front & lower left) : // r u,  R' u, r U'2, R'
        rotateBulkAlgoString("r u R' u r 2U' R'");
        break;
    case 41:
    // r u2, R' u, r u2, R'
        //Last Layer Step 2: continued
        //#1. Two Edges Solved : (Solved edge in the front & upper right) :
        //#3. One Edge is Permuted : (Permuted edge in the front) this algo + then go back do the previous algo)
        rotateBulkAlgoString("r u2, R' u, r u2, R'");
        break;
    case 5:
    // u l U' R' u L' U' r , 55 orient bottom Corners // 1,2,3
        //Only affects corners. //repeat 3x = undo 
        //the 3, 5 and 7 o clock corners will rotate with each other  //moves Corners from #1to2,2to3,3to1
        //left-side (8:00 to 2:00) and diag-left-back 2corner+3edges will stay the SAME.
        rotateBulkAlgoString("u l U' R' u L' U' r");
        break;
    case 6:
    // u r 2U' L' 2u R' 2U' l u , 56
        //Only affects corners. //repeat 3x = undo 
        //Last Layer Step 3:
        // Get the corner into their correct positions. // 3,5,4
        // The front face corners (1&2) at the 5 and 7 o'clock will stay same,
        // and the three surrounding front edges(1,2,5) stay the SAME.
        // The 3 affected corners will cycle rotate around counter-clockwise.
        rotateBulkAlgoString("u r 2U' L' 2u R' 2U' l u");
        break;
    case 7:
    // R' D' R D , 57
        //Last Layer Step 4: Orientating the Corners.
        //You repeat with all corners until they are all orientated correctly.
        //repeat by executions of 4x. It will dis-align the R and D pieces temporarily.
        //NOTE: This may result in a catch-22 where the 1 last gray corner piece is color-flipped but everything else is solved. Not sure what then.
        //This is also done in the White Corners stage by repetitions of 3
        rotateBulkAlgoString("R' DR' R DR");
        break;
    case 207: //four times
        for (int i = 0; i < 4; ++i)
            rotateBulkAlgoString("R' DR' R DR");
        break;
    case 8:
    // r2 U2' R2' U' r2 U2' R2' (5 to 2, 2 to 4, 4 to 5) //8 o clock to 4 o clock, 11 o clock to 8 o clock, 4 o clock to 11 o clock.
        //Edge Permutation 1:
        //6 o'clock and 1 o'clock STAY the same. Right Star Arrow -> rotate others CCW
        //Only affects Edges & needs 5 executions; Called on Front Face, but affects top face's 3 edges //Front=Light_Blue,Changes Top=Gray
        //rotateBulkAlgoString("r2 U2' R2' U' r2 U2' R2', r2 U2' R2' U' r2 U2' R2', r2 U2' R2' U' r2 U2' R2', r2 U2' R2' U' r2 U2' R2', r2 U2' R2' U' r2 U2' R2'");
        for (int i = 0; i < 5; ++i)
            rotateBulkAlgoString("r2 U2' R2' U' r2 U2' R2'");
        //13 moves * Repeated 5 times = Total 65 moves.
        break;
    case 9:
    // r2 u2  R2' u  r2 u2  R2' (5 to 4, 4 to 2, 2 to 5) (opposite of previous; all the "up"s get reversed)
        //Edge Permutation 2:
        //6 o'clock and 1'o clock STAY the same. Right Star Arrow -> rotate others CW
        //Only affects Edges & needs 5 executions; Called on Front Face, but affects top face's 3 edges //Front=Light_Blue,Changes Top=Gray
        //rotateBulkAlgoString("r2 u2 R2' u r2 u2 R2', r2 u2 R2' u r2 u2 R2', r2 u2 R2' u r2 u2 R2', r2 u2 R2' u r2 u2 R2', r2 u2 R2' u r2 u2 R2'");
        for (int i = 0; i < 5; ++i)
            rotateBulkAlgoString("r2 u2 R2' u r2 u2 R2'");
        //13 moves * Repeated 5 times = Total 65 moves.
        break;
    case 10:
    // r u R' F', r  u  R' U', R' f r2 U' R' (5 to 1, 1 to 2, 2 to 5)
        //Edge Permutation 3:
        // Front face is untouched. (front 2 corners, front/left 2 edges) Rotates chunks of 2 clockwise
        //NOTE: CORNERS ARE AFFECTED by this too.  // 3 repeats = undo
        rotateBulkAlgoString("r u R' F', r  u  R' U', R' f r2 U' R'");
        break;
    case 11:
    // r u R' u,  R' U' r2 U',  R' u R' u,  r U2' (5 to 2, 2 to 1, 1 to 5)
        //Edge Permutation 4a:
        //11 o'clock to 4 o'clock, 4 o'clock to 1 o'clock, 1 o'clock to 11 o'clock
        //Only affects Edges, only needs one run.
        // 2 unaffecteds stay on front/left sides
        //opposite of the previous one above , but corners aren't affected...
        rotateBulkAlgoString("r u R' u,  R' U' r2 U',  R' u R' u,  r U2'");
        break;
    case 201:
        //Edge Permutation 4b:
        //Opposite of EdgePermutation4a. Reverses #4A only when 3 edges are positioned in the front row,
        // 2 unaffecteds stay on both/back sides. Cycles edges in the opposite rotation.
        //manually reverse engineered from 4, to be equal to #3 but without affecting corners.
        //Reverses 4a if cube is rotated 2 turns CW.
        rotateBulkAlgoString("r' u' r u', r u r2' u, r u' r u', r' u2");
        break;
    case 202:
        //Edge Permutation 4c:
        //2 unaffecteds = right/back side untouched. edges cycle rotate = clockwise
        //Reverses 4a if cube is rotated 2 turns CCW.
        //Identical Twin to 4b but Rotates the front face instead of right
        rotateBulkAlgoString("f' u' f u', f u f2' u, f u' f u', f' u2");
        break;
    case 12:
        //Edge Permutation 5:
        // l r u2,  L' u R',  l U' r u2,  L' u2 R' (5 and 3 swap, 1 and 2 swap) //1 and 5, 4 and 2
        //NOTE: CORNERS ARE AFFECTED by this edge algo,   //1 Repeat = Undo
        rotateBulkAlgoString("l r u2, L' u R', l U' r u2, L' u2 R'");
        break;
    case 18:
        // #2nd-Layer Edges/Star(LEFT) =  7 o'clock to 9 o'clock:
        //reverse engineered from #17 myself (Exact opposite), manual is lacking.
        rotateBulkAlgoString("dl l dl l' dl' f' dl' f");
    case 17:
        // #2nd-Layer Edges/Star(RIGHT) =  5 o'clock to 3 o'clock:
        //Algo from QJ cube manual & (White face on top)
        rotateBulkAlgoString("dr' r' dr' r dr f dr f'");
        break;
    case 14:
        // F' R', F' F', r f    // 12 o'clock to 7 o'clock
        // #4th-Layer Edges(LEFT), (between the middle W), fourthLayerEdgesA()
        //Cube must have gray side on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
        rotateBulkAlgoString("F' R', F' F', r f");
        break;
    case 13:        
        // f l, f f, L' F'      //12 o'clock to 5 o'clock.        
        // #4th-Layer Edges(RIGHT), (between the middle W), fourthLayerEdgesB()
        rotateBulkAlgoString("f l, f f, L' F'");
        break;
    case 15:
        // #6th-Layer Edges(LEFT) = U' L' u l u f U' F'  //Must have Layers 1-5 solved, and 7th layer is affected.
        // swap edge from face's star at 12 o'clock to Flop in (pinned to center) To the edge @  9 o'clock
        //https://www.youtube.com/watch?v=j4x61L5Onzk
        rotateBulkAlgoString("U' L' u l u f U' F'");
        break;
    case 16:
        // #6th-Layer Edges(RIGHT) = u r U' R' U' F' u f //opposite of previous, To the edge @ 3 o'clock
        rotateBulkAlgoString("u r U' R' U' F' u f");
        break;
    case 203:
        // #7Last-Layer 5-way-star, Opposites Clockwise, 1 to 4, 4 to 2, 2 to 5, 5 to 3, 3 to 1 (aka 1,3,5,2,4)
        //#last layer Star, copied from cube manual (turned upside down)
        //rotateBulkAlgoString("l' u2 r u2' l u2 r', l' u2 r u2' l u2 r', l' u2 r u2' l u2 r', l' u2 r u2' l u2 r', l' u2 r u2' l u2 r', l' u2 r u2' l u2 r'");
        for (int i = 0; i < 6; ++i)
            rotateBulkAlgoString("l' u2 r u2' l u2 r'");
        break;
    case 204:
        // #7Last-Layer 5-way star, copied from cube manual, Two halves //2,1,3,4,5
        // 60 moves. Gray Face must be on top. Non invasive overall but very invasive temporarily
        //rotateBulkAlgoString("r' l u' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u2' r l' u2, r' l u' r l'");
        rotateBulkAlgoString("r' l u' r l' u2");
        for (int i = 0; i < 6; ++i)
            rotateBulkAlgoString("r' l u2' r l' u2");
        rotateBulkAlgoString("r' l u' r l'");
        break;
    case 205:
        // #7Last-Layer Edge swaps - have 1 edge solved remains in front, then swap 2&3 and 4&5
        //copied from manual. 44 moves total. Gray Face must be on top. Non invasive overall but very invasive temporarily
        //rotateBulkAlgoString("u2' l2' u2' l2 u' l2' u2' l2 u', l2' u2' l2 u' l2' u2' l2 u', l2' u2' l2 u' l2' u2' l2 u");
        rotateBulkAlgoString("u2' l2' u2' l2 u' l2' u2' l2 u'");
        rotateBulkAlgoString("    l2' u2' l2 u' l2' u2' l2 u'");
        rotateBulkAlgoString("    l2' u2' l2 u' l2' u2' l2 u ");
        break;
    case 206:
        //#7Last-Layer-Edge-Permu Copied from manual, 30 moves total... have 1 edge solved, then swap 2&4 and 3&5/INVERTED.
        //Solved piece remains in front, right/backRight swap and left/backLeft swap
        // with INVERTS @ 8 o'clock and 1 o'clock. Gray Face must be on top.
        //rotateBulkAlgoString("r' l f2' r l' u2 r' l, f' r l' u2' r' l f2' r, l' u2 r' l f' r l' u2'");
        rotateBulkAlgoString("r' l f2' r l' u2 r' l");
        rotateBulkAlgoString("f' r l' u2' r' l f2' r");
        rotateBulkAlgoString("l' u2 r' l f' r l' u2'");
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
    while (ss) {                           // while the stream is good
        std::string word;                  // parse first word
        numdir op = { -1, Face::Clockwise };
        if (ss >> word) {
            if (word.find("'") != std::string::npos)
                op.dir *= -1;
            if ((word.find("dr") != std::string::npos) ||
                (word.find("DR") != std::string::npos))
                op.num = loc.downr - 1;
            else if ((word.find("dl") != std::string::npos) ||
                     (word.find("DL") != std::string::npos))
                op.num = loc.downl - 1;
            else if ((word.find("r") != std::string::npos) ||
                     (word.find("R") != std::string::npos))
                op.num = loc.right - 1;
            else if ((word.find("f") != std::string::npos) ||
                     (word.find("F") != std::string::npos))
                op.num = loc.front - 1;
            else if ((word.find("l") != std::string::npos) ||
                     (word.find("L") != std::string::npos))
                op.num = loc.left - 1;
            else if ((word.find("u") != std::string::npos) ||
                     (word.find("U") != std::string::npos))
                op.num = loc.up - 1;
            if (op.num > -1)
                readVector.push_back(op);
            if (word.find("2") != std::string::npos) {
                assert(readVector.size() > 0);
                readVector.push_back(readVector.back());
            }
        }
    }
    return readVector;
}

int Megaminx::findEdge(int pieceNum)
{
    for (int i = 0; i < numEdges; ++i)
        if (edges[i].data.pieceIndex == pieceNum)
            return i;
    return -1;
}
std::vector<int> Megaminx::findEdgePieces(const int pieceNums[5])
{
    std::vector<int> pieceList;
    for (int i = 0; i < 5; i++)
        pieceList.push_back(findEdge(pieceNums[i]));
    return pieceList;
}
std::vector<int> Megaminx::findEdgePieces(std::vector<int> &v)
{
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    return findEdgePieces(vecPieceNums);
}

void Megaminx::resetFiveEdges(const int indexes[5]) {
    for (int i = 0; i < 5; ++i) {
        std::vector<int> whereAreTheyNow = findEdgePieces(indexes);
        if (edges[indexes[i]].data.pieceIndex != indexes[i]) {
            edges[indexes[i]].swapdata(edges[whereAreTheyNow[i]].data);
            i = -1;
        }
    }
    //Pieces are in the right place but maybe wrong orientation, so flip the colors:
    for (int i = 0; i < 5; ++i) {
        while (edges[indexes[i]].data.flipStatus != 0)
            edges[indexes[i]].flip();
    }
}
void Megaminx::resetFiveEdges(std::vector<int> &v) {
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    resetFiveEdges(vecPieceNums);
}

int Megaminx::findCorner(int pieceNum)
{
    for (int i = 0; i < numCorners; ++i)
        if (corners[i].data.pieceIndex == pieceNum)
            return i;
    return -1;
}
std::vector<int> Megaminx::findCornerPieces(const int pieceNums[5])
{
    std::vector<int> pieceList;
    for (int i = 0; i < 5; i++)
        pieceList.push_back(findCorner(pieceNums[i]));
    return pieceList;
}
std::vector<int> Megaminx::findCornerPieces(std::vector<int> &v)
{
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    return findCornerPieces(vecPieceNums);
}

void Megaminx::resetFiveCorners(const int indexes[5]) {
    for (int i = 0; i < 5; ++i) {
        std::vector<int> whereAreTheyNow = findCornerPieces(indexes);
        if (corners[indexes[i]].data.pieceIndex != indexes[i]) {
            corners[indexes[i]].swapdata(corners[whereAreTheyNow[i]].data);
            i = -1;
        }
    }
    //Pieces are in the right place but maybe wrong orientation, so flip the colors:
    for (int i = 0; i < 5; ++i) {
        while (corners[indexes[i]].data.flipStatus != 0)
            corners[indexes[i]].flip();
    }
}
void Megaminx::resetFiveCorners(std::vector<int> &v) {
    assert(v.size() == 5);
    const int vecPieceNums[5] = { v[0], v[1], v[2], v[3], v[4] };
    resetFiveCorners(vecPieceNums);
}

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

//Start of AI:
//Layer 1: White Face:  White Edges, White Corners.
//Do the same thing as below, but with the Edges first, intuitively moving them in.
//We can do either, but presumably it will be harder to jump the edges over the corners afterwards.
//one by one, find all white corners. Move any in the white face OUT and move ALL of them to the middle "W" layer 3/5,
// preferably right above its face and position. so you can insert it down and in by RDR'D' move or whatever.
//Has to be color flipped. Can be avoided if placed on the W the right way.
//Position it in the right spot:
//BUILD A PIECE SWAPPER that acts like .swapdata(edge->data) but goes through the rotations. It needs to know obviously,
// source and dest Piece, which can be given and assigned by Piece Index #. This system has only recently been used for the algorithms above, highYmiddleW(),
// for example: the concept of assigning semantic names to these arbitrary human combinations of pieces. (These can be named constexpr later)
// To avoid all the conflicts we had with Swap, we need to check first and make sure any source pieces are not in any dest slots, if there are, move them OUT (to anywhere above/below the solved layer boundary).
//Good idea: Think about Linear pathway lines that the blocks travel around on. These are made from a known number of fixed points, the corner's outer tips. These represent train stations aka stops along a train track.
//Once the piece has been confirmed as on the track (hoisted up or intersected with it from another route) - we can compute how many stops it needs to travel to its destination.
//From there we can use a TTL routing protocol to forward the block as if it were a packet across the pathway line.
//That way we can make a decision on whether to take a color flip detour before we get to the end or not.
//Once the pathways are defined, the pieces can be scored and ranked on how far away they are from their destination. Farther ones should be more aggressively moved toward their destination as an optimization.
// ://www.jaapsch.net/puzzles/megaminx.htm for a more list of notations.
//Layer 2: Edges,One at a time, If any of the 5 pieces that are supposed to be in this #2 edge slot, move them OUT, and to the W layer.
//Position it in the right spot, the source location decides the destination drop-in:
//Execute the known, human algo Layer 2 Edge Star Left/Right if its Left or right.
//Find desired edge piece, surf it around to the gray layer, then back down to the top of the star either to the left or the right of dropping it into place.
//Drop in procedure: Move star-top away from the drop-in location, then spin the R/L side UP (the side thats opposite of the star-top turn-away direction) (up is either CW or CCW depending on the side) and then rotate both back
// this will group the correct edge to the correct the corner, above the drop-in location. A second similar drop-in move is needed, likely "u r U' R'" or "u l U' L'"
//3rd layer Corners = Low Y's involve flipping the puzzle upside down, white face on top, and positioning the desired piece on the bottom layer, then swiveling the bottom face around to orient it,
// and then rotating it up and into the Low Y. since the entire rest of the puzzle is unsolved, this can be done intuitively, just rotate the piece on the bottom until its colored correctly, then drop it in.
//Layer 3: Corners Like layer 1 and we use the gray layer as the temp layer
//Layer 4: Edges, since We know the all the Star/edges algo for 2/4/6. Left/Right needs to be decided on the destination this time,
// the source is always at 12 o clock. drops into either 5 / 7 o'clock (Right/Left)
//Pieces already in the layer 4 area but wrong in location need to be moved up and out onto the Gray face as temporary, using Algos.
//Any pieces on the gray face need to be Color-flipped favorably before the drop-in. To change the color, we need to TODO:??????????????????
//
//We can also solve some Layer 6 edges now without hurting anything, which helps because some of our source pieces may be there.
//Layer 5: Corners, again. This time they source from Gray layer 7 as the drop-in point. We should find one that exists up on that top layer that can be dropped in.
// otherwise we will have to hoist the pieces up from below. We will have to do this anyway. Theoretically you can pick any piece to start, hoist it up, then drop it in. Then repeat until you've finished.
// But there should be some already up there, and if we Look for them as they come around, and solve those next, more will be solved this way.
//When they are ready to drop in, the colors can be one of 3 ways, and this can be accomplished by either 1, 3 or 5 R/D moves
//Layer 6: Edges source from Gray layer 7 and drop in a short 1/5th sideways. same idea applies from Layer#4
// the source is always at 12 o clock. drops into either 3 / 9 o'clock (Right/Left)
//Layer 7: from what I can tell, do Corners first, because some of the corner algos affect the edges also, and some of the edge algos affect the corners, so those can be used too...
// So do corners with the intention of corrupting the edges but making ultimate progress on getting the edges all facing downwards at least.
//Coloring the corners involves multiple moves of R/D/R'/D' until they color correctly. This corrupts the lines you use for the transformation, "Z",
// Repeats and reverts in cycles of 6. So after one corner is solved, the temporary lines may still be corrupted, if they are,
// you must rotate the Top/Gray again and continue to color-flip additional Top corners to revert the corrupted "Z" line before you move on.
// This may approach a catch 22, where everything is solved but only one gray corner is 1 flip away, if this is the case, I don't know what to do...
//Theres a lot of last-layer algorithms to choose from. Choose wisely!
//
//TODO: Uncovered a bug in this algo where it swaps 90% of the elements Out and then is left with a parity, for example:
//A = { 22, 11, 10, 22, 3, 26, 28, 26,  }
//B = { 26, 3, 28, 26, 11, 22, 10, 22, }
//It thinks swapping A[0] with B[0] and so on, amounts to a net change of 0. And its right. But why am i wrong?
//This also occurs down to 3 pieces: such like: A = { 1, 2, 4 } and B = { 2, 4, 1 }, or even just two: {0,4}/{4,0} 
//
//get piece from A to B by rotating it:
//white edges. intuitive. edges 0-4 will flop in from 5-9, watch polarity.
//Possibility = Alternate 1Edge + 1Corner for whiteface.
//for corners find out what floor 1-4 theyre on
//for first step, whole cube is unsolved we can rotate without bad consequences, but later we will need to know how to stick to the bottom half and gray layer for scratch temp space, without affecting anything. or at least switch to non-destructive forwarding
//for next step we need to move-out with a human-algo
//white corners 0-4 will drop in from 5-9
//to affect a piece, we query the &Pieces for which 2 or 3 faces its claimed by. Add a stat to track this.
//Q:) How to figure out which 3 faces and dir to turn them? A:) we can fake try all 6 choices and see which gets closer.
//Level 3 corners can be flipped color easily, follow this, they use the bottom side layers and level 5 corners to temp-hold them in 3 moves - 5 moves.
//Pathways will be defined as the 5 Z-lines going from white to gray corner Line covers 3 edges. 5 lines * 3 = 15. Plus the top/bottom lines = 5 * 2 = 10, + 15, = 25 the 5 extra edges to make 30 happen in the 10-wide 4th layer and need not be worried about.

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
        if (offby == 4) {
            offby = 1;
            defaultDir *= -1;
        }
        else if (offby == 3) {
            offby = 2;
            defaultDir *= -1;
        }
        for (int j = 0; j < offby; ++j)
            shadowDom->shadowRotate(face, defaultDir);
        //do not reset the loop, keep executing,
    }
    return (offby > 0);
}
void Megaminx::DetectSolvedWhiteEdgesUnOrdered(bool piecesSolved[5])
{
    std::vector<int> piecesSeenOnTop;
    std::vector<int> piecesOrderedOnTop; //why do we have this?
    //populate piecesSolved
    int numSolved = 0;
    //Find out if any applicable 0-4 pieces are in the 0-4 slots:
    for (int p = 0; p < 5; ++p) {
        int pIndex = findEdge(p);
        //make sure its 0-4 and make sure the colors arent flipped
        if (pIndex >= 0 && pIndex <= 4 && edges[pIndex].data.flipStatus == 0)
            piecesSeenOnTop.push_back(p);
        //If we found the correct piece solved in the correct spot
        if (p == pIndex && edges[pIndex].data.flipStatus == 0) {
            piecesSolved[p] = true;
            numSolved++;
        }
    }
    if (piecesSeenOnTop.size() > 1) {
        //Check if the ordering blue->red,red->green is correct,etc... even if the top is twisted vs the sides
        for (int p = 0; p < 5; ++p) {
            int pNext = (p != 4) ? p + 1 : 0;
            int pIndex = findEdge(p);
            int pNextIndex = findEdge(pNext);
            if (((pIndex >= 0 && pIndex <= 4) && (pNextIndex >= 0 && pNextIndex <= 4)) &&
                edges[pNextIndex].data.flipStatus == 0 && edges[pIndex].data.flipStatus == 0 &&
                ((pNextIndex == pIndex + 1) || (pIndex == 4 && pNextIndex == 0))) {
                piecesOrderedOnTop.push_back(p);
                piecesSolved[p] = true;
                piecesSolved[pNext] = true;
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
void Megaminx::DetectSolvedWhiteCorners(bool piecesSolved[5])
{
    //Find out if any applicable 0-4 corner pieces are exactly in their slots.
    for (int p = 0; p < 5; ++p) {
        int pIndex = findCorner(p);
        //make sure its 0-4 and make sure the colors arent flipped
        if (pIndex >= 0 && pIndex <= 4 && p == pIndex && corners[pIndex].data.flipStatus == 0)
            piecesSolved[p] = true;
    }
}
void Megaminx::DetectSolved2ndLayerEdges(bool piecesSolved[5])
{
    //Find out if any applicable 5-9 edge pieces are exactly in their slots.
    for (int p = 5; p < 10; ++p) {
        int pIndex = findEdge(p);
        //make sure its 5-9 and make sure the colors arent flipped
        if (pIndex >= 5 && pIndex <= 9 && p == pIndex && edges[pIndex].data.flipStatus == 0)
            piecesSolved[p-5] = true;
    }
}
void Megaminx::DetectSolved3rdLayerCorners(bool piecesSolved[5])
{
    //Find out if any applicable 5-9 corner pieces are exactly in their slots.
    for (int p = 5; p < 10; ++p) {
        int pIndex = findCorner(p);
        //make sure its 5-9 and make sure the colors arent flipped
        if (pIndex >= 5 && pIndex <= 9 && p == pIndex && corners[pIndex].data.flipStatus == 0)
            piecesSolved[p-5] = true;
    }
}
void Megaminx::DetectSolved4thLayerEdges(bool piecesSolved[5])
{
    //Find out if any applicable 10-20 edge pieces are exactly in their slots.
    for (int p = 10; p < 20; ++p) {
        int pIndex = findEdge(p);
        //make sure its 10-20 and make sure the colors arent flipped
        if (pIndex >= 10 && pIndex <= 19 && p == pIndex && edges[pIndex].data.flipStatus == 0)
            piecesSolved[p - 10] = true;
    }
}
void Megaminx::DetectSolved5thLayerCorners(bool piecesSolved[5])
{
    //Find out if any applicable 10-15 corner pieces are exactly in their slots.
    for (int p = 10; p < 15; ++p) {
        int pIndex = findCorner(p);
        //make sure its 10-15 and make sure the colors arent flipped
        if (pIndex >= 10 && pIndex <= 14 && p == pIndex && corners[pIndex].data.flipStatus == 0)
            piecesSolved[p - 10] = true;
    }
}
void Megaminx::DetectSolved6thLayerEdges(bool piecesSolved[5])
{
    //Find out if any applicable 20-25 edge pieces are exactly in their slots.
    for (int p = 20; p < 25; ++p) {
        int pIndex = findEdge(p);
        //make sure its 20-25 and make sure the colors arent flipped
        if (pIndex >= 20 && pIndex <= 24 && p == pIndex && edges[pIndex].data.flipStatus == 0)
            piecesSolved[p - 20] = true;
    }
}
//Layer 1 part 1
void Megaminx::rotateSolveWhiteEdges(Megaminx* shadowDom)
{   //FromCubeToShadowCube
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    int i = 0;
    //Loop management:
    int loopcount = 0;
    do {        
        //temporary overflow protection:
        loopcount++;
        if (loopcount > 101)
            break;
        bool facesSolved[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
        bool piecesSolved[5] = { false, false, false, false, false };
        int firstSolvedPiece = -1;
        shadowDom->DetectSolvedWhiteEdgesUnOrdered(piecesSolved);
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
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4watc
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);            
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        //auto defaultEdgeFaceLocsA = shadowDom->faces[edgeFaceNeighbors.a - 1].defaultEdges;
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //auto defaultEdgeFaceLocsB = shadowDom->faces[edgeFaceNeighbors.b - 1].defaultEdges;
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        Edge* EdgeItselfB = shadowDom->faces[edgeFaceNeighbors.b - 1].edge[edgeFaceLocB];
        assert(EdgeItselfA == EdgeItselfB); //sanity check.
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        assert(whichcolorEdgeA + whichcolorEdgeB == 1); //just makes sure the reference table is accurate, it is.
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        int edgeHalfColorB = EdgeItselfB->data._colorNum[whichcolorEdgeB];
        bool colormatchA = edgeHalfColorA != WHITE;
        bool colormatchB = edgeHalfColorB != WHITE;
        assert(colormatchA != colormatchB); //sanity check.
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        bool isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        bool isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        bool isOnRow4 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20);
        bool isOnRow44 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
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
        //New breakthrough idea, any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        bool ontopA = (edgeFaceNeighbors.a > 1 && edgeFaceNeighbors.a < 7);
        bool ontopB = (edgeFaceNeighbors.b > 1 && edgeFaceNeighbors.b < 7);
        if (ontopA && ((isOnRow4 && colormatchA) || isOnRow2)) {
            int offby = colormatchA ? (edgeFaceNeighbors.a - edgeHalfColorA) : (edgeFaceNeighbors.b - edgeHalfColorB);
            //Set up Rotated White top to be in-line with the face we want to spin in.
            shadowMultiRotate(WHITE, offby, shadowDom);
            if (isOnRow4 && colormatchA) {
                shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
                shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
            }
            else if (isOnRow2 && colormatchA) {
                shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
            }
            else if (isOnRow2 && colormatchB) {
                shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
            }
        }
        //Locates any straggler pieces on the bottom and bubbles them up to the top layers, as long as the face isnt protected by facesSolved pieces
        else if (isOnRow4 && dirToWhiteA != 0 && ((edgeFaceNeighbors.a < GRAY && !facesSolved[edgeFaceNeighbors.a - 1]) || edgeFaceNeighbors.a >= GRAY)) {
            shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
        }
        else if (isOnRow4 && dirToWhiteB != 0 && ((edgeFaceNeighbors.b < GRAY && !facesSolved[edgeFaceNeighbors.b - 1]) || edgeFaceNeighbors.b >= GRAY)) {
            shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
            //REFRESH
            int sourceEdgeIndexNext = shadowDom->findEdge(i);
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == edgeFaceNeighbors.b)
                //check the solved for the next neighbor (vs what if we just check the original, face.loc.right) ?
                if (facesSolved[edgeFaceNeighborsNext.a - 1])
                    //make sure its the move going to the right
                    if (edgeFaceNeighborsNext.a == g_faceNeighbors[edgeFaceNeighbors.a].right)
                        shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
        }
        else if (isOnRow6 || isOnRow7 && dirToWhiteA != 0) {
            shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
        }
        else if (isOnRow6 || isOnRow7 && dirToWhiteB != 0) {
            shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
        }
        //These can still trigger if the top pieces are in the top row but the wrong slot (or are color-flipped)
        else if (isOnRow1 && dirToWhiteA != 0 && !facesSolved[edgeFaceNeighbors.a - 1]) {
            shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
        }
        else if (isOnRow1 && dirToWhiteB != 0 && !facesSolved[edgeFaceNeighbors.b - 1]) {
            shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
        }

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
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}

//Layer 1 part 2
void Megaminx::rotateSolveWhiteCorners(Megaminx* shadowDom)
{   //White Face on Top:
    if (!shadowDom) {
        shadowDom = new Megaminx();
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    bool allSolved = false;
    int i = 0;
    //Loop management:
    int loopcount = 0;
    do {
        //temporary overflow protection:
        loopcount++;
        if (loopcount > 101)
            break;
        bool facesSolved[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
        bool piecesSolved[5] = { false, false, false, false, false };
        int firstSolvedPiece = -1;
        shadowDom->DetectSolvedWhiteCorners(piecesSolved);
        for (int a = 0; a < 5; ++a) {
            facesSolved[1 + a] = piecesSolved[a];
            if (firstSolvedPiece == -1 && piecesSolved[a] == true)
                firstSolvedPiece = a;
        }
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
        //    //RE-NOTE: Doing this over and over is wasting moves solving the partial-solved top every time.
        //    //RE-TODO: This means we need a plan for any 5-9 corner to get moved into any 0-4 slot even when top isn't solved.
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
                for (auto op : bulk)    //returns raw faces
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
                std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("R' DR' R DR", loc);
                for (auto op : bulk)    //returns raw faces
                    shadowDom->shadowRotate(op.num+1, op.dir);
            }
        }
        //Cycles the pieces along the W, Right Part 1/2 Hi/Low (ends up on row3)
        else if (isOnRow2) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 5;
            if (ontopA && ontopB)
                shadowDom->shadowRotate(cornerFaceNeighbors.c, defaultDir);
            else if (ontopA && ontopC)
                shadowDom->shadowRotate(cornerFaceNeighbors.b, defaultDir);
            else if (ontopB && ontopC)
                shadowDom->shadowRotate(cornerFaceNeighbors.a, defaultDir);
        }
        //TODO: they go the long way around no matter what. make them take shortest path.
        //Note, 10 max moves on long path. short path can be using bottom face, max 2 moves per side, 3 sides = shortcut path = 6 moves. 10 > 6
        //Cycles the pieces along the W, Right Part 2/2 Low/Hi (ends up on row2)
        else if (isOnRow3) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 10;
            megaminxColor maxABC;
            if (!ontopA)
                maxABC = max(cornerFaceNeighbors.b, cornerFaceNeighbors.c);
            else if (!ontopB)
                maxABC = max(cornerFaceNeighbors.a, cornerFaceNeighbors.c);
            else if (!ontopC)
                maxABC = max(cornerFaceNeighbors.a, cornerFaceNeighbors.b);
            shadowDom->shadowRotate(maxABC, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2
        else if (isOnRow4) {
            int offby = sourceCornerIndex - i - 15;
            //BUG: Find out if this offset is correct 
            for (int j = 0; j < offby; ++j)
                shadowDom->shadowRotate(GRAY, Face::CW);
            megaminxColor maxABC;
            if (cornerFaceNeighbors.a == GRAY)
                maxABC = max(cornerFaceNeighbors.b, cornerFaceNeighbors.c);
            else if (cornerFaceNeighbors.b == GRAY)
                maxABC = max(cornerFaceNeighbors.a, cornerFaceNeighbors.c);
            else if (cornerFaceNeighbors.c == GRAY)
                maxABC = max(cornerFaceNeighbors.a, cornerFaceNeighbors.b);
            //Find out if this is the most useful thing we can do from the bottom row.
            shadowDom->shadowRotate(maxABC, Face::CW);
            shadowDom->shadowRotate(maxABC, Face::CW);
        }
    } while (!allSolved);
    //After both loops, load the shadow Queue into the real queue    
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}

//Layer 2 - Edges
void Megaminx::rotateSolveLayer2Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube
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
        assert(unknownloop == 0);
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolved2ndLayerEdges(piecesSolved);
        int i = 5; //the piece
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
        //Rotates the white face to its solved position, first solved EDGE matches up to its face.
        //Edges should already be solved, if not, get top white face spun oriented back to normal
        int edgesOffBy = shadowDom->findEdge(0); //always piece 0
        if (edgesOffBy > 0 && edgesOffBy < 5) {
            edgesOffBy *= -1;
            shadowMultiRotate(WHITE, edgesOffBy, shadowDom);
        }
        else if (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus == 0) {
            piecesSolved[i - 5] = true;
            continue;
        }
        else if ((isOnRow2 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
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
            int offby = 0;
            int row7 = sourceEdgeIndex - 28 - i;
            if (edgeFaceNeighbors.a == GRAY)
                offby = RED + edgeFaceLocA + 1 + i;
            else if (edgeFaceNeighbors.b == GRAY)
                offby = RED + edgeFaceLocB + 1 + i;
            //These should be equivalent but for example -12 and -7 both reduce to -2
            //TODO: make function to reduce the number itself
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
        else //this happens in limbo. prevents full auto-solve, needs re-keypress
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    
    //After all loops, load the shadow Queue into the real queue    
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}

//Layer 3 - Corners
void Megaminx::rotateSolve3rdLayerCorners(Megaminx* shadowDom)
{   //White Face on Top:
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
        assert(unknownloop == 0);
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        bool facesLocked[13] = { false, false, false, false, false, false, false, false, false, false, false, false, false };
        shadowDom->DetectSolved3rdLayerCorners(piecesSolved);
        for (int a = 0; a < 5; ++a) {
            int x = PINK + a;
            if (x > BEIGE)
                x -= 5;
            facesLocked[x] = piecesSolved[a];
        }
        int i = 5; //the piece
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
        //Solved case
        else if (sourceCornerIndex == i && CornerItselfA->data.flipStatus == 0) {
            piecesSolved[i - 5] = true;
            continue;
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
            //If the piece is solved positioned but color flipped wrong:
            // turn opposite direction row2->row3->row4 to use gray face then repeat till solved.
            if (sourceCornerIndex == i) {
                shadowDom->shadowRotate(turnface, -1 * defaultDir);
                shadowDom->shadowRotate(turnface, -1 * defaultDir);
            }
            else {
                shadowDom->shadowRotate(turnface, defaultDir);
                shadowDom->shadowRotate(turnface, defaultDir);
            }
        }
        //Row 3 pieces go to gray face as temporary holding (1 CCW turn) (ends up on row4)
        else if (isOnRow3) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - i - 10;
            megaminxColor maxABC;
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
            maxABC = (megaminxColor)result;
            shadowDom->shadowRotate(maxABC, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2 Solved
        else if (isOnRow4) {
            int offby = sourceCornerIndex - (i - 5) - 18;
            shadowMultiRotate(GRAY, offby, shadowDom);
            int x = PINK + (i - 5);
            if (x > BEIGE)
                x -= 5;
            int defaultDir = Face::CCW;
            shadowDom->shadowRotate(x, defaultDir);
            shadowDom->shadowRotate(x, defaultDir);
        }
        else //if this happens we are undefined behavior.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After both loops, load the shadow Queue into the real queue    
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}

//Layer 4 - Edges
//Cube must have gray side on top, layer 1+2+3 Solved (white face+2nd layer edges+LowY's), and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer4Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube
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
        shadowDom->DetectSolved4thLayerEdges(piecesSolved);
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
        //Solved case
        if (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus == 0) {
            piecesSolved[i - 10] = true;
            continue;
        }
        //Get ready for algorithms
        else if ((isOnRow34 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow7)) {
            if (isOnRow7) {
                //Align GRAY top to the exact position for pre-drop-in.
                int offby = 0;
                if (isRight)
                    offby = sourceEdgeIndex - 27 - i;
                else if (isLeft) //row 4B's B-half is +1 from row4A's B-half (DARK_BLUE,LIGHT_GREEN) vs (DARK_BLUE,PINK)
                    offby = sourceEdgeIndex - 28 - i;
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
        assert(unknownloop == 0);
        loopcount++;
    } while (!allSolved);

    //After all loops, load the shadow Queue into the real queue    
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}

//Layer 5 - Corners
void Megaminx::rotateSolve5thLayerCorners(Megaminx* shadowDom)
{   //White Face on Top:
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
        assert(unknownloop == 0);
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolved5thLayerCorners(piecesSolved);
        int i = 10; //the piece
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
        //Solved case
        if (sourceCornerIndex == i && CornerItselfA->data.flipStatus == 0) {
            piecesSolved[i - 10] = true;
            continue;
        }
        //Get the Pieces to drop-in ready on Row 4 (gray layer) solved into Row3
        //Any Row 3 pieces that are mis-solved use same algo to go up to gray layer (ends up on row4)
        else if (isOnRow3) {
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
        else //if this happens we are undefined behavior.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After both loops, load the shadow Queue into the real queue    
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}

//Layer 6 - Edges
//Cube must have gray side on top, layer 1+2+3+4+5 Solved, and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer6Edges(Megaminx* shadowDom)
{   //FromCubeToShadowCube
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
        shadowDom->DetectSolved6thLayerEdges(piecesSolved);
        int i = 20; //the piece
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
        //Solved case
        if (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus == 0) {
            piecesSolved[i - 20] = true;
            continue;
        }
        //Get ready for algorithms
        else if ((isOnRow6 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
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
        assert(unknownloop == 0);
        loopcount++;
    } while (!allSolved);

    //After all loops, load the shadow Queue into the real queue    
    if (shadowDom->shadowRotateQueue.size() > 0) {
        if (rotateQueue.size() == 0)
            rotateQueue.swap(shadowDom->shadowRotateQueue);
        else {
            size_t numsize = shadowDom->shadowRotateQueue.size();
            for (int q = 0; q < numsize; ++q) {
                rotateQueue.push(shadowDom->shadowRotateQueue.front());
                shadowDom->shadowRotateQueue.pop();
            }
        }
    }
}
