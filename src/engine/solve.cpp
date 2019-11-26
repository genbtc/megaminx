#include "megaminx.h"


//This is more complex than normal because its the most flexible
void Megaminx::DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5])
{
    int endI = startI + 5;
    std::vector<int> piecesSeenOnTop;
    //populate piecesSolved
    int numSolved = 0;
    //Find out if any applicable pieces are in the desired slots:
    for (int p = startI; p < endI; ++p) {
        int pIndex = findEdge(p);
        //make sure its a match and make sure the colors arent flipped
        if (pIndex >= startI && pIndex < endI && edges[pIndex].data.flipStatus == 0)
            piecesSeenOnTop.push_back(p - startI);
        //If we found the correct piece solved in the correct spot
        if (p == pIndex && edges[pIndex].data.flipStatus == 0) {
            piecesSolved[p - startI] = true;
            numSolved++;
        }
    }
    if (piecesSeenOnTop.size() > 1) {
        //Check if the ordering blue->red,red->green is correct,etc... even if the top is twisted vs the sides
        for (int p = startI; p < endI; ++p) {
            int pNext = (p != (endI - 1)) ? p + 1 : startI; //handle the loop numbering boundary overrun
            int pIndex = findEdge(p);
            int pNextIndex = findEdge(pNext);
            if (((pIndex >= startI && pIndex < endI) && (pNextIndex >= startI && pNextIndex < endI)) &&
                edges[pIndex].data.flipStatus == 0 && edges[pNextIndex].data.flipStatus == 0 &&
                ((pNextIndex == pIndex + 1) || (pIndex == (endI - 1) && pNextIndex == startI))) {
                piecesSolved[p - startI] = true;
                piecesSolved[pNext - startI] = true;
                numSolved++;
            }
        }
    }
    //Fallback to at least get first piece solved
    if ((numSolved == 0 && piecesSeenOnTop.size() > 1) || piecesSeenOnTop.size() == 1) {
        std::sort(piecesSeenOnTop.begin(), piecesSeenOnTop.end());
        piecesSolved[piecesSeenOnTop[0]] = true;
        numSolved++;
    }
}

//Generic template way to detect if pieces are solved in their correct locations with correct colors
template <typename T>
void Megaminx::DetectSolvedPieces(int startI, bool piecesSolved[5])
{
    int endI = startI + 5;
    //Find out if any applicable startI-endI pieces are exactly in their slots.
    for (int p = startI; p < endI; ++p) {
        int pIndex = findPiece<T>(p);
        Piece* piece = getPieceArray<T>(pIndex);
        //make sure its startI-endI and make sure the colors arent flipped
        if (pIndex >= startI && pIndex < endI && p == pIndex && piece->data.flipStatus == 0)
            piecesSolved[p - startI] = true;
    }
} //where T = Corner or Edge
void Megaminx::DetectSolvedCorners(int startI, bool piecesSolved[5]) {
    DetectSolvedPieces<Corner>(startI, &piecesSolved[0]);
}
void Megaminx::DetectSolvedEdges(int startI, bool piecesSolved[5]) {
    DetectSolvedPieces<Edge>(startI, &piecesSolved[0]);
}


class LayerAssist {
public:
    int sourceEdgeIndex;
    colorpiece edgeFaceNeighbors;
    int edgeFaceLocA;
    int edgeFaceLocB;
    int dirToWhiteA;
    int dirToWhiteB;
    Edge* EdgeItselfA;
    Edge* EdgeItselfB;
    int whichcolorEdgeA;
    int whichcolorEdgeB;
    int edgeHalfColorA;
    int edgeHalfColorB;
    bool colormatchA;
    bool colormatchB;
    bool isOnRow1;
    bool isOnRow2;
    bool isOnRow3;
    bool isOnRow4;
    bool isOnRow5;
    bool isOnRow6;
    bool isOnRow7;
    bool ontopA;
    bool ontopB;

    LayerAssist(Megaminx* shadowDom, int i) {
        sourceEdgeIndex = shadowDom->findEdge(i);
        auto currentPiece = shadowDom->getPieceArray<Edge>(sourceEdgeIndex);
        auto currentpieceColor = currentPiece->data._colorNum[0];
        auto currentpieceFlipStatus = currentPiece->data.flipStatus;
        //Determine which two faces the edge belongs to:
        edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, from 0-4
        edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        EdgeItselfB = shadowDom->faces[edgeFaceNeighbors.b - 1].edge[edgeFaceLocB];
        assert(EdgeItselfA == EdgeItselfB); //sanity check.        
        //Determine which direction those faces need to rotate to land the Edge on the white
        dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        //Use reference table to check edge internal color data struct-order.
        whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        assert(whichcolorEdgeA + whichcolorEdgeB == 1); //just makes sure the reference table is accurate, it is.        
        //Determine which color half-edge is on each face
        edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        edgeHalfColorB = EdgeItselfB->data._colorNum[whichcolorEdgeB];
        colormatchA = edgeHalfColorA != WHITE;
        colormatchB = edgeHalfColorB != WHITE;
        assert(colormatchA != colormatchB); //sanity check.
        //Line up things that are solved on the top face.
        isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        isOnRow4 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20);
        isOnRow5 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        //Check if the faces we have are considered as the Top half of the cube.
        ontopA = (edgeFaceNeighbors.a > 1 && edgeFaceNeighbors.a < 7);
        ontopB = (edgeFaceNeighbors.b > 1 && edgeFaceNeighbors.b < 7);
    }
};

//Layer 1 part 1 - White Edges
void Megaminx::rotateSolveWhiteEdges(Megaminx* shadowDom)
{
    int i = 0;
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //temporary overflow protection:
        if (loopcount > 101)
            break;
        bool facesSolved[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
        bool piecesSolved[5] = { false, false, false, false, false };
        int firstSolvedPiece = -1;
        shadowDom->DetectSolvedEdgesUnOrdered(0, piecesSolved);
        for (int a = 0; a < 5; ++a) {
            facesSolved[1 + a] = piecesSolved[a];
            if (firstSolvedPiece == -1 && piecesSolved[a] == true)
                firstSolvedPiece = a;
        }
        while (i < 5 && piecesSolved[i] == true)
            i++;
        if (i >= 5) {
            allSolved = true;
            break;
        }

        //Rotates the white face to its solved position, first solved edge matches up to its face.
        if (firstSolvedPiece != -1) {
            //NOTE: Doing this over and over is wasting moves solving the partial-solved top every time.
            //TODO: This means we need a plan for any 5-9 edge to get moved into any 0-4 slot even when top isn't solved.
            int findIfPieceSolved = shadowDom->findEdge(firstSolvedPiece); //always piece 0
            int offby = findIfPieceSolved - firstSolvedPiece;    //example: piece 0, 5 - 5 - 0 = 0, so no correction.
            if (findIfPieceSolved > 0 && findIfPieceSolved < 5 && offby > 0) {
                offby *= -1;
                shadowMultiRotate(WHITE, offby, shadowDom);
                continue;
            }
        }
        LayerAssist l{ shadowDom,i };
        //Any matching pieces that end up on its matching face can be spun in with just 2 or 1 moves.
        if (l.ontopA && ((l.isOnRow4 && l.colormatchA) || l.isOnRow2)) {
            int offby = l.colormatchA ? (l.edgeFaceNeighbors.a - l.edgeHalfColorA) : (l.edgeFaceNeighbors.b - l.edgeHalfColorB);
            //Set up Rotated White top to be in-line with the face we want to spin in.
            shadowMultiRotate(WHITE, offby, shadowDom);
            if (l.isOnRow4 && l.colormatchA) {
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            }
            else if (l.isOnRow2 && l.colormatchA)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            else if (l.isOnRow2 && l.colormatchB)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        //Locates any straggler pieces on the bottom and bubbles them up to the top layers, as long as the face isnt protected by facesSolved pieces
        else if (l.isOnRow4 && l.dirToWhiteA != 0 && ((l.edgeFaceNeighbors.a < GRAY && !facesSolved[l.edgeFaceNeighbors.a - 1]) || l.edgeFaceNeighbors.a >= GRAY)) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        }
        else if (l.isOnRow4 && l.dirToWhiteB != 0 && ((l.edgeFaceNeighbors.b < GRAY && !facesSolved[l.edgeFaceNeighbors.b - 1]) || l.edgeFaceNeighbors.b >= GRAY)) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
            //REFRESH
            int sourceEdgeIndexNext = shadowDom->findEdge(i);
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == l.edgeFaceNeighbors.b)
                //check if the next neighbor face is Solved aka blocked ?
                if (facesSolved[edgeFaceNeighborsNext.a - 1])
                    //make sure its the move going to the right
                    if (edgeFaceNeighborsNext.a == g_faceNeighbors[l.edgeFaceNeighbors.a].right)
                        shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        else if (l.isOnRow6 || l.isOnRow7 && l.dirToWhiteA != 0)
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        else if (l.isOnRow6 || l.isOnRow7 && l.dirToWhiteB != 0)
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        //These can still trigger if the top pieces are in the top row but the wrong slot (or are color-flipped)
        else if (l.isOnRow1 && l.dirToWhiteA != 0 && !facesSolved[l.edgeFaceNeighbors.a - 1])
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        else if (l.isOnRow1 && l.dirToWhiteB != 0 && !facesSolved[l.edgeFaceNeighbors.b - 1])
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //If its solved, get top white face spun oriented back to normal
    if (allSolved) {
        int findIfPieceSolved = shadowDom->findEdge(0); //always piece 0
        if (findIfPieceSolved > 0 && findIfPieceSolved < 5) {
            findIfPieceSolved *= -1;
            shadowMultiRotate(WHITE, findIfPieceSolved, shadowDom);
        }
    }
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 1 part 2 - White Corners
void Megaminx::rotateSolveWhiteCorners(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //temporary overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedCorners(0, piecesSolved);
        int i = 0; //the starting piece
        while (i < 5 && piecesSolved[i] == true)
            i++;
        if (i >= 5) {
            allSolved = true;
            continue;
        }
        int sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        colorpiece cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the corners are positioned, 0-4
        int cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        int cornerFaceLocB = shadowDom->faces[cornerFaceNeighbors.b - 1].find5CornerLoc(i);
        assert(cornerFaceLocB != -1); //should not happen
        int cornerFaceLocC = shadowDom->faces[cornerFaceNeighbors.c - 1].find5CornerLoc(i);
        assert(cornerFaceLocC != -1); //should not happen
        Corner* CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceCornerIndex >= 0 && sourceCornerIndex < 5);
        bool isOnRow2 = (sourceCornerIndex >= 5 && sourceCornerIndex < 10);
        bool isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        bool isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        //New breakthrough idea, any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        bool ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        bool ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        bool ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);
        //Rotates the white face to its solved position, first solved EDGE matches up to its face.
        //Edges should already be solved, if not, get top white face spun oriented back to normal
        int edgesOffBy = shadowDom->findEdge(0); //always piece 0
        if (edgesOffBy > 0 && edgesOffBy < 5) {
            edgesOffBy *= -1;
            shadowMultiRotate(WHITE, edgesOffBy, shadowDom);
        }
        //Move incorrect corners out of the 0-4 slots moves them right down with the algo
        else if (isOnRow1 && ((i != sourceCornerIndex) || (i == sourceCornerIndex && CornerItselfA->data.flipStatus != 0)) && piecesSolved[i] == false) {
            int offby = RED;
            if (cornerFaceNeighbors.a == WHITE)
                offby += cornerFaceLocA;
            else if (cornerFaceNeighbors.b == WHITE)
                offby += cornerFaceLocB;
            else if (cornerFaceNeighbors.c == WHITE)
                offby += cornerFaceLocC;
            if (offby > 6)
                offby -= 5;
            if (offby >= 2) {
                const colordirs &loc = g_faceNeighbors[offby];
                std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("R' DR' R DR", loc);
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num + 1, op.dir);
            }
        }
        //Move correct corners straight up and in from 5-9 to 0-4
        else if (isOnRow2 && sourceCornerIndex - i - 5 == 0) {
            int offby = sourceCornerIndex - 5 + RED;
            if (offby > 6)
                offby -= 5;
            if (offby >= 2) {
                const colordirs &loc = g_faceNeighbors[offby];
                std::vector<numdir> bulk;
                if (CornerItselfA->data.flipStatus == 2)
                    bulk = shadowDom->ParseAlgorithmString("f dr F'", loc);
                else
                    bulk = shadowDom->ParseAlgorithmString("R' DR' R", loc);
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num + 1, op.dir);
            }
        }
        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        else if (isOnRow2) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 5;
            megaminxColor turnface;
            //2nd rotation puts them on the gray face.
            if (ontopA && ontopB)
                turnface = cornerFaceNeighbors.c;
            else if (ontopA && ontopC)
                turnface = cornerFaceNeighbors.b;
            else if (ontopB && ontopC)
                turnface = cornerFaceNeighbors.a;
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
        }
        //Row 3 pieces go to gray face as temporary holding (1 CW turn) (ends up on row4)
        else if (isOnRow3) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 10;
            int x, y;
            if (ontopA) {
                x = cornerFaceNeighbors.b;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopB) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopC) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.b;
            }
            int result = max(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = LIGHT_BLUE;
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2
        else if (isOnRow4) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - i - 18;
            shadowMultiRotate(GRAY, offby, shadowDom);
            int x = PINK + (i);
            if (x > BEIGE)
                x -= 5;
            shadowDom->shadowRotate(x, defaultDir);
            shadowDom->shadowRotate(x, defaultDir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 2 - Edges
void Megaminx::rotateSolveLayer2Edges(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedEdges(5, piecesSolved);
        int i = 5; //the starting piece
        while (i < 10 && piecesSolved[i - 5] == true)
            i++;
        if (i >= 10) {
            allSolved = true;
            break;
        }
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        //Mark where edge-halves have neighbor faces belonging to the top-half of cube.
        bool ontopA = (edgeFaceNeighbors.a > 1 && edgeFaceNeighbors.a < 7);
        bool ontopB = (edgeFaceNeighbors.b > 1 && edgeFaceNeighbors.b < 7);
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        bool isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        bool isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        bool isOnRow34 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20); //Middle W
        bool isOnRow4 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);

        if ((isOnRow2 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow34 && ontopA && edgeFaceLocA == 4 && edgeFaceNeighbors.a == edgeHalfColorA)) {
            const colordirs &loc = g_faceNeighbors[edgeFaceNeighbors.a];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("dl l dl l' dl' f' dl' f", loc); // left
            for (auto op : bulk)    //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else if (isOnRow34 && ontopA && edgeFaceLocA == 3 && edgeFaceNeighbors.a == edgeHalfColorA) {
            const colordirs &loc = g_faceNeighbors[edgeFaceNeighbors.a];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("dr' r' dr' r dr f dr f'", loc); // right
            for (auto op : bulk)    //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        //BUG?: Pieces still go the long-way around from 7->6->3->(skip)4->6->7->6->4 then algo....
        //TODO: Need to detect when the face matches up then go backwards.
        else if (isOnRow34 && dirToWhiteB != 0 && edgeFaceNeighbors.b >= GRAY) {
            shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
            int sourceEdgeIndexNext = shadowDom->findEdge(i);   //REFRESH
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == edgeFaceNeighbors.b)
                //check the solved for the next neighbor (vs what if we just check the original, face.loc.right) ?
                //make sure its the move going to the right
                if (edgeFaceNeighborsNext.a == g_faceNeighbors[edgeFaceNeighbors.a].right)
                    shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
        }
        //Layer 6 pieces are moved down to gray layer 7
        else if (isOnRow6) {
            if (dirToWhiteA != 0)
                shadowDom->shadowRotate(edgeFaceNeighbors.a, -1 * dirToWhiteA);
            else if (dirToWhiteB != 0)
                shadowDom->shadowRotate(edgeFaceNeighbors.b, -1 * dirToWhiteB);
        }
        //Layer 7 is an intermediate buffer
        else if (isOnRow7) {
            int row7 = sourceEdgeIndex - 28 - i;
            bool moved = shadowMultiRotate(GRAY, row7, shadowDom);
            //Align the GRAY layer 7 to be directly underneath the intended solve area
            if (moved == false) {
                if (dirToWhiteA != 0) {
                    shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
                    shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
                }
                else if (dirToWhiteB != 0) {
                    shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
                    shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
                }
            }
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 3 - Corners
void Megaminx::rotateSolve3rdLayerCorners(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedCorners(5, piecesSolved);
        int i = 5; //the starting piece
        while (i < 10 && piecesSolved[i - 5] == true)
            i++;
        if (i >= 10) {
            allSolved = true;
            break;
        }
        int sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        colorpiece cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the corners are positioned, 0-4
        int cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        Corner* CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceCornerIndex >= 0 && sourceCornerIndex < 5);
        bool isOnRow2 = (sourceCornerIndex >= 5 && sourceCornerIndex < 10);
        bool isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        bool isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        //New breakthrough idea, any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        bool ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        bool ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        bool ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);

        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        if (isOnRow2) {
            int defaultDir = Face::CW;
            int offby = sourceCornerIndex - i - 5;
            megaminxColor turnface;
            //2nd rotation puts them on the gray face.
            if (ontopA && ontopB)
                turnface = cornerFaceNeighbors.c;
            else if (ontopA && ontopC)
                turnface = cornerFaceNeighbors.b;
            else if (ontopB && ontopC)
                turnface = cornerFaceNeighbors.a;
            //If the piece is solved positioned, but color flipped wrong:
            if (sourceCornerIndex == i)
                defaultDir *= -1;
            // turn opposite direction row2->row3->row4 to use gray face then repeat
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
        }
        //Row 3 pieces go to gray face as temporary holding (1 CCW turn) (ends up on row4)
        else if (isOnRow3) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - i - 10;
            int x, y;
            if (ontopA) {
                x = cornerFaceNeighbors.b;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopB) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopC) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.b;
            }
            //The loop point crosses over at the Pink 
            if (x < PINK)
                x += 5;
            if (y < PINK)
                y += 5;
            int result = max(x, y);
            if (result > BEIGE)
                result -= 5;
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2 Solved
        else if (isOnRow4) {
            int defaultDir = Face::CCW;
            int offby = sourceCornerIndex - (i - 5) - 18;
            shadowMultiRotate(GRAY, offby, shadowDom);
            int x = PINK + (i - 5);
            if (x > BEIGE)
                x -= 5;
            shadowDom->shadowRotate(x, defaultDir);
            shadowDom->shadowRotate(x, defaultDir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 4 - Edges
//Cube must have gray side on top, layer 1+2+3 Solved and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer4Edges(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[10] = { false, false, false, false, false, false, false, false, false, false };
        shadowDom->DetectSolvedEdges(10, piecesSolved);
        shadowDom->DetectSolvedEdges(15, &piecesSolved[5]);
        int i = 10; //the piece
        while (i < 20 && piecesSolved[i - 10] == true)
            i++;
        if (i >= 20) {
            allSolved = true;
            break;
        }
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        int edgeHalfColorB = EdgeItselfA->data._colorNum[whichcolorEdgeB];
        //Line up things that are solved on the top face.
        bool isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        bool isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        bool isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        bool isOnRow34 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20); //Middle W
        bool isOnRow4 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        bool graymatchA = edgeFaceNeighbors.a == GRAY;
        bool graymatchB = edgeFaceNeighbors.b == GRAY;
        //drop-in directions: left is row 4, right is row 3 (determined from original I, not sourceEdgeIndex)
        bool isRight = (i >= 10 && i < 15);
        bool isLeft = (i >= 15 && i < 20);
        //Get ready for algorithms
        if ((isOnRow34 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow7)) {
            if (isOnRow7) {
                //Align GRAY top to the exact position for pre-drop-in.
                int offby = sourceEdgeIndex - 27 - i;
                if (isLeft) //row 4B's B-half is +1 from row4A's B-half (DARK_BLUE,LIGHT_GREEN) vs (DARK_BLUE,PINK)
                    offby -= 1;
                bool moved = shadowMultiRotate(GRAY, offby, shadowDom);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
            }
            //obtain the non-gray face neighbor we need to be rotating
            colordirs loc;
            if (isOnRow7)
                loc = (graymatchA) ? g_faceNeighbors[edgeFaceNeighbors.b] : g_faceNeighbors[edgeFaceNeighbors.a];
            else if (isOnRow34)
                loc = g_faceNeighbors[max(edgeFaceNeighbors.b, edgeFaceNeighbors.a)];
            //works to insert pieces from row7 to 3/4 and also pops wrong pieces out from 3/4 to 6
            std::vector<numdir> bulk;
            if ((isOnRow7 && isLeft) || (isOnRow4)) {
                if (EdgeItselfA->data.flipStatus == 0 || (isOnRow4))
                    bulk = shadowDom->ParseAlgorithmString("F' R', F' F', r f", loc);  // left
                else
                    bulk = shadowDom->ParseAlgorithmString("U' R' DR' F F DR R", loc); //l+inverted
            }
            else if ((isOnRow7 && isRight) || (isOnRow3)) {
                if (EdgeItselfA->data.flipStatus == 0 || (isOnRow3))
                    bulk = shadowDom->ParseAlgorithmString("f l, f f, L' F'", loc);    // right
                else
                    bulk = shadowDom->ParseAlgorithmString("u l dl F' F' DL' L'", loc); //r+inverted
            }
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        //Row 6 - flip piece up to GRAY, then return the moved faces to unharm the low corners.
        else if (isOnRow6) {
            if (dirToWhiteA != 0) {
                shadowDom->shadowRotate(edgeFaceNeighbors.a, -1 * dirToWhiteA);
                shadowDom->shadowRotate(GRAY, dirToWhiteA);
                shadowDom->shadowRotate(edgeFaceNeighbors.a, dirToWhiteA);
            }
            else if (dirToWhiteB != 0) {
                shadowDom->shadowRotate(edgeFaceNeighbors.b, -1 * dirToWhiteB);
                shadowDom->shadowRotate(GRAY, dirToWhiteB);
                shadowDom->shadowRotate(edgeFaceNeighbors.b, dirToWhiteB);
            }
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 5 - Corners
void Megaminx::rotateSolve5thLayerCorners(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedCorners(10, piecesSolved);
        int i = 10; //the starting piece
        while (i < 15 && piecesSolved[i - 10] == true)
            i++;
        if (i >= 15) {
            allSolved = true;
            break;
        }
        int sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        colorpiece cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Determine where on those faces the corners are positioned, 0-4
        int cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        Corner* CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        bool isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        bool isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        bool ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        bool ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        bool ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);
        //Get the Pieces to drop-in ready on Row 4 (gray layer) solved into Row3
        //Any Row 3 pieces that are mis-solved use same algo to go up to gray layer (ends up on row4)
        if (isOnRow3) {
            //int defaultDir = Face::CCW;
            //int offby = sourceCornerIndex - i;
            int x, y;
            if (ontopA) {
                x = cornerFaceNeighbors.b;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopB) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.c;
            }
            else if (ontopC) {
                x = cornerFaceNeighbors.a;
                y = cornerFaceNeighbors.b;
            }
            int result = min(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = BEIGE;
            colordirs loc = g_faceNeighbors[result];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("r u R' U'", loc);  //right
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else if (isOnRow4) {
            //Orient Gray Top layer (index goes in reverse)
            int offby = sourceCornerIndex + (i - 10) - 20;
            shadowMultiRotate(GRAY, offby, shadowDom);
            //quick shortcut to know which face we're working on.
            int front = BEIGE - (i - 10);
            colordirs loc = g_faceNeighbors[front];
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString("r u R' U'", loc);  //right
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 6 - Edges
//Cube must have gray side on top, layer 1+2+3+4+5 Solved, and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer6Edges(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false, false, false, false, false };
        shadowDom->DetectSolvedEdges(20, piecesSolved);
        int i = 20; //the starting piece
        while (i < 25 && piecesSolved[i - 20] == true)
            i++;
        if (i >= 25) {
            allSolved = true;
            break;
        }
        int sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        colorpiece edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, 0-4
        int edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgeLoc(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        int edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgeLoc(i);
        assert(edgeFaceLocB != -1); //should not happen
        //Determine which direction those faces need to rotate to land the Edge on the white
        int dirToWhiteA = DirToWhiteFace[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int dirToWhiteB = DirToWhiteFace[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        Edge* EdgeItselfA = shadowDom->faces[edgeFaceNeighbors.a - 1].edge[edgeFaceLocA];
        //Use reference table to check edge internal color data struct-order.
        int whichcolorEdgeA = BlackEdgesNumber2[edgeFaceNeighbors.a - 1][edgeFaceLocA];
        int whichcolorEdgeB = BlackEdgesNumber2[edgeFaceNeighbors.b - 1][edgeFaceLocB];
        //Determine which color half-edge is on each face
        int edgeHalfColorA = EdgeItselfA->data._colorNum[whichcolorEdgeA];
        int edgeHalfColorB = EdgeItselfA->data._colorNum[whichcolorEdgeB];
        //Only care about row6 and 7
        bool isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        bool isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        //on Row7, One of these will be gray, so the other letter is the colored face target.
        bool graymatchA = edgeFaceNeighbors.a == GRAY;
        bool graymatchB = edgeFaceNeighbors.b == GRAY;
        //Get ready for algorithms
        if ((isOnRow6 && (sourceEdgeIndex != i || (sourceEdgeIndex == i && EdgeItselfA->data.flipStatus != 0))) ||
            (isOnRow7)) {
            if (isOnRow7) {
                assert(dirToWhiteA == 0);
                int offby = graymatchA ? (edgeFaceNeighbors.b - edgeHalfColorB) : (edgeFaceNeighbors.a - edgeHalfColorA);
                //Align GRAY top to the exact position for pre-drop-in.
                bool moved = shadowMultiRotate(GRAY, offby, shadowDom);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
            }
            //obtain the non-gray face neighbor we need to be oriented to
            int result = 0;
            int x = edgeFaceNeighbors.a;
            int y = edgeFaceNeighbors.b;
            if (isOnRow7)
                result = (graymatchA) ? y : x;
            else if (isOnRow6) {
                result = min(x, y);
                if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                    result = BEIGE;
            }
            colordirs loc = g_faceNeighbors[result]; //loc.front
            //Check left/right faces for which direction to drop-in
            bool isLeft = isOnRow7 && (loc.left == edgeHalfColorA);
            bool isRight = isOnRow6 || isOnRow7 && (loc.right == edgeHalfColorA);
            //works to insert pieces from row7 to 6 and also pops wrong pieces out from 6 to 7
            std::vector<numdir> bulk;
            if (isLeft)
                bulk = shadowDom->ParseAlgorithmString("U' L' u l u f U' F'", loc); //left
            else if (isRight)
                bulk = shadowDom->ParseAlgorithmString("u r U' R' U' F' u f", loc); //right
            for (auto op : bulk)        //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Last Layer (a Work in Progress)
//Layer 7 - Last Layer Edges (GRAY top)
//Cube must have gray side on top, layer 1+2+3+4+5+6 Solved
void Megaminx::rotateSolveLayer7Edges(Megaminx* shadowDom)
{
    bool allSolved = false;
    //Loop management:
    int loopcount = 0;
    int unknownloop = 0;
    do {
        if (loopcount > 101)
            break;
        int i = 25; //the starting piece
        //Stores piece and color status from the gray face, as detected by the functions
        bool piecesSolvedMaybe[5] = { false, false, false, false, false };
        bool piecesSolvedStrict[5] = { false, false, false, false, false };
        bool colorsSolvedMaybe[5] = { false, false, false, false, false };
        bool colorsSolvedStrict[5] = { false, false, false, false, false };        
        shadowDom->DetectSolvedEdgesUnOrdered(i, &piecesSolvedMaybe[0]);
        shadowDom->DetectSolvedEdges(i, &piecesSolvedStrict[0]);
        //Iterate past any already completely solved pieces in order.
        while (i < 30 && piecesSolvedStrict[i - 25] == true)
            i++;
        //Either we are completely solved ? 
        if (i >= 30) {
            allSolved = true;
            break;
        }
        //Or have an unsolved piece to compute.
        //LayerAssist l{ shadowDom,i };
        //Asks some additional questions about what pieces/colors are concrete viable options
        int solvedCount = 0;
        int firstSolvedPiece = -1;
        int firstUnsolvedPiece = -1;
        for (int k = 0; k < 5; ++k) {
            if (piecesSolvedStrict[k]) {
                solvedCount++;
                colorsSolvedStrict[k] = true;
            }
            if (piecesSolvedMaybe[k] && firstSolvedPiece == -1)
                firstSolvedPiece = k;
            if (!piecesSolvedMaybe[k] && firstUnsolvedPiece == -1)
                firstUnsolvedPiece = k;
        }
        //Compile the accurate color solved maybe list.  (if rotated)
        for (int k = 0; k < 5; ++k) {
            Edge* EdgeItselfNext = shadowDom->faces[GRAY - 1].edge[k];
            if (EdgeItselfNext->data.flipStatus == 0)
                colorsSolvedMaybe[k] = true;
        }
        //iterate through the piece/color lists and make some easy compound conditional aliases
        bool allColorsSolvedMaybe = colorsSolvedMaybe[0] && colorsSolvedMaybe[1] && colorsSolvedMaybe[2] && colorsSolvedMaybe[3] && colorsSolvedMaybe[4];
        bool twoAdjacentPieces = (  //test2-p3p4OK-rotate3a+ PASS
            (piecesSolvedStrict[0] && piecesSolvedStrict[1]) ||
            (piecesSolvedStrict[1] && piecesSolvedStrict[2]) ||
            (piecesSolvedStrict[2] && piecesSolvedStrict[3]) ||
            (piecesSolvedStrict[3] && piecesSolvedStrict[4]) ||
            (piecesSolvedStrict[4] && piecesSolvedStrict[0]));
        //Set up main loop, we refer to these a lot.
        std::vector<numdir> bulk;
        int sourceEdgeIndex = shadowDom->findEdge(i);
        int offby = sourceEdgeIndex - i;
        auto currentPiece = shadowDom->getPieceArray<Edge>(sourceEdgeIndex);
        auto currentpieceColor = currentPiece->data._colorNum[0];
        auto currentpieceFlipStatus = currentPiece->data.flipStatus;
        //START MAIN
        //TODO:         //conditions need to be written variadicly for solvedcount/piecesSolved + offby, as we rotate the gray top.
        //auto e = findPieces<Edge>(GRAY);    //this is useless - just returns 25,26,27,28,29 always.
        //orient the first piece if it exists:
        int findIfPieceSolved = shadowDom->findEdge(25); //always piece 0
        if (findIfPieceSolved > 25 && findIfPieceSolved < 30 && currentpieceFlipStatus == 0 && !piecesSolvedStrict[0] && offby != 0 && !twoAdjacentPieces) {
            //Test 3 passes.
            int offby = findIfPieceSolved - 25;
            shadowMultiRotate(GRAY, offby, shadowDom);
            //break;
        }
        //Rotates the GRAY face to any solved position, first out of order but solved EDGE rotates to match up to its face.
        else if (!piecesSolvedStrict[0] && firstSolvedPiece != -1 && piecesSolvedMaybe[i - 25] && solvedCount >= (i - 25)) {
            int findIfPieceSolved = shadowDom->findEdge(i + firstSolvedPiece);
            int offby = findIfPieceSolved - i + firstSolvedPiece;
            if (findIfPieceSolved >= 25 && findIfPieceSolved < 30 && offby != 0) {
                shadowMultiRotate(GRAY, offby, shadowDom);
                //break;
            }
            //break;        
        }
        else if (offby == 1 && solvedCount == 1 && piecesSolvedStrict[0] && allColorsSolvedMaybe)   //
        {
            colordirs loc = g_faceNeighbors[LIGHT_BLUE];
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[19].algo, loc);    //algo 3-#205 SWAP pieces 2/3 & 4/5 (keep color)
        }
        else if ((offby == 2 && piecesSolvedStrict[2]) || (offby == 1 && piecesSolvedStrict[3]) && solvedCount == 2 && piecesSolvedStrict[0] && allColorsSolvedMaybe)  //testCube3-p2 ->(blue/green)=PASS
        {                                                       //TestCube10-2
            colordirs loc;
            if (piecesSolvedStrict[2])
                loc = g_faceNeighbors[LIGHT_BLUE];
            if (piecesSolvedStrict[3])
                loc = g_faceNeighbors[PINK];
            for (int i = 0; i < 5; ++i) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo,loc);    //algo 3-#2(CW) //opposites faces at 6' and 1' must be solved 
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num + 1, op.dir);
            }
            continue;
        }
        else if (offby == 3 && solvedCount == 2 && piecesSolvedStrict[0] && (piecesSolvedStrict[2] || piecesSolvedStrict[3]) && allColorsSolvedMaybe)  //test6-p2 ->(blue/green)=PASS
        {                                                                                           //RandomTestCube1-3
            colordirs loc;
            if (piecesSolvedStrict[2])
                loc = g_faceNeighbors[LIGHT_BLUE];
            if (piecesSolvedStrict[3])
                loc = g_faceNeighbors[PINK];
            for (int i = 0; i < 5; ++i) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, loc);    //algo 3-#1(CC/W) //opposites faces at 6' and 1' must be solved 
                for (auto op : bulk)    //+1 the 0-11 faces
                    shadowDom->shadowRotate(op.num + 1, op.dir);
            }
            continue;
        }
        else if (offby == 1 && solvedCount == 2 && piecesSolvedStrict[4] && piecesSolvedStrict[0] && allColorsSolvedMaybe)  //Test2-p4-Pass ->(beige/blue)=PASS
        {
            colordirs loc = g_faceNeighbors[PINK];          //algo #13 3c- (backwards)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[16].algo, loc);
        }
        else if (offby == 1 && solvedCount == 2 && piecesSolvedStrict[0] && piecesSolvedStrict[1] && (allColorsSolvedMaybe || (!colorsSolvedMaybe[2] && !colorsSolvedMaybe[3])))  //test4-p2 ->(blue/orange)=PASS
        {                                                                                                                           //TestCube6-2
            colordirs loc = g_faceNeighbors[BEIGE];         //algo #13 3c- (backwards)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[16].algo, loc);
        }
        else if (offby == 3 && solvedCount == 2 && piecesSolvedStrict[1] && piecesSolvedStrict[2])  //test3-pt2-manual->(orange/green)=PASS
        {
            colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //algo #13 3c- (backwards) 
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[16].algo, loc);
        }
        else if (!colorsSolvedMaybe[1] && !colorsSolvedMaybe[3]) {
            colordirs loc = g_faceNeighbors[BEIGE];         //algo #206 //INVERT color: has to be on 8' & 1' oclock relative to face.
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, loc);
        }
        else if (!colorsSolvedMaybe[1] && !colorsSolvedMaybe[4]) {
            colordirs loc = g_faceNeighbors[LIGHT_GREEN];    //algo #206 //INVERT color: has to be on 8' & 1' oclock relative to face.
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, loc);
        }
        else if (!colorsSolvedMaybe[2] && !colorsSolvedMaybe[4]) {
            colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //algo #206 //INVERT color: has to be on 8' & 1' oclock relative to face.
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, loc);
        }
        else if ((offby == 0) && !colorsSolvedStrict[i - 25] && !allColorsSolvedMaybe &&
            (solvedCount >= 2 && piecesSolvedStrict[0] && !piecesSolvedStrict[1]) &&
            (!colorsSolvedMaybe[1] && !colorsSolvedMaybe[3]) //test4+manual.
            ) {
            colordirs loc = g_faceNeighbors[BEIGE];         //algo #206  //INVERT color: has to be on 8' & 1' oclock relative to face.
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, loc);
        }
        else if ((offby == 1) && !colorsSolvedStrict[i - 25] && !allColorsSolvedMaybe &&
            (solvedCount >= 1 && piecesSolvedStrict[0] && !piecesSolvedStrict[1])
            ) {
            colordirs loc = g_faceNeighbors[BEIGE];         //algo #206  //INVERT color: has to be on 8' & 1' oclock relative to face.
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, loc);
        }
        else if (
            //Piece 0 is colored wrong, check offset, reverse it to the 1 oclock face. 
            (i == 25 && !colorsSolvedStrict[0])
            ) {
            offby -= 2;
            if (!colorsSolvedMaybe[1] && !colorsSolvedMaybe[4])
                offby = 2;
            shadowMultiRotate(GRAY, offby, shadowDom);
            colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //algo #206 //INVERT color: has to be on 8' & 1' oclock relative to face.
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, loc);
        }
        //first and second piece solved for sure. (attached)
        else if ((offby == 2) && (
            (solvedCount == 1 && piecesSolvedStrict[0] && !piecesSolvedStrict[1]) ||
            (solvedCount == 2 && piecesSolvedStrict[0] && piecesSolvedStrict[1]) ||
            (solvedCount == 2 && piecesSolvedStrict[1] && piecesSolvedStrict[2]) ||
            (solvedCount == 2 && piecesSolvedStrict[3] && piecesSolvedStrict[4]) ||
            (solvedCount >= 2 && piecesSolvedStrict[4] && piecesSolvedStrict[0]
                && ((!colorsSolvedMaybe[2] && !colorsSolvedMaybe[3]) || allColorsSolvedMaybe)))  //test5
            ) {
            if (solvedCount == 2 && piecesSolvedStrict[3] && piecesSolvedStrict[4])
                solvedCount += 3;   //Test2-pt2-pass
            if (solvedCount >= 2 && piecesSolvedStrict[4] && piecesSolvedStrict[0] &&           //test5
                ((!colorsSolvedMaybe[2] && !colorsSolvedMaybe[3]) || allColorsSolvedMaybe))
                solvedCount = 1;   //Test4-fixns
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + solvedCount - 1];    //algo 3a+
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, loc);
        }
        else
            break;
        //DO IT:
        for (auto op : bulk)    //+1 the 0-11 faces
            shadowDom->shadowRotate(op.num + 1, op.dir);
        //EXIT FAST:
        //break;

        //PSEUDOCODE
        /*----------
        Last Layer includes both sides of the edge and all corners.
        EDGES = (phase one):
        Start finding out which Edges are OK
        OK means multiple correct pieces in order, or  1 piece's color facing correctly (gray),
        Does it Need to be rotated? must be considered...
        if 0/5, find how many colors are gray,
        If solved 1/5, find out which piece and try to solve the adjacent piece to it next.
        If solved 2/5 find out if #2 is attached? NO: If not, why? are they 3 away? Choose a three-way-opposite Algo
                                                   YES: Choose a three-ina-row Edge+/Corner rotate algo, ie: algo #11
        If solved >=3/5 find out if #3 is attached?: NO: IDK
                                                      YES: ie: Algo #206 to invert the other 2 corners
        Can go from 2/5 to all 5/5 at once OR go from 3/5 to 5/5.

        CORNERS = (phase two):
        Corners must keep the edges in place, we will know solved rotation by having the offby=0 as a query to solved findEdges above.
        If solved 1/5, determine if better to skip and start at 0 or continue with this.
         1pt2: check next piece, find out if its capable of being moved in 1 move, or if its 2 moves.
        If solved 2/5, find out if they are attached: NO: If not, why? are they 3 away? we have no three-way-opposite algo, must give up, go back to 1.
                                                     YES: Choose right three-in-a-row corner-only algo
        If solved 3/5, find out if they are attached: NO: If not, proceed to next.
                                                     YES: Choose right three-way corner-only algo
        */
        loopcount++;
    } while (!allSolved);

    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

