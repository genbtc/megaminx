/* MegaMinx2 v1.29 - 2017 - genBTC mod
 * Uses code from Taras Khalymon (tkhalymon) / @cybervisiontech / taras.khalymon@gmail.com
 * genBTC November 2017 - genbtc@gmx.com / @genr8_ / github.com/genbtc/
 * genBTC December 2017 - fixups, tweaks.
 */
#include <cassert>
#include <algorithm>
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
    _rotatingFaceIndex = 0;
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
        faces[i].attachEdgePieces(edges[0], numEdges);
        faces[i].attachCornerPieces(corners[0], numCorners);
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

    //Finish the rotation Queue
    const bool isRotaFullyRendered = faces[_rotatingFaceIndex].render();
    if (isRotaFullyRendered) {
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

//Scramble 600 times (50 x 12)
void Megaminx::scramble()
{
    //Do 50 iterations of scrambling (like a human)
    for (int q = 0; q < 50; q++) {
        //numFaces faces - turn one each, randomizing direction
        for (int i = 0; i < numFaces; i++) {
            const int r = rand() % 2 * 2 - 1;
            faces[i].placeParts(r);
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
    assert(i > 0);
    assert(i < numFaces);
    assert(x < 5);
    faces[i].corner[x]->flip();
}
/**
 * \brief Toggle the colors of a single Edge piece
 * * If called externally make sure its color 1-12.
 * \param i Nth-face's number (color) [0-11]
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::swapOneEdge(int i, int x)
{
    assert(i > 0);
    assert(i < numFaces);
    assert(x < 5);
    faces[i].edge[x]->flip();
}

/**
 * \brief Makes a pointer to g_currentFace
 * \param i Nth-face number color (1-12)
 */
void Megaminx::setCurrentFaceActive(int i)
{
    assert(i > 0);
    assert(i <= numFaces);
    i -= 1;     //Convert 1-numFaces Faces into array 0-11
    g_currentFace = &faces[i];
    assert(g_currentFace->thisNum == i);    //double check.
}

/**
 * \brief Reset all the pieces on a face and set it to active.
 * \param n Nth-face number color (1-12)
 * \return n same thing.
 */
void Megaminx::resetFace(int n)
{
    assert(n > 0);
    assert(n <= numFaces);
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
    assert(i > 0);
    assert(i <= numFaces);
    i -= 1;     //Convert 1-numFaces Faces into array 0-11
    return faces[i].findPiece(edges[0], numEdges);
}

/**
 * \brief Find all corner pieces.
 * \param i Nth-face number color (1-12)
 * \return std::vector, 5 long of index positions of found corners
 */
std::vector<int> Megaminx::findCorners(int i)
{
    assert(i > 0);
    assert(i <= numFaces);
    i -= 1;     //Convert 1-numFaces Faces into array 0-11
    return faces[i].findPiece(corners[0], numCorners);
}

/**
 * \brief Revert all the edge pieces on the Nth colored face back to normal.
 *          To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number (1-12)
 * \return success (1)
 */
int Megaminx::resetFacesEdges(int color_n)
{
    assert(color_n > 0);
    assert(color_n <= numFaces);
    const auto activeFace = faces[(color_n - 1)];
    const auto defaultEdges = activeFace.edgeNativePos;
    auto foundEdges = findEdges(color_n);
    assert(foundEdges.size() == 5);
    for (size_t j = 0; j < foundEdges.size(); ++j) {
        if (activeFace.edge[j]->matchesColor(color_n))
            continue;
        edges[foundEdges[j]].swapdata(activeFace.edge[j]->data);
        j = -1;
    }
    auto epos = activeFace.edgeColorPos;
    auto foundEdges2 = findEdges(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundEdges2 == defaultEdges);
    assert(foundEdges2.size() == 5);
    //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
    for (size_t j = 0; j < foundEdges2.size(); ++j) {
        while (activeFace.edge[j]->data._colorNum[epos[j]] != color_n)
            activeFace.edge[j]->flip();
    }
    return 1;
}

/**
 * \brief Revert all the Corners pieces on the Nth colored face back to normal.
 *          To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number (1-12)
 * \return success (1)
 */
int Megaminx::resetFacesCorners(int color_n)
{
    assert(color_n > 0);
    assert(color_n <= numFaces);
    const auto& activeFace = faces[(color_n - 1)];
    const auto& defaultCorners = activeFace.cornerNativePos;
    auto& foundCorners = findCorners(color_n);
    assert(foundCorners.size() == 5);
    for (size_t j = 0; j < foundCorners.size(); ++j) {
        if (activeFace.corner[j]->matchesColor(color_n))
            continue;
        corners[foundCorners[j]].swapdata(activeFace.corner[j]->data);
        j = -1;
    }
    auto& cpos = activeFace.cornerColorPos;
    auto& foundCorners2 = findCorners(color_n);
    //assert check just double checking - we dont want to get stuck in while
    assert(foundCorners2 == defaultCorners);
    assert(foundCorners2.size() == 5);
    //Pieces are in the right place but maybe wrong orientation, so Swap the colors:
    for (size_t j = 0; j < foundCorners2.size(); ++j) {
        while (activeFace.corner[j]->data._colorNum[cpos[j]] != color_n)
            activeFace.corner[j]->flip();
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
    const auto& y1  = y >= (s - d) && y <= (s + d);                  // 60
    const auto& y1b = y >= (s + 240 - d) && y <= (s + 240 + d);      //300 (other opposite)
    const auto& y2  = y >= (s + 180 - d) && y <= (s + 180 + d);      //240
    const auto& y2b = y >= (s +  60 - d) && y <= (s +  60 + d);      //120 (other opposite)
    const auto& y3  = y >= (s + 120 - d) && y <= (s + 120 + d);      //180
    const auto& y4a = y >= (0 - d)   && y <= (0 + d);                //0
    const auto& y4b = y >= (360 - d) && y <= (360 + d);              //360
    int toplist[5] = { BEIGE, PINK, LIGHT_GREEN, ORANGE, LIGHT_BLUE };   //{12,11,10,9,8}
    int botlist[5] = { YELLOW, PURPLE, DARK_GREEN, RED, DARK_BLUE };                //{6,5,4,3,2}
    //Top half - Part 1:
    if(y1 && x < d + r)
        face = LIGHT_BLUE;
    //Bottom half - Part 1:
    else if(y2 && x < d + r)
        face = DARK_BLUE;
    for (int i = 0; i < 5; ++i) {
        if (y1 && x >= d + r * i && x < d + r * (i + 1))
            face = toplist[i];
        else if (y2 && x >= d + r * i && x < d + r * (i + 1))
            face = botlist[i];
    }
    if (face) return face;
    //Top half - Part 2: offset by 180 Degrees, therefore the starting point is a diff color(+2)
    //Bottom half - Part 2: offset by 180 Degrees, therefore the starting point is a diff color(+2).
    //std::rotate is to cyclically advance the list by +2.
    std::rotate(std::begin(toplist), std::begin(toplist) + 2, std::end(toplist));
    std::rotate(std::begin(botlist), std::begin(botlist) + 2, std::end(botlist));
    for (int i = 0; i < 5; ++i) {
        if (y1b && x >= r * i && x < r * (i + 1))
            face = toplist[i];
        else if (y2b && x >= r * i && x < r * (i + 1))
            face = botlist[i];
    }
    if (face) return face;
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
 * \param i op # from 1 - 7
 */
void Megaminx::rotateAlgo(int current_face, int i)
{
    assert(current_face > 0);
    assert(current_face <= numFaces);
    assert(i > 0 && i < 8);
    const auto& loc = g_faceNeighbors[current_face];
    switch (i) {
    //("r u R' U'", 51);
    case 1:
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::CCW);
        rotate(loc.up,   Face::CCW);
        break;
    //("l u L' U'", 52);
    case 2:
        rotate(loc.left, Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.left, Face::CCW);
        rotate(loc.up,   Face::CCW);
        break;
    //("U' L' u l u r U' R'", 53);
    case 3:
        rotate(loc.up,   Face::CCW);
        rotate(loc.left, Face::CCW);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.left, Face::Clockwise);
        rotate(loc.up,   Face::Clockwise);
        rotate(loc.right,Face::Clockwise);
        rotate(loc.up,   Face::CCW);
        rotate(loc.right,Face::CCW);
        break;
    //("r u R' u r 2U' R'", 54);
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
    //("u l U' R' u L' U' r", 55);
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
    //("u r 2U' L' 2u R' 2U' l u", 56);
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
    //("R' D' R D", 57);
    case 7:
        rotate(loc.right,Face::CCW);
        rotate(loc.downr,Face::CCW);
        rotate(loc.right,Face::Clockwise);
        rotate(loc.downr,Face::Clockwise);
        break;
    default:
        break;
    }
}
