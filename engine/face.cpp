#include <GL/glut.h>
#include <vector>
#include <cassert>
#include <initializer_list>
#include "face.h"

Face::Face()
{
    center = nullptr;
    turnDir = 0;
    thisNum = 0;
    rotating = false;
    angle = 0;
    axis[0] = 0;
    axis[1] = 0.001;
    axis[2] = -1;

}
/*
w* createNextImpl(std::true_type) { return new w(); }
n* createNextImpl(std::false_type) { return new n(); }

template <typename T>
T* createNext() {
    return createNextImpl(std::is_same<T, w>());
};
*/
//when this is run, iterate through and check to see which position the Face->center color is in
void Face::makeEdgePositionArray()
{
    for(int i = 0 ; i < 5 ; ++i)
        for (int j = 0; j < 2; ++j)
            if (edge[i]->data._colorNum[j] == thisNum + 1)
                edgeColorPos.push_back(j);
}

//connect the right matching Edge pieces to the face. and store the list.
void Face::attachEdgePieces(Edge& n, int numEdges)
{
    const auto pieceList = findPiece(n, numEdges);
    for (int i = 0; i < 5; ++i) {
        edge[i] = &n + pieceList[i];
    }
    edgeNativePos = pieceList;
    makeEdgePositionArray();
}

//when this is run, iterate through and check to see which position the Face->center color is in
void Face::makeCornerPositionArray()
{
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 3; ++j)
            if (corner[i]->data._colorNum[j] == thisNum + 1)
                cornerColorPos.push_back(j);
}

//connect the right Corner Edge pieces to the face. and store the list.
void Face::attachCornerPieces(Corner& n, int numCorners)
{
    const auto pieceList = findPiece(n, numCorners);
    for (int i = 0; i < 5; ++i) {
        corner[i] = &n + pieceList[i];
    }
    cornerNativePos = pieceList;
    makeCornerPositionArray();
}

/**
 * \brief  This finds the color to the center/Face (since a center is perm-attached to a face)
 *   and then iterates the entire list of pieces to find when the colors match, outputs a list.
 * \pieceRef Takes a reference to the [0]th member of Pointer_array of (either Corner/Edge's)
 * \times how many times to iterate (for simplicity).
 * \return Returns the list of 5 positions where the starting face's pieces have ended up at.
 */
std::vector<int> Face::findPiece(Piece& pieceRef, int times) const
{
    std::vector<int> pieceList;
    const auto color = center->data._colorNum[0];
    for (int i = 0; i < times; ++i) {
        const bool result = (&pieceRef)[i].matchesColor(color);
        if (result)
            pieceList.push_back(i);
    }
    return pieceList;
}

void Face::attachCenter(Center *a)
{
    center = a;
}

/**
 * \brief Pre-initialize center with a re-usable list
 * \param a The center to attach
 * \param centerVertexBase array of geometric vertexes
 */
void Face::attachCenter(Center *a, double* centerVertexBase)
{
    center = a;
    memcpy(&_vertex, centerVertexBase, sizeof(_vertex));
}

/**
 * \brief create our Face's Axes
 * We can re-use the Center axis creation
 * \param n is the number of this face
 */
void Face::initAxis(int n)
{
    assert(n < 12);
    thisNum = n;
    center->createAxis(n,axis);
    for (int i = 0; i < 5; ++i) {
        center->createAxis(n,_vertex[i]);
    }
}

/**
 * \brief Private. Simple-Flips (inverts) one Edge-piece
 * and then the other, individually.
 */
void Face::twoEdgesFlip(int a,int b)
{
    assert(a >= 0 && a < 5 && b >= 0 && b < 5);
    edge[a]->flip();
    edge[b]->flip();
}

//Private. Functional Generic Switch that flips
void Face::Flip(int a, int b, int c, int d, const int* pack)
{
    //Feed in 4 ints abcd representing the face's five corner indexes 0-4
    // (hint: [0-4]=5, but we only need to flip 4 at once)
    //Feed in these vector lists like { 0, 1, 1, 0 }; telling each index how to flip
    // Boolean ? 1 = Flip piece once ||  0      = Flip twice
    pack[0] ? corner[a]->flip() : corner[a]->flipTwice();
    pack[1] ? corner[b]->flip() : corner[b]->flipTwice();
    pack[2] ? corner[c]->flip() : corner[c]->flipTwice();
    pack[3] ? corner[d]->flip() : corner[d]->flipTwice();
}

