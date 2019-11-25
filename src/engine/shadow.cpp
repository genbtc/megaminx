#include "megaminx.h"

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

void Megaminx::shadowRotate(int num, int dir)
{
    assert(num > 0 && num <= numFaces);
    assert(dir == Face::Clockwise || dir == Face::CCW);
    num -= 1; //Convert 1-12 Faces into array [0-11]
    shadowRotateQueue.push({ num, dir });
    faces[num].placeParts(dir);
}

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
        if (offby > 2.5) {
            offby = (offby - 5) * -1;
            defaultDir *= -1;
        }
        for (int j = 0; j < offby; ++j)
            shadowDom->shadowRotate(face, defaultDir);
    }
    return (offby > 0);
}

void Megaminx::updateRotateQueueWithShadow(Megaminx* shadowDom)
{
    size_t numsize = shadowDom->shadowRotateQueue.size();
    if (numsize <= 0) return;
    undoStack.push({ -999, -999 });
    for (int q = 0; q < numsize; ++q) {
        auto op = shadowDom->shadowRotateQueue.front();
        rotateQueue.push(op);
        shadowDom->shadowRotateQueue.pop();
        undoStack.push(op);
    }
    undoStack.push({ 999, 999 });
}
