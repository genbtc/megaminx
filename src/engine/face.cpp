#include "megaminx.hpp"
#include "../ui/opengl.h"
#include <cstring>  //memcpy

constexpr int turnspeed = 32; //144 is max, 1 is min (for render() @ end of file)

Face::Face()
{
    center = nullptr;
    rotating = false;
    angle = 0;
    thisFaceNum = 0;
    turnDir = 0;
    axis[0] = 0;
    axis[1] = 1e-8;
    axis[2] = -1;
}

/**
 * \brief Pre-initialize center with a re-usable list
 * \param c The center to attach
 * \param centerVertexBase array of geometric vertexes
 */
void Face::attachCenter(Center *c, double* centerVertexBase)
{
    center = c;
    memcpy(&_vertex, centerVertexBase, sizeof(_vertex));
    normalFvec = normalF.get_normal(Vec3d(_vertex[0]),Vec3d(_vertex[1]),Vec3d(_vertex[2]));
}

/**
 * \brief create our Face's Axes (we can re-use the Center axis creation)
 * \param n is the number of this face 0-11
 */
void Face::initAxis(int n)
{
    assert(n >= 0 && n <= 11);
    thisFaceNum = n;    //0-11
    center->createAxis(n, axis);
    for (int i = 0; i < 5; ++i)
        center->createAxis(n, _vertex[i]);
}

/** \brief connect matching Edge pieces to the face. and store the list. */
void Face::attachEdgePieces(const Megaminx* megaminx, Edge &edgesPTR)
{
    defaultEdges = megaminx->findPiecesOfFace(thisFaceNum+1, edgesPTR, Megaminx::numEdges);
    for (int i = 0; i < 5; ++i) {
        edge[i] = &edgesPTR + defaultEdges[i];
        assert(edge[i]->data.pieceNum == defaultEdges[i]);
    }
}

/** \brief connect matching Corner pieces to the face. and store the list. */
void Face::attachCornerPieces(const Megaminx* megaminx, Corner &cornersPTR)
{
    defaultCorners = megaminx->findPiecesOfFace(thisFaceNum+1, cornersPTR, Megaminx::numCorners);
    for (int i = 0; i < 5; ++i) {
        corner[i] = &cornersPTR + defaultCorners[i];
        assert(corner[i]->data.pieceNum == defaultCorners[i]);
    }
}

/**
* \brief Returns an EXACT ORDER list of pieces on[Face], (either Edge or Corner piece)
* \return List of face's pieces in 1-5 face order
//Compiles an accurate colorsolvedmaybe list.  (piece exists but color maybe rotated)
*/
template <typename T>
std::vector<int> Face::findPiecesOrder() const
{
    std::vector<int> pieceOrder;
    for (int i = 0; i < 5; ++i) {
        const auto piece = getFacePiece<T>(i);
        pieceOrder.push_back(piece->data.pieceNum);
    }
    return pieceOrder;
}
std::vector<int> Face::findCornersOrder() const { return findPiecesOrder<Corner>(); }
std::vector<int> Face::findEdgesOrder() const { return findPiecesOrder<Edge>(); }

template <typename T>
std::vector<int> Face::findPiecesColorFlipStatus() const
{
    std::vector<int> pieceColor;
    for (int i = 0; i < 5; ++i) {
        const auto piece = getFacePiece<T>(i);
        pieceColor.push_back(piece->data.flipStatus);
    }
    return pieceColor;
}
std::vector<int> Face::findCornersColorFlipStatus() const { return findPiecesColorFlipStatus<Corner>(); }
std::vector<int> Face::findEdgesColorFlipStatus() const { return findPiecesColorFlipStatus<Edge>(); }

/** \brief General usage: Is this piecenum on this face anywhere 1-5?
   Is this 1-30 piecenum on this face anywhere 1-5 edges?
   Is this 1-20 piecenum on this face anywhere 1-5 corners?
*/
template <typename T>
int Face::find5PiecePresent(int pieceNum) const
{
    for (int i = 0; i < 5; ++i) {
        const auto piece = getFacePiece<T>(i);
        if (piece->data.pieceNum == pieceNum)
            return i;
    }
    return -1;
} //where T = Corner or Edge
int Face::find5EdgePresent(int pieceNum) const { return find5PiecePresent<Edge>(pieceNum); }
int Face::find5CornerPresent(int pieceNum) const { return find5PiecePresent<Corner>(pieceNum); }