//Private. Swap 4 Corners, given a list of 8 indexes
void Face::QuadSwapCorners(int const pack[8])
{
    swapCorners(pack[0], pack[1]);
    swapCorners(pack[2], pack[3]);
    swapCorners(pack[4], pack[5]);
    swapCorners(pack[6], pack[7]);
}
//Private. Swap 4 Edges, given a list of 8 indexes
void Face::QuadSwapEdges(int const pack[8])
{
    swapEdges(pack[0], pack[1]);
    swapEdges(pack[2], pack[3]);
    swapEdges(pack[4], pack[5]);
    swapEdges(pack[6], pack[7]);
}

/**
 * \brief Colorizing function. Intricate series of flips/swaps.
 * \param dir Each case is for each of the 12 faces,
 * / in order to get it to switch colors after it rotates.
 */
bool Face::placeParts(int dir)
{
    assert(dir == 1 || dir == -1);
    if (dir == 1) { // 1 = CCW
        switch (thisNum) {

        case 0:
            constexpr static int CCW0C[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
            QuadSwapCorners(CCW0C);
            constexpr static int CCW0E[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
            QuadSwapEdges(CCW0E);
            break;
        case 1:
            //1,2,3,4,5
            Flip(0, 1, 2, 4, FlipBackwards);
            constexpr static int CCW1C[8] = { 4, 0, 4, 2, 0, 3, 0, 1 };
            QuadSwapCorners(CCW1C);
            constexpr static int CCW1E[8] = { 4, 1, 1, 3, 0, 1, 0, 2 };
            QuadSwapEdges(CCW1E);
            twoEdgesFlip(1, 2);
            break;
        case 2:
            twoEdgesFlip(0, 3);
            constexpr static int CCW2E[8] = { 1, 0, 1, 2, 1, 3, 3, 4 };
            QuadSwapEdges(CCW2E);
            Flip(0, 1, 3, 4, FlipBackwards);
            constexpr static int CCW2C[8] = { 0, 1, 0, 2, 2, 3, 2, 4 };
            QuadSwapCorners(CCW2C);
            break;
        case 3:
            constexpr static int CCW3E[8] = { 3, 2, 4, 3, 0, 1, 1, 2 };
            QuadSwapEdges(CCW3E);
            twoEdgesFlip(1, 2);
            constexpr static int CCW3C[8] = { 3, 4, 1, 3, 1, 2, 0, 1 };
            QuadSwapCorners(CCW3C);
            Flip(1, 2, 3, 4, FlipAlternatingBackwards);
            break;
        case 4:
            constexpr static int CCW4E[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
            QuadSwapEdges(CCW4E);
            twoEdgesFlip(1, 2);
            constexpr static int CCW4C[8] = { 0, 1, 0, 3, 0, 4, 0, 2 };
            QuadSwapCorners(CCW4C);
            Flip(1, 2, 3, 4, FlipAlternatingBackwards);
            break;
        case 5:
            constexpr static int CCW5E[8] = { 2, 4, 2, 3, 0, 2, 0, 1 };
            QuadSwapEdges(CCW5E);
            twoEdgesFlip(1, 2);
            constexpr static int CCW5C[8] = { 1, 3, 1, 4, 1, 2, 0, 1 };
            QuadSwapCorners(CCW5C);
            Flip(1, 2, 3, 4, FlipAlternatingBackwards);
            break;
        case 6:
            constexpr static int CCW6C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
            QuadSwapCorners(CCW6C);
            constexpr static int CCW6E[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
            QuadSwapEdges(CCW6E);
            break;
        case 7:
            constexpr static int CCW7E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };
            QuadSwapEdges(CCW7E);
            twoEdgesFlip(3, 4);
            constexpr static int CCW7C[8] = { 0, 4, 0, 2, 0, 1, 0, 3 };
            QuadSwapCorners(CCW7C);
            Flip(0, 1, 3, 4, FlipOutwards);
            break;
        case 8:
            constexpr static int CCW8E[8] = { 0, 1, 1, 2, 2, 4, 3, 4 };
            QuadSwapEdges(CCW8E);
            twoEdgesFlip(3, 4);
            constexpr static int CCW8C[8] = { 0, 4, 1, 4, 1, 2, 2, 3 };
            QuadSwapCorners(CCW8C);
            Flip(0, 1, 3, 4, FlipAlternatingForwards);
            break;
        case 9:
            constexpr static int CCW9E[8] = { 0, 1, 1, 2, 2, 4, 3, 4 };
            QuadSwapEdges(CCW9E);
            twoEdgesFlip(3, 4);
            constexpr static int CCW9C[8] = { 0, 4, 1, 4, 1, 2, 2, 3 };
            QuadSwapCorners(CCW9C);
            Flip(0, 1, 3, 4, FlipAlternatingForwards);
            break;
        case 10:
            constexpr static int CCW10E[8] = { 0, 1, 1, 3, 3, 4, 2, 4 };
            QuadSwapEdges(CCW10E);
            twoEdgesFlip(2, 4);
            constexpr static int CCW10C[8] = { 0, 4, 1, 4, 1, 2, 2, 3 };
            QuadSwapCorners(CCW10C);
            Flip(0, 1, 3, 4, FlipAlternatingForwards);
            break;
        case 11:
            constexpr static int CCW11E[8] = { 0, 3, 0, 4, 0, 2, 0, 1 };
            QuadSwapEdges(CCW11E);
            twoEdgesFlip(3, 4);
            constexpr static int CCW11C[8] = { 0, 3, 0, 1, 0, 2, 0, 4 };
            QuadSwapCorners(CCW11C);
            Flip(0, 2, 3, 4, FlipAlternatingForwards);
            break;
        default:
            break;
        }
    }
    else {
        // -1 = CW
        switch (thisNum) {
        case 0:
            constexpr static int CW0C[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
            QuadSwapCorners(CW0C);
            constexpr static int CW0E[8] = { 0, 1, 4, 0, 3, 4, 2, 3 };
            QuadSwapEdges(CW0E);
            break;
        case 1:
            Flip(0, 2, 3, 4, FlipForwards);
            constexpr static int CW1C[8] = { 0, 1, 0, 3, 4, 2, 4, 0 };
            QuadSwapCorners(CW1C);
            constexpr static int CW1E[8] = { 0, 2, 0, 1, 1, 3, 4, 1 };
            QuadSwapEdges(CW1E);
            twoEdgesFlip(0, 3);
            break;
        case 2:
            twoEdgesFlip(1, 2);
            constexpr static int CW2E[8] = { 3, 4, 1, 3, 1, 2, 1, 0 };
            QuadSwapEdges(CW2E);
            Flip(1, 2, 3, 4, FlipAlternatingForwards);
            constexpr static int CW2C[8] = { 2, 4, 2, 3, 0, 2, 0, 1 };
            QuadSwapCorners(CW2C);
            break;
        case 3:
            constexpr static int CW3E[8] = { 1, 2, 0, 1, 4, 3, 3, 2 };
            QuadSwapEdges(CW3E);
            twoEdgesFlip(0, 3);
            constexpr static int CW3C[8] = { 0, 1, 1, 2, 1, 3, 3, 4 };
            QuadSwapCorners(CW3C);
            Flip(0, 1, 3, 4, FlipForwards);
            break;
        case 4:
            constexpr static int CW4E[8] = { 3, 4, 1, 3, 1, 2, 0, 1 };
            QuadSwapEdges(CW4E);
            twoEdgesFlip(0, 3);
            constexpr static int CW4C[8] = { 0, 2, 0, 4, 0, 3, 0, 1 };
            QuadSwapCorners(CW4C);
            Flip(0, 1, 3, 4, FlipForwards);
            break;
        case 5:
            constexpr static int CW5E[8] = { 0, 1, 0, 2, 2, 3, 2, 4 };
            QuadSwapEdges(CW5E);
            twoEdgesFlip(0, 3);
            constexpr static int CW5C[8] = { 0, 1, 1, 2, 1, 4, 1, 3 };
            QuadSwapCorners(CW5C);
            Flip(0, 1, 3, 4, FlipForwards);
            break;
        case 6:
            constexpr static int CW6C[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
            QuadSwapCorners(CW6C);
            constexpr static int CW6E[8] = { 0, 1, 1, 2, 2, 3, 3, 4 };
            QuadSwapEdges(CW6E);
            break;
        case 7: //front CW;
            constexpr static int CW7E[8] = { 0, 1, 0, 2, 0, 4, 0, 3 };
            QuadSwapEdges(CW7E);
            twoEdgesFlip(0, 3);
            constexpr static int CW7C[8] = { 0, 3, 0, 1, 0, 2, 0, 4 };
            QuadSwapCorners(CW7C);
            Flip(0, 1, 2, 3, FlipInwards);
            break;
        case 8:
            constexpr static int CW8E[8] = { 3, 4, 2, 4, 1, 2, 0, 1 };
            QuadSwapEdges(CW8E);
            twoEdgesFlip(0, 3);
            constexpr static int CW8C[8] = { 2, 3, 1, 2, 1, 4, 0, 4 };
            QuadSwapCorners(CW8C);
            Flip(0, 1, 2, 4, FlipInwards);
            break;
        case 9:
            constexpr static int CW9E[8] = { 3, 4, 2, 4, 1, 2, 0, 1 };
            QuadSwapEdges(CW9E);
            twoEdgesFlip(0, 3);
            constexpr static int CW9C[8] = { 2, 3, 1, 2, 1, 4, 0, 4 };
            QuadSwapCorners(CW9C);
            Flip(0, 1, 2, 4, FlipInwards);
            break;
        case 10:
            constexpr static int CW10E[8] = { 2, 4, 3, 4, 1, 3, 0, 1 };
            QuadSwapEdges(CW10E);
            twoEdgesFlip(0, 2);
            constexpr static int CW10C[8] = { 2, 3, 1, 2, 1, 4, 0, 4 };
            QuadSwapCorners(CW10C);
            Flip(0, 1, 2, 4, FlipInwards);
            break;
        case 11:
            constexpr static int CW11E[8] = { 0, 1, 0, 2, 0, 4, 0, 3 };
            QuadSwapEdges(CW11E);
            twoEdgesFlip(0, 3);
            constexpr static int CW11C[8] = { 0, 4, 0, 2, 0, 1, 0, 3 };
            QuadSwapCorners(CW11C);
            Flip(0, 1, 2, 4, FlipInwards);
            break;
        default:
            break;
        }
    }
    return true;
}


/**
 * \brief OpenGL Display function. Calling this makes the faces rotate,the only real move.
 * \return true if we full-spun, to tell the parent function that rotating=false also.
 */
bool Face::render()
{
    //4 is the current rotational turnspeed for turnDir
    constexpr int turnspeed = 4;
    if (rotating) angle += turnDir * turnspeed;
    if (angle >= 56 || angle <= -56) {
        if (rotating) angle -= turnDir * (turnspeed/2);
    }
    glPushMatrix();
    glRotated(angle, axis[0], axis[1], axis[2]);

    for (int i = 0; i < 5; ++i) {
        corner[i]->render();
        edge[i]->render();
        glVertex3dv(_vertex[i]);
    }
    center->render();
    glLineWidth(4);
    glColor3d(0, 0, 0);
    glPopMatrix();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i) {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
    }
    glEnd();

    if (angle >= 72 || angle <= -72) {
        angle = 0;
        rotating = false;
        return placeParts(turnDir);
    }
    return false;
}

/**
 * \brief Public. Calling this sets off a chain of events in the render loops to rotate.
 * \param direction turn direction - 1 for Right, -1 for left.
 */
void Face::rotate(int direction)
{
    assert(direction == 1 || direction == -1);
    rotating = true;
    turnDir = direction;
}

/**
 * \brief Public. Given two local indexes 0-5, swap the Corners.
 */
void Face::swapCorners(int a, int b)
{
    assert(a >= 0 && a < 5 && b >= 0 && b < 5);
    corner[a]->swapdata(corner[b]->data);
}

/**
 * \brief Public. given two local indexes 0-5, swap the Edges.
 */
void Face::swapEdges(int a, int b)
{
    assert(a >= 0 && a < 5 && b >= 0 && b < 5);
    edge[a]->swapdata(edge[b]->data);
}