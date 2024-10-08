#include "megaminx.hpp"
//Shadow.cpp - A virtual verison of the cube that only exists in theory. - Megaminx - @gen 2024
//Can be created and destroyed at whim without affecting the main one,
// and State can be cloned back and forth.
//Useful for iterating moves into the future, then throwing them away

/**
 * \brief Load a new shadow cube up with the old ones Edges
 */
void Megaminx::LoadNewEdgesFromOtherCube(Megaminx* source)
{
    for (int i = 0; i < numEdges; ++i)
        this->edges[i].data = source->edges[i].data;
}

/**
 * \brief Load a new shadow cube up with the old ones Corners
 */
void Megaminx::LoadNewCornersFromOtherCube(Megaminx* source)
{
    for (int i = 0; i < numCorners; ++i)
        this->corners[i].data = source->corners[i].data;
}

/**
 * \brief Rotate a face on the shadowcube
 * \note  Push/Place one item on the shadowRotateQueue
 *        skips call to .render(), and calls .placeParts() directly.
 */
void Megaminx::shadowRotate(int num, int dir)
{
    assert(num > 0 && num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    //if (DEBUGPRINT) std::cout << "Rotate Face: # " << num << " : "  << g_colorRGBs[num].name  << "  Dir: " << dir << std::endl;
    num -= 1; //Convert 1-12 Faces into array [0-11]
    shadowRotateQueue.push({ num, dir });
    faces[num].placeParts(dir);
}

/**
 * \brief Shadow Rotate same as above, take numdir struct as parameter directly
 * \note  Essentially does this shadowRotate(op.num + 1, op.dir); (no need to add one)
 */
void Megaminx::shadowRotateND(numdir op)
{
    assert(op.num > 0 && op.num <= numFaces);
    assert(op.dir == Face::Clockwise || op.dir == Face::CCW);
    shadowRotateQueue.push(op);
    faces[op.num].placeParts(op.dir);
    //if (DEBUGPRINT) std::cout << "Rotate Face: # " << op.num << " : "  << g_colorRGBs[op.num].name  << "  Dir: " << op.dir << "  Algo: # " << op.algo  << std::endl;
}
/**
 * \brief Populate the shadowRotateQueue with a whole bulk sequence of numdir vectors
 */
void Megaminx::shadowBulkRotate(std::vector<numdir> bulk) {
    for (auto op : bulk)
        shadowRotateND(op);
}

/**
 * \brief Rotate one face by multiple rotations. Converts any out of bound numbers to the most efficient.
 */
bool Megaminx::shadowMultiRotate(int face, int &offby)
{
    int defaultDir = Face::CW;
    if (offby < 0) {    //negative reverses direction
        offby *= -1;
        defaultDir *= -1;
    }
    if (offby > 0) {
        //Save moves by never going past 5
        MM5(offby);
        //Saves moves by going the opposite direction:
        if (offby > 2.5) {
            offby = (offby - 5) * -1;
            defaultDir *= -1;
        }
        for (int j = 0; j < offby; ++j)
            shadowRotate(face, defaultDir);
    }
    return (offby > 0);
}

/**
 * \brief Apply all shadow cube changes put in the queue <- back to the main cube, one by one
 */
void Megaminx::updateRotateQueueWithShadow(Megaminx* shadowDom)
{
    const size_t numsize = shadowDom->shadowRotateQueue.size();
    if (numsize <= 0) return;
    undoStack.push({ -999, -999 });
    for (int q = 0; q < numsize; ++q) {
        auto& op = shadowDom->shadowRotateQueue.front();
        rotateQueue.push(op);
        undoStack.push(op);
        shadowDom->shadowRotateQueue.pop();
    }
    undoStack.push({ 999, 999 });
}

/**
 * \brief Create megaminx array, from pieces read from a file, and put into shadow array.
 */
template <typename T>
int Megaminx::createMegaMinxFromShadowVec(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors, Megaminx* shadowDom)
{
    const auto megaminxArray = this->getPieceArray<T>(0);
    const auto shadowArray = shadowDom->getPieceArray<T>(0);
    const auto arrMaxSize = getMaxNumberOfPieces<T>(); // 30 or 20 pieces
    //Re-Arrange Pieces first
    for (int each = 0; each < arrMaxSize; ++each) {
        const auto &pv = readPieces[each];
        megaminxArray[each].data = shadowArray[pv].data;
    }   //Two Loops to avoid clobbering state midway through
    //Re-Flip Colors last
    for (int each = 0; each < readPieceColors.size(); ++each) {
        const auto &cv = readPieceColors[each];
        //Pieces are in the right place but maybe wrong orientation, so Flip the colors:
        while (megaminxArray[each].data.flipStatus != cv)
            megaminxArray[each].flip();
    }
    return 0;
} //where T = Corner or Edge
int Megaminx::LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors, Megaminx* shadowDom) {
    return createMegaMinxFromShadowVec<Corner>(readCorners, readCornerColors, shadowDom);
}
int Megaminx::LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors, Megaminx* shadowDom) {
    return createMegaMinxFromShadowVec<Edge>(readEdges, readEdgeColors, shadowDom);
}
