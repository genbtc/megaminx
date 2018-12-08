#include <vector>
#include <cassert>
#include "face.h"
#include "../common_physics/utils.h"

Face::Face()
{
    center = nullptr;
    rotating = false;
    angle = 0;
    thisNum = 0;
    turnDir = 0;        
    axis[0] = 0;
    axis[1] = 0.001;
    axis[2] = -1;
}

//when this is run, iterate through and check to see which position the Face->center color is in
template <typename T>
void Face::makePositionArray(int rows)
{
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < rows; ++j) {
            if constexpr (std::is_same_v < T, Edge >) {
                if (edge[i]->data._colorNum[j] == thisNum + 1)
                    edgeColorPos.push_back(j);
            }
            else if constexpr (std::is_same_v < T, Corner >) {
                if (corner[i]->data._colorNum[j] == thisNum + 1)
                    cornerColorPos.push_back(j);
            }
        }
    }
}

//connect the right matching Edge pieces to the face. and store the list.
void Face::attachEdgePieces(Edge& n, int numEdges)
{
    edgeNativePos = findPiece(n, numEdges);
    for (int i = 0; i < 5; ++i) {
        edge[i] = &n + edgeNativePos[i];
    }
    makePositionArray<Edge>(2);
}

//connect the right Corner Edge pieces to the face. and store the list.
void Face::attachCornerPieces(Corner& n, int numCorners)
{
    cornerNativePos = findPiece(n, numCorners);
    for (int i = 0; i < 5; ++i) {
        corner[i] = &n + cornerNativePos[i];
    }
    makePositionArray<Corner>(3);
}

/**
 * \brief  This finds the color to the center/Face (since a center is perm-attached to a face)
 *   and then iterates the entire list of pieces to find when the colors match, outputs a list.
 * \pieceRef Takes a reference to the [0]th member of Pointer_array of (either Corner/Edge's)
 * \times how many times to iterate over the ref'd array
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
 * \brief Public. Calling this sets off a chain of events in the render loops to rotate.
 * \param direction turn direction - 1 for Right, -1 for left.
 */
