#include <vector>
#include <cassert>
#include "megaminx.h"

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

//connect the right matching Edge pieces to the face. and store the list.
void Face::attachEdgePieces(Edge& n)
{
    edgeNativePos = Face::findPiece(n, Megaminx::numEdges);
    for (int i = 0; i < 5; ++i) {
        edge[i] = &n + edgeNativePos[i];
        for (int j = 0; j < 2; ++j) {
            if (edge[i]->data._colorNum[j] == thisNum + 1)
                edgeColorPos.push_back(j);
        }
    }
}

//connect the right matching Corner pieces to the face. and store the list.
void Face::attachCornerPieces(Corner& n)
{
    cornerNativePos = Face::findPiece(n, Megaminx::numCorners);
    for (int i = 0; i < 5; ++i) {
        corner[i] = &n + cornerNativePos[i];
        for (int j = 0; j < 3; ++j) {
            if (corner[i]->data._colorNum[j] == thisNum + 1)
                cornerColorPos.push_back(j);
        }
    }
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
    const int color = center->data._colorNum[0];
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
 * \param direction turn direction: -1 for Right, +1 for left (seems/is backwards).
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
void Face::TwoEdgesFlip(int a,int b)
{
    assert(a >= 0 && a < 5 && b >= 0 && b < 5);
    edge[a]->flip();
    edge[b]->flip();
}

//Private. Functional Generic Switch that flips
void Face::FlipCorners(int a, int b, int c, int d, const int* pack)
{
    //Feed in 4 ints a,b,c,d representing four of the face's Five Corner indexes (Range 0-4)
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

    assert(dir == Face::CCW || dir == Face::CW);
    if (dir == Face::CounterClockwise) { // 1 = CCW = Left Turn = Counter-ClockWise
        switch (thisNum) {
        case 0: //WHITE
            QuadSwapEdges(CCW0E);
            QuadSwapCorners(CCW0C);            
            break;
        case 1: //DARK_BLUE
            QuadSwapEdges(CCW1E);
            TwoEdgesFlip(1, 2);
            //FlipCorners(0, 1, 2, 4, FlipBackwards);  //This can come first instead, if we skip the 3 instead of the 1.
            QuadSwapCorners(CCW1C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
            break;
        case 2: //RED
            QuadSwapEdges(CCW2E);
            TwoEdgesFlip(1, 2);
            //FlipCorners(0, 1, 3, 4, FlipBackwards);  //This can come first, instead, if we reverse the 2 and the 0
            QuadSwapCorners(CCW2C);
            FlipCorners(1, 2, 3, 4, FlipBackwardAlt);
            break;
        case 3: //DARK_GREEN
            QuadSwapEdges(CCW3E);
            TwoEdgesFlip(1, 2);
            QuadSwapCorners(CCW3C);
            FlipCorners(1, 2, 3, 4, FlipBackwardAlt);
            break;
        case 4: //PURPLE
            QuadSwapEdges(CCW4E);
            TwoEdgesFlip(1, 2);
            QuadSwapCorners(CCW4C);
            FlipCorners(1, 2, 3, 4, FlipBackwardAlt);
            break;
        case 5: //YELLOW
            QuadSwapEdges(CCW5E);
            TwoEdgesFlip(1, 2);
            QuadSwapCorners(CCW5C);
            FlipCorners(1, 2, 3, 4, FlipBackwardAlt);
            break;
        case 6: //GRAY
            QuadSwapEdges(CCW6E);
            QuadSwapCorners(CCW6C);            
            break;
        case 7: //LIGHT_BLUE Front Face, Left Turn = Counter-ClockWise;
            QuadSwapEdges(CCW7E);
            TwoEdgesFlip(3, 4);
            QuadSwapCorners(CCW7C);
            FlipCorners(0, 1, 2, 3, FlipForwardAlt);
            break;
        case 8: //ORANGE
            QuadSwapEdges(CCW8E);
            TwoEdgesFlip(3, 4);
            QuadSwapCorners(CCW8C);
            FlipCorners(0, 1, 2, 3, FlipForwards);
            break;
        case 9: //GREEN
            QuadSwapEdges(CCW9E);
            TwoEdgesFlip(3, 4);
            QuadSwapCorners(CCW9C);
            FlipCorners(0, 1, 2, 3, FlipForwards);
            break;
        case 10: //PINK
            QuadSwapEdges(CCW10E);
            TwoEdgesFlip(2, 4);
            QuadSwapCorners(CCW10C);
            FlipCorners(0, 1, 2, 3, FlipForwards);
            break;
        case 11: //BEIGE
            QuadSwapEdges(CCW11E);
            TwoEdgesFlip(3, 4);
            QuadSwapCorners(CCW11C);
            FlipCorners(0, 1, 2, 4, FlipForwards);
            break;
        default:
            break;
        }
    }
    else {  // -1 = CW = Right Turn = ClockWise
        switch (thisNum) {
        case 0: //WHITE
            QuadSwapEdges(CW0E);
            QuadSwapCorners(CW0C);            
            break;
        case 1: //DARK_BLUE
            QuadSwapEdges(CW1E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW1C);
            FlipCorners(0, 1, 2, 4, FlipForwards);
            break;
        case 2: //RED (inverse order for fun)
            TwoEdgesFlip(1, 2);
            QuadSwapEdges(CW2E);
            FlipCorners(1, 2, 3, 4, FlipForwardAlt);
            QuadSwapCorners(CW2C);
            break;
        case 3: //DARK_GREEN
            QuadSwapEdges(CW3E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW3C);
            FlipCorners(0, 1, 3, 4, FlipForwards);
            break;
        case 4: //PURPLE
            QuadSwapEdges(CW4E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW4C);
            FlipCorners(0, 1, 3, 4, FlipForwards);
            break;
        case 5: //YELLOW
            QuadSwapEdges(CW5E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW5C);
            FlipCorners(0, 1, 3, 4, FlipForwards);
            break;
        case 6: //GRAY
            QuadSwapEdges(CW6E);
            QuadSwapCorners(CW6C);            
            break;
        case 7: //LIGHT_BLUE Front Face, Right Turn = ClockWise;
            QuadSwapEdges(CW7E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW7C);
            FlipCorners(0, 1, 3, 4, FlipBackwards);
            break;
        case 8: //ORANGE
            QuadSwapEdges(CW8E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW8C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
            break;
        case 9: //GREEN
            QuadSwapEdges(CW9E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW9C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
            break;
        case 10: //PINK (Corner Star = 0 bottom, 1 right lower, 2 left lower, 3 left upper, 4 right upper)
            //corner[1]->initColor(0, 0, 0); //set to black to find a piece
            QuadSwapEdges(CW10E);
            TwoEdgesFlip(0, 2);
            QuadSwapCorners(CW10C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
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
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW11C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
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
        //temp diagnose:
        //corner[i]->initColor(i, i); //set to solid colors to find order of pieces
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
