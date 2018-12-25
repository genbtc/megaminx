/* MegaMinx2 v1.32 - 2017+2018 - genBTC mod
 * Uses code from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
 * genBTC November 2017 - genbtc@gmx.com / @genr8_ / github.com/genbtc/
 * genBTC December 2018 - fixups, tweaks.
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
//FIXED: Noticed that _rotatingFaceIndex as set to 0 on startup, means that is .rendered()
//     : make startup not call this. Solution A), set to -1 on startup and check everytime.
    if (_rotatingFaceIndex == -1) return;
    //Or Finish the rotation Queue
    const bool isRotaFullyRendered = faces[_rotatingFaceIndex].render();
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
    assert(num > 0);
    assert(num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    num -= 1; //Convert 1-12 Faces into array [0-11]
    _rotate_internal({ num, dir });
}
//The most important rotate function - no validation. (internal,private)
void Megaminx::_rotate_internal(numdir i)
{
    rotateQueue.push({ i.num, i.dir });
    undoQueue.push({ i.num, i.dir });
    //TODO: use undoQueue as written log.
}
void Megaminx::_rotate_internal(int num, int dir)
{
    _rotate_internal({ num, dir });
}

//An unlimited undo queue based off std::queue.
void Megaminx::undo()
{
    if (undoQueue.empty()) return;
    auto op = undoQueue.front();
    op.dir *= -1;
    rotateQueue.push({ op.num, op.dir });
    undoQueue.pop();
}

//Clear the Queue and stop any repeated rotating actions.
void Megaminx::resetQueue()
{
    isRotating = false;
    rotateQueue = std::queue<numdir>();
    //TODO: at this point, the other Undo-Queue now has extra actions in it.
    //    : find how many we skipped in the "cancel" and pop those off the undo queue too.
}

//Scramble 600 times (50 x 12)
//FIXED: apparently scrambles are even more random if you rotate by 2/5ths instead of 1/5th.
void Megaminx::scramble()
{
    //Do 50 iterations of scrambling (like a human)
    for (int q = 0; q < 50; q++) {
        //numFaces faces - turn one each, randomizing direction
        for (int i = 0; i < numFaces; i++) {
            const int r = rand() % 2 * 2 - 1;
            faces[i].placeParts(r);
            faces[i].placeParts(r); //FIXED: do a second rotation.
        }
    }
}

/**
 * \brief Toggle the colors of a single Corner piece
 * * If called externally make sure its color 1-12.
 * \param i Nth-face's number (color) [0-11]
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::swapOneCorner(int i, int x)
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
void Megaminx::swapOneEdge(int i, int x)
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
    return faces[i-1].findPiece(edges[0], numEdges);
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
    return faces[i-1].findPiece(corners[0], numCorners);
}

std::vector<int> Megaminx::returnNativeCornerColorPos()
{
    std::vector<int> allCornerPos;
    for (int face = 0; face < 12; ++face) {
        for (int r = 0; r < 5; ++r) {
            allCornerPos.push_back(faces[face].cornerColorPos[r]);
        }
    }
    return allCornerPos;
}
std::vector<int> Megaminx::returnNativeEdgeColorPos()
{
    std::vector<int> allEdgePos;
    for (int face = 0; face < 12; ++face) {
        for (int r = 0; r < 5; ++r) {
            allEdgePos.push_back(faces[face].edgeColorPos[r]);
        }
    }
    return allEdgePos;
}

int Megaminx::LoadNewEdgesFromVector(const std::vector<int> &readEdges)
{
    assert(readEdges.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        std::vector<int> loadEdges = { readEdges[f + 0],readEdges[f + 1],readEdges[f + 2],readEdges[f + 3],readEdges[f + 4] };
        resetFacesEdges(face, loadEdges, false);
    }
    return 1;
}

int Megaminx::LoadNewCornersFromVector(const std::vector<int> &readCorners)
{
    assert(readCorners.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        std::vector<int> loadCorners = { readCorners[f + 0],readCorners[f + 1],readCorners[f + 2],readCorners[f + 3],readCorners[f + 4] };
        resetFacesCorners(face, loadCorners, false);
    }
    return 1;
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
    auto activeFace = faces[(color_n - 1)];
    //const auto defaultEdges = activeFace.defaultEdges;
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
    auto foundEdges2 = findEdges(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundEdges2 == defaultEdges);
    assert(foundEdges2.size() == 5);
    //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
    for (int j = 0; j < 5; ++j) {
        auto &cpos = activeFace.edgeColorPos[j];
        while (activeFace.edge[j]->data._colorNum[cpos] != color_n)
            activeFace.edge[j]->flip();
    }
    //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
    for (int j = 0; j < 5; ++j) {
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
    auto activeFace = faces[(color_n - 1)];
    //const auto defaultCorners = activeFace.defaultCorners;
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
    auto foundCorners2 = findCorners(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundCorners2 == defaultCorners);
    assert(foundCorners2.size() == 5);
    //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
    for (int j = 0; j < 5; ++j) {
        auto &cpos = activeFace.cornerColorPos[j];
        while (activeFace.corner[j]->data._colorNum[cpos] != color_n)
            activeFace.corner[j]->flip();
    }
    //Maybe Pieces got loaded in the wrong place on the face. (secondary colors dont match)
    for (int j = 0; j < 5; ++j) {
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
    constexpr int toplist[5] = { BEIGE, PINK, LIGHT_GREEN, ORANGE, LIGHT_BLUE };      //{12,11,10,9,8}
    constexpr int botlist[5] = { YELLOW, PURPLE, DARK_GREEN, RED, DARK_BLUE };        // { 6,5,4,3,2 }
    //Top half - Part 1:
    if(y1 && x < d)
        face = LIGHT_BLUE;
    //Bottom half - Part 1:
    else if(y2 && x < d)
        face = DARK_BLUE;
    if (face) return face;
    else if (y1 || y2) {
        for (int i = 0; i < 5; ++i) {
            if (x >= d + r * i && x < d + r * (i + 1)) {
                if (y1)
                    face = toplist[i];
                else if (y2)
                    face = botlist[i];
                if (face) return face;
            }
        }
    }
    //Part 2: offset by 180 Degrees, therefore the starting point is a diff color
    //equivalent to std::rotate cyclically advancing the list by + 2
    else if (y1b || y2b) {
        constexpr int toplist[5] = { LIGHT_GREEN, ORANGE, LIGHT_BLUE, BEIGE, PINK }; //{ 10,9,8,12,11}
        constexpr int botlist[5] = { DARK_GREEN, RED, DARK_BLUE, YELLOW, PURPLE };   // { 4,3,2,6,5 }
        for (int i = 0; i < 5; ++i) {
            if (x >= r * i && x < r * (i + 1)) {
                if (y1b)
                    face = toplist[i];
                else if (y2b)
                    face = botlist[i];
                if (face) return face;
            }
        }
    }
    else if (y3 && !face)    //Bottom {1}
        face = WHITE;
    else if ((y4a || y4b) && !face) //Top {7}
        face = GRAY;
    return face;
}

/**
 * \brief Algorithm Switcher Dispatcher. Queues multiple rotate ops to happen
 * in accordance with how a player would want to achieve certain swaps.
 * \param current_face from 1 - 12
 * \param i op # from 1 - 12 (coincidence)
 */
