#include "megaminx.hpp"
//Shadow.cpp - A virtual verison of the cube that only exists in theory.
//Can be created and destroyed at whim without affecting the main one,
// and State can be cloned back and forth.
//Useful for iterating moves into the future, then throwing them away

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

//Rotate a face on the shadowcube as if it was normal. but
// skips call to .render() and calls .placeParts() directly.
//Place one item on the shadowRotateQueue
void Megaminx::shadowRotate(int num, int dir)
{
    assert(num > 0 && num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    //std::cout << "Rotate Face: # " << num << " : "  << g_colorRGBs[num].name  << "  Dir: " << dir << std::endl;
    num -= 1; //Convert 1-12 Faces into array [0-11]
    shadowRotateQueue.push({ num, dir });
    faces[num].placeParts(dir);
}

//Same as above, take struct as parameter, no need to subtract 1. unused.
void Megaminx::shadowRotate(numdir op)
{
    shadowRotate(op.num, op.dir);
    //TODO: FIX
    assert(op.num > 0 && op.num <= numFaces);
    assert(op.dir == Face::Clockwise || op.dir == Face::CCW);
    shadowRotateQueue.push({ op.num, op.dir });
    faces[op.num].placeParts(op.dir);
    //same as above, but prints out the action to the console.
    std::cout << "Rotate Face: # " << op.num << " : "  << g_colorRGBs[op.num].name  << "  Dir: " << op.dir << "  Algo: # " << op.algo  << std::endl;
}

//Rotate one face by multiple rotations. Converts any out of bound numbers to the most efficient.
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

//Populate the shadowRotateQueue with a whole bulk sequence of numdir vectors
void Megaminx::shadowBulkRotate(std::vector<numdir> bulk) {
    for (auto op : bulk)    //+1 the 0-11 faces
        shadowRotate(op.num + 1, op.dir);
}

//Apply our shadow cube changes we put in the queue <- back to our main cube, one by one
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

//Create megaminx array, from pieces read from a file, and put into shadow array.
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
