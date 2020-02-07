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

/**
 * \brief Pre-initialize center with a re-usable list
 * \param c The center to attach
 * \param centerVertexBase array of geometric vertexes
 */
void Face::attachCenter(Center *c, double* centerVertexBase)
{
    center = c;
    memcpy(&_vertex, centerVertexBase, sizeof(_vertex));
}

/**
 * \brief create our Face's Axes (we can re-use the Center axis creation)
 * \param n is the number of this face 0-11
 */
void Face::initAxis(int n)
{
    assert(n >= 0 && n <= 11);
    thisNum = n;    //0-11
    center->createAxis(n, axis);
    for (int i = 0; i < 5; ++i)
        center->createAxis(n, _vertex[i]);
}

/** \brief connect the right matching Edge pieces to the face. and store the list. */
void Face::attachEdgePieces(const Megaminx* megaminx, Edge& edgesPTR)
{
    defaultEdges = megaminx->findPiecesOfFace(thisNum+1, edgesPTR, Megaminx::numEdges);
    for (int i = 0; i < 5; ++i) {
        edge[i] = &edgesPTR + defaultEdges[i];
        assert(edge[i]->data.pieceNum == defaultEdges[i]);
    }
}

/** \brief connect the right matching Corner pieces to the face. and store the list. */
void Face::attachCornerPieces(const Megaminx* megaminx, Corner& cornersPTR)
{
    defaultCorners = megaminx->findPiecesOfFace(thisNum+1, cornersPTR, Megaminx::numCorners);
    for (int i = 0; i < 5; ++i) {
        corner[i] = &cornersPTR + defaultCorners[i];
        assert(corner[i]->data.pieceNum == defaultCorners[i]);
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
* \brief Returns an EXACT ORDER list of pieces on[Face], (either Edge or Corner piece)
* \return List of face's pieces in 1-5 face order
*/
template <typename T>
std::vector<int> Face::findPiecesOrder() const
{
    std::vector<int> pieceOrder;
    //Compile the accurate color solved maybe list.  (maybe rotated)
    for (int i = 0; i < 5; ++i) {
        const auto piece = getFacePiece<T>(i);
        pieceOrder.push_back(piece->data.pieceNum);
    }
    return pieceOrder;
} //where T = Corner or Edge
std::vector<int> Face::findCornersOrder() const { return findPiecesOrder<Corner>(); };
std::vector<int> Face::findEdgesOrder() const { return findPiecesOrder<Edge>(); };

//General: Is this piecenum on this face 1-5?
//Is this 1-30 piecenum on this face 1-5 edges?
//Is this 1-20 piecenum on this face 1-5 corners?
template <typename T>
int Face::find5PieceLoc(int pieceNum) const
{
    for (int i = 0; i < 5; ++i) {
        const auto piece = getFacePiece<T>(i);
        if (piece->data.pieceNum == pieceNum)
            return i;
    }
    return -1;
} //where T = Corner or Edge
int Face::find5EdgeLoc(int pieceNum) const { return find5PieceLoc<Edge>(pieceNum); }
int Face::find5CornerLoc(int pieceNum) const { return find5PieceLoc<Corner>(pieceNum); }

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

/* Public. Given two pieces on the face with local indexes 0-5, swap them. */
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
    for (int i = 0; i < 8; ++i)
        swapPieces<T>(pack[i], pack[i++]);
} //where T = Corner or Edge
void Face::QuadSwapCorners(const int pack[8]) { QuadSwapPieces<Corner>(pack); }
void Face::QuadSwapEdges(const int pack[8]) { QuadSwapPieces<Edge>(pack); }

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
    glPushMatrix();
    //8 is the current rotational turnspeed for turnDir
    constexpr int turnspeed = 72; //16 is fastmode
    if (rotating)
        angle += turnDir * turnspeed;
    //Slow down once its 75% complete (and angle == 56 == 56 % 8 == 0)
    if (rotating && angle >= 56 || angle <= -56)
        angle -= turnDir * (turnspeed/2);
    if (angle)
        glRotated(angle, axis[0], axis[1], axis[2]);
    //Render:
    for (int i = 0; i < 5; ++i) {
        corner[i]->render();
        edge[i]->render();
    }
    center->render();
    glPopMatrix();
    //Draw a black pentagon to block out view from see-thru hollow insides
    glColor3d(0, 0, 0);
    makeGLpentagon(_vertex, 1.0 , GL_POLYGON);
    //Done animating, clean up and commit
    if (angle >= 72 || angle <= -72) {
        angle = 0;
        rotating = false;
        return placeParts(turnDir);
    }
    return false;
}