void Megaminx::testingAlgostrings(Megaminx* shadowDom)
{
    if (!shadowDom) {
        shadowDom = new Megaminx();
        //needs the this* when called without first initializing with FromCubeToShadowCube().
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    //for (; ;)   //call one at a time.
    int i = 12;
    {
        auto a = g_AlgoStrings[i];
        colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //front-face (adjustable)
        int repeat = (a.repeatX != NULL) ? a.repeatX : 1;
        for (int n = 0; n < repeat; ++n) {
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString(a.algo, loc);
            for (auto op : bulk)    //+1 the 0-11 faces
                shadowDom->shadowRotate(op.num + 1, op.dir);
        }
        //find where pieces actually are vs. where they're supposed to be
        std::vector<int> foundEdges;
        auto defaultEdges = faces[GRAY - 1].defaultEdges;
        for (int k = 0; k < 5; ++k) {
            const auto piece = shadowDom->faces[GRAY - 1].getFacePiece<Edge>(k);
            foundEdges.push_back(piece->data.pieceNum);
        }
        //output the difference as an int.
        std::vector<int> newDifference;
        for (int k = 0; k < 5; ++k) {
            newDifference.push_back(foundEdges[k] - defaultEdges[k]);
        }
        int printfbreakpoint = 0;   //break on this and inspect.
    }
    updateRotateQueueWithShadow(shadowDom); //actually output the algo's changes to main cube's GUI
}