void Face::rotate(int direction)
{
    assert(direction == Face::Clockwise || direction == Face::CCW);
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
    //Feed in 4 ints a,b,c,d representing the face's Four Corner indexes (Range 0-4)
    //Feed in these Flip lists like { 0, 1, 1, 0 }; telling each index how to flip
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
            QuadSwapCorners(CCW0C);
            QuadSwapEdges(CCW0E);
            break;
        case 1:
            Flip(0, 1, 2, 4, FlipBackwards);
            QuadSwapCorners(CCW1C);
            QuadSwapEdges(CCW1E);
            twoEdgesFlip(1, 2);
            break;
        case 2:
            twoEdgesFlip(0, 3);
            QuadSwapEdges(CCW2E);
            Flip(0, 1, 3, 4, FlipBackwards);
            QuadSwapCorners(CCW2C);
            break;
        case 3:
            QuadSwapEdges(CCW3E);
            twoEdgesFlip(1, 2);
            QuadSwapCorners(CCW3C);
            Flip(1, 2, 3, 4, FlipAlternatingBackwards);
            break;
        case 4:
            QuadSwapEdges(CCW4E);
            twoEdgesFlip(1, 2);
            QuadSwapCorners(CCW4C);
            Flip(1, 2, 3, 4, FlipAlternatingBackwards);
            break;
        case 5:
            QuadSwapEdges(CCW5E);
            twoEdgesFlip(1, 2);
            QuadSwapCorners(CCW5C);
            Flip(1, 2, 3, 4, FlipAlternatingBackwards);
            break;
        case 6:
            QuadSwapCorners(CCW6C);
            QuadSwapEdges(CCW6E);
            break;
        case 7:
            QuadSwapEdges(CCW7E);
            twoEdgesFlip(3, 4);
            QuadSwapCorners(CCW7C);
            Flip(0, 1, 2, 3, FlipAlternatingForwards);
            break;
        case 8:
            QuadSwapEdges(CCW8E);
            twoEdgesFlip(3, 4);
            QuadSwapCorners(CCW8C);
            Flip(0, 1, 2, 3, FlipForwards);
            break;
        case 9:
            QuadSwapEdges(CCW9E);
            twoEdgesFlip(3, 4);
            QuadSwapCorners(CCW9C);
            Flip(0, 1, 2, 3, FlipForwards);
            break;
        case 10: //works
            QuadSwapEdges(CCW10E);
            twoEdgesFlip(2, 4);
            QuadSwapCorners(CCW10C);
            Flip(0, 1, 2, 3, FlipForwards);
            break;
        case 11: //works
            QuadSwapEdges(CCW11E);
            twoEdgesFlip(3, 4);
            QuadSwapCorners(CCW11C);
            Flip(0, 1, 2, 4, FlipForwards);
            break;
        default:
            break;
        }
    }
    else {
        // -1 = CW = Right Turn = ClockWise;
        switch (thisNum) {
        case 0:
            QuadSwapCorners(CW0C);
            QuadSwapEdges(CW0E);
            break;
        case 1:
            Flip(0, 2, 3, 4, FlipForwards);
            QuadSwapCorners(CW1C);
            QuadSwapEdges(CW1E);
            twoEdgesFlip(0, 3);
            break;
        case 2:
            twoEdgesFlip(1, 2);
            QuadSwapEdges(CW2E);
            Flip(1, 2, 3, 4, FlipAlternatingForwards);
            QuadSwapCorners(CW2C);
            break;
        case 3:
            QuadSwapEdges(CW3E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW3C);
            Flip(0, 1, 3, 4, FlipForwards);
            break;
        case 4:
            QuadSwapEdges(CW4E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW4C);
            Flip(0, 1, 3, 4, FlipForwards);
            break;
        case 5:
            QuadSwapEdges(CW5E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW5C);
            Flip(0, 1, 3, 4, FlipForwards);
            break;
        case 6:
            QuadSwapCorners(CW6C);
            QuadSwapEdges(CW6E);
            break;
        case 7: //LIGHT_BLUE Front Face, Right Turn = ClockWise;
            QuadSwapEdges(CW7E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW7C);
            Flip(0, 1, 3, 4, FlipBackwards);
            break;
        case 8: //ORANGE
            QuadSwapEdges(CW8E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW8C);
            Flip(0, 2, 3, 4, FlipBackwards);
            break;
        case 9: //GREEN
            QuadSwapEdges(CW9E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW9C);
            Flip(0, 2, 3, 4, FlipBackwards);
            break;
        case 10: //PINK (Corner Star = 0 bottom, 1 right lower, 2 left lower, 3 left upper, 4 right upper)
//            corner[1]->initColor(0, 0, 0); //set to black to find a piece
            QuadSwapEdges(CW10E);
            twoEdgesFlip(0, 2);
            QuadSwapCorners(CW10C);
            Flip(0, 2, 3, 4, FlipBackwards);
            //swapCorners(4, 1);
            //swapCorners(4, 0);
            //swapCorners(4, 2);
            //swapCorners(4, 3);
            //1 ? corner[0]->flip() : corner[0]->flipTwice();
            //1 ? corner[1]->flip() : corner[1]->flipTwice();
            //0 ? corner[2]->flip() : corner[2]->flipTwice();
            //0 ? corner[3]->flip() : corner[3]->flipTwice();
            break;
        case 11: //BEIGE
            QuadSwapEdges(CW11E);
            twoEdgesFlip(0, 3);
            QuadSwapCorners(CW11C);
            Flip(0, 2, 3, 4, FlipBackwards);
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
    glPushMatrix();
    //8 is the current rotational turnspeed for turnDir
    constexpr int turnspeed = 8;
    if (rotating)
        angle += turnDir * turnspeed;
    if (rotating && angle >= 56 || angle <= -56)
        angle -= turnDir * (turnspeed/2);
    if (angle)
        glRotated(angle, axis[0], axis[1], axis[2]);
    for (int i = 0; i < 5; ++i) {
        corner[i]->render();
        edge[i]->render();
    }
    center->render();
    glPopMatrix();
    glColor3d(0, 0, 0);
    makeGLpentagon(_vertex, 1.0 , GL_POLYGON);
    if (angle >= 72 || angle <= -72) {
        angle = 0;
        rotating = false;
        return placeParts(turnDir);
    }
    return false;
}