//TODO: Make a letter/notation parser to shorten all this down.
void Megaminx::rotateAlgo(int current_face, int i)
{
    assert(current_face > 0 && current_face <= numFaces);
    //assert(i > 0 && i <= 14);
    const colordirs &loc = g_faceNeighbors[current_face];
    switch (i) {
    // r u R' U' , 51
    case 1:
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::CCW);
        rotate(loc.up,   Face::CCW);
        break;
    // l u L' U' , 52
    case 2:
        rotate(loc.left, Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.left, Face::CCW);
        rotate(loc.up,   Face::CCW);
        break;
    // U' L' u l , 53
    case 3:
        rotate(loc.up,   Face::CCW);
        rotate(loc.left, Face::CCW);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.left, Face::Clockwise);
    case 41:
    // u r U' R' , 53
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::CCW);
        rotate(loc.right,Face::CCW);
        break;
    // r u R' u r 2U' R' , 54
        //Last Layer Step 2:
        // Rotating the star/Edge pieces into their correct positions.
        //The pieces that will remain in the same position are the 6 and 8 o'clock ones.
        //The remaining 3 will rotate in an Anti Clockwise fashion. Repeat until all correct positions.
        //NOTE: Affects corners too.
    case 4:
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::CCW);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::CCW);
        rotate(loc.up,   Face::CCW);
        rotate(loc.right,Face::CCW);
        break;
    // u l U' R' u L' U' r , 55 orient bottom Corners // 1,3,2
        //3, 5 and 7 o clock. will rotate with each other 
    //repeat 3x = undo
    case 5:
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.left, Face::Clockwise);
        rotate(loc.up,   Face::CCW);
        rotate(loc.right,Face::CCW);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.left, Face::CCW);
        rotate(loc.up,   Face::CCW);
        rotate(loc.right,Face::Clockwise);
        break;
    // u r 2U' L' 2u R' 2U' l u , 56
        //Last Layer Step 3:
        // Get the corner into their correct positions. // 3,5,4
        //The corners facing you at the 5 and 7 o'clock will stay and the other 3 corners will also rotate in an anticlockwise fashion!
    case 6:
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::CCW);
        rotate(loc.up,   Face::CCW);
        rotate(loc.left, Face::CCW);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::CCW);
        rotate(loc.up,   Face::CCW);
        rotate(loc.up,   Face::CCW);
        rotate(loc.left, Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        break;
    // R' D' R D , 57
        //Last Layer Step 4: Orientating the Corners.
        //You repeat with all corners until they are all orientated correctly.
    case 7:
        rotate(loc.right,Face::CCW);
        rotate(loc.downr,Face::CCW);
        rotate(loc.right,Face::Clockwise);
        rotate(loc.downr,Face::Clockwise);
        break;
    //Edge Permutation 1:
    case 8:
        // r2 U2' R2' U' r2 U2' R2' (5 to 2, 2 to 4, 4 to 5) //4 to 1, 1 to 3, 3 to 4
        //needs 5 executions;
        rotate(loc.right, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::CCW);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::CCW);
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::CCW);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::CCW);
        rotate(loc.right, Face::CCW);
        break;
    //Edge Permutation 2:
    case 9:
        // r2 u2  R2' u  r2 u2  R2' (5 to 4, 4 to 2, 2 to 5) //3 to 1, 1 to 4, 4 to 3 (opposite of last one)
        //needs 5 executions;
        //all the "up"s get reversed. nothing else changes from 1st one.
        rotate(loc.right, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        rotate(loc.right, Face::CCW);
        break;
    //Edge Permutation 3:
    case 10:
        // r u R' F', r  u  R' U', R' f r2 U' R' (5 to 1, 1 to 2, 2 to 5)
        //NOTE: corners are also affected
        // 3 repeats = undo
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        rotate(loc.front, Face::CCW);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::CCW);
        rotate(loc.front, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::CCW);
        break;
    //Edge Permutation 4:
    case 11:
        // r u R' u ,  R' U' r2 U',  R' u R' u,  r U2' (5 to 2, 2 to 1, 1 to 5)
        //Perfect Edges Swap, only needs one run.
        //opposite of the Edges one above , but corners aren't affected.?
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::Clockwise); //, 
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::CCW); //, 
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        rotate(loc.up, Face::Clockwise); //, 
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::CCW);
        rotate(loc.up, Face::CCW);
        break;
    //Edge Permutation 5:
    case 12:
        // l r u2,  L' u R',  l U' r u2,  L' u2 R' (5 and 3 swap, 1 and 2 swap) //1 and 5, 4 and 2
        //corners are also affected
        //repeating it again is Equalivalent to Undo
        rotate(loc.left, Face::Clockwise);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.up, Face::Clockwise); //, 
        rotate(loc.left, Face::CCW);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);    //, 
        rotate(loc.left, Face::Clockwise);
        rotate(loc.up, Face::CCW);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);  //, 
        rotate(loc.left, Face::CCW);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.up, Face::Clockwise);
        rotate(loc.right, Face::CCW);
        break;
    case 13:
        // f l ff L' F' Low Y's 12'oclock to 5 o clock.
        //First Y, right edge.
        rotate(loc.front, Face::Clockwise);
        rotate(loc.left, Face::Clockwise);
        rotate(loc.front, Face::Clockwise);
        rotate(loc.front, Face::Clockwise);
        rotate(loc.left, Face::CCW);
        rotate(loc.front, Face::CCW);
    case 14:
        //Other Y, Left Edge
        rotate(loc.front, Face::CCW);
        rotate(loc.right, Face::CCW);
        rotate(loc.front, Face::CCW);
        rotate(loc.front, Face::CCW);
        rotate(loc.right, Face::Clockwise);
        rotate(loc.front, Face::Clockwise);
    default:
        break;
    }
}