//Private/Internal. Simple-Flips (inverts) one Edge-piece and then the other, individually.
void Face::TwoEdgesFlip(int a,int b)
{
    assert(a >= 0 && a < 5 && b >= 0 && b < 5);
    edge[a]->flip();
    edge[b]->flip();
}

//Private/Internal. Functional Generic Switch that flips
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

/** \brief (Public) Given two pieces on the face with local indexes 0-5, swap them. */
template <typename T>
void Face::swapPieces(int a, int b)
{
    assert(a >= 0 && a < 5 && b >= 0 && b < 5);
    Piece* pieceA = getFacePiece<T>(a);
    Piece* pieceB = getFacePiece<T>(b);
    pieceA->swapdata(pieceB->data);
} //where T = Corner or Edge
void Face::swapCorners(int a, int b) { swapPieces<Corner>(a, b); }
void Face::swapEdges(int a, int b) { swapPieces<Edge>(a, b); }

//Private. Swap 4 Pieces, given a list of 8 indexes
template <typename T>
void Face::QuadSwapPieces(const int pack[8])
{
    for (int i = 0; i < 8; i+=2) {
        swapPieces<T>(pack[i], pack[i+1]);
    }
} //where T = Corner or Edge
void Face::QuadSwapCorners(const int pack[8]) { QuadSwapPieces<Corner>(pack); }
void Face::QuadSwapEdges(const int pack[8]) { QuadSwapPieces<Edge>(pack); }

/**
 * \brief (Public) Calling this sets a flag for a chain of events in the render loops to rotate.
 * \param direction Turn direction: -1 for Right, +1 for left (seems/is backwards).
 */
void Face::rotate(int direction)
{
    assert(direction == Face::Clockwise || direction == Face::CCW);
    rotating = true;
    turnDir = direction;
}

/**
 * \brief Colorizing function. Orientation is an Intricate series of flips/swaps.
 * \param dir Turn Direction, for each of the 12 faces,
 * NOTE: called from render() in order to get it to switch colors after it rotates.
 */
bool Face::placeParts(int dir)
{
    assert(dir == Face::CCW || dir == Face::CW);
    if (dir == Face::CounterClockwise) { // 1 = CCW = Left Turn = Counter-ClockWise
        switch (thisFaceNum) {
        case 0: //WHITE
            QuadSwapEdges(CCW0E);
            QuadSwapCorners(CCW0C);
            break;
        case 1: //DARK_BLUE
            QuadSwapEdges(CCW1E);
            TwoEdgesFlip(1, 2);
            QuadSwapCorners(CCW1C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
            break;
        case 2: //RED
            QuadSwapEdges(CCW2E);
            TwoEdgesFlip(1, 2);
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
        switch (thisFaceNum) {
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
        case 2: //RED
            QuadSwapEdges(CW2E);
            TwoEdgesFlip(0, 3);
            QuadSwapCorners(CW2C);
            FlipCorners(0, 1, 3, 4, FlipForwards);
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
        case 10: //PINK
            QuadSwapEdges(CW10E);
            TwoEdgesFlip(0, 2);
            QuadSwapCorners(CW10C);
            FlipCorners(0, 2, 3, 4, FlipBackwards);
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
    //Start Rotating
    if (rotating) {
        glPushMatrix();
        angle += turnDir * turnspeed;
        //Slow down to half-speed once its 75% complete
        //  (56/72 is ~77.7% but use 56 because % mod 8 == 0)
        if (angle >= 56 || angle <= -56)
            angle -= turnDir * (turnspeed/2);
        //Rotate axis by angle
        glRotated(angle, axis[0], axis[1], axis[2]);
    }
    else
        angle = 0;

    //Render parts:
    for (const auto c : corner)
        c->render();
    for (const auto e : edge)
        e->render();
    center->render();
    
    //Render normal:
    glColor3dv(data._color[0]);
    glBegin(GL_LINE_LOOP);
    glVertex3d(normalFvec.x,normalFvec.y,normalFvec.z);
    glEnd();

    if (angle) {
        glPopMatrix();
        //Color Black
        glColor3d(0, 0, 0);
        //Draw a black pentagon to block out view from see-thru hollow insides
        makeGLpolygon(_vertex, 1.0 , 5);
    }
    //Done animating, clean up and commit
    // 72 is one fifth of 360 circle
    if (angle >= 72 || angle <= -72) {
        angle = 0;
        rotating = false;
        //returns True if successful
        return placeParts(turnDir);
        //NOTE: ^^ internal structure of pieces is calculated last
    }
    return false;
}
