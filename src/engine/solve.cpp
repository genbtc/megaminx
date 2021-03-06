#include "megaminx.h"


//the most flexible
void Megaminx::DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5])
{
    std::vector<int> piecesSeenOnTop;
    //populate piecesSolved
    int numSolved = 0;
    int endI = startI + 5;
    //Check if any pieces are already in their assigned slots:
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

//call to find out if the puzzle is fully solved.
bool Megaminx::isFullySolved()
{
    return (DetectIfAllSolved<Edge>() && DetectIfAllSolved<Corner>());
}

//fast way to detect if the puzzle is solved
template <typename T>
bool Megaminx::DetectIfAllSolved()
{
    int allSolved = 0;
    Piece* piece = getPieceArray<T>(0);
    int numpieces = getMaxNumberOfPieces<T>();
    for (int i = 0; i < numpieces; ++i) {
        if (piece[i].data.pieceNum == i && piece[i].data.flipStatus == 0)
            allSolved++;
    }
    return (numpieces==allSolved);
}

//Generic template way to detect if pieces are solved, in their correct locations with correct colors, on one face
template <typename T>
void Megaminx::DetectSolvedPieces(int startI, bool piecesSolved[5])
{
    int endI = startI + 5;
    //Find out if any applicable startI-endI pieces are exactly in their slots.
    for (int p = startI; p < endI; ++p) {
        int pIndex = findPiece<T>(p);
        const Piece* piece = getPieceArray<T>(pIndex);
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


class EdgeLayerAssist {
public:
    int sourceEdgeIndex;
    colorpiece edgeFaceNeighbors;
    int edgeFaceLocA, edgeFaceLocB;
    int dirToWhiteA, dirToWhiteB;
    Edge *EdgeItselfA, *EdgeItselfB;
    int whichcolorEdgeA, whichcolorEdgeB;
    int edgeHalfColorA, edgeHalfColorB;
    bool colormatchA, colormatchB;
    bool isOnRow1, isOnRow2, isOnRow3, isOnRow34;
    bool isOnRow4, isOnRow5, isOnRow6, isOnRow7;
    bool ontopA, ontopB;
    bool graymatchA, graymatchB;
    bool isRight, isLeft;
    Piece* currentPiece;
    bool currentpieceFlipStatusOK;
    Megaminx* shadowDom;

    EdgeLayerAssist(Megaminx* shadowDom, int i) {
        sourceEdgeIndex = shadowDom->findEdge(i);
        currentPiece = shadowDom->getPieceArray<Edge>(sourceEdgeIndex);
        currentpieceFlipStatusOK = currentPiece->data.flipStatus == 0;
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
        //Line up things that are solved on the top face.
        isOnRow1 = (sourceEdgeIndex >= 0 && sourceEdgeIndex < 5);
        isOnRow2 = (sourceEdgeIndex >= 5 && sourceEdgeIndex < 10);
        isOnRow3 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 15);
        isOnRow34 = (sourceEdgeIndex >= 10 && sourceEdgeIndex < 20); //Middle W
        isOnRow4 = (sourceEdgeIndex >= 15 && sourceEdgeIndex < 20);
        isOnRow6 = (sourceEdgeIndex >= 20 && sourceEdgeIndex < 25);
        isOnRow7 = (sourceEdgeIndex >= 25 && sourceEdgeIndex < 30);
        //Check if the faces we have are considered as the Top half of the cube.
        ontopA = (edgeFaceNeighbors.a > 1 && edgeFaceNeighbors.a < 7);
        ontopB = (edgeFaceNeighbors.b > 1 && edgeFaceNeighbors.b < 7);
        graymatchA = edgeFaceNeighbors.a == GRAY;
        graymatchB = edgeFaceNeighbors.b == GRAY;
        //drop-in directions: left is row 4, right is row 3 (determined from original I, not sourceEdgeIndex)
        isRight = (i >= 10 && i < 15);
        isLeft = (i >= 15 && i < 20);
    }
};

class CornerLayerAssist {
public:
    int sourceCornerIndex;
    colorpiece cornerFaceNeighbors;
    int cornerFaceLocA, cornerFaceLocB, cornerFaceLocC;
    Corner *CornerItselfA;
    bool isOnRow1, isOnRow2, isOnRow3, isOnRow4;
    bool ontopA, ontopB, ontopC;

    CornerLayerAssist(Megaminx* shadowDom, int i) {
        sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the corners are positioned, 0-4
        cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerLoc(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        cornerFaceLocB = shadowDom->faces[cornerFaceNeighbors.b - 1].find5CornerLoc(i);
        assert(cornerFaceLocB != -1); //should not happen
        cornerFaceLocC = shadowDom->faces[cornerFaceNeighbors.c - 1].find5CornerLoc(i);
        assert(cornerFaceLocC != -1); //should not happen
        CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        //Line up things that are solved on the top face.
        isOnRow1 = (sourceCornerIndex >= 0 && sourceCornerIndex < 5);
        isOnRow2 = (sourceCornerIndex >= 5 && sourceCornerIndex < 10);
        isOnRow3 = (sourceCornerIndex >= 10 && sourceCornerIndex < 15);
        isOnRow4 = (sourceCornerIndex >= 15 && sourceCornerIndex < 20);
        //New breakthrough idea, any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        ontopA = (cornerFaceNeighbors.a > 1 && cornerFaceNeighbors.a < 7);
        ontopB = (cornerFaceNeighbors.b > 1 && cornerFaceNeighbors.b < 7);
        ontopC = (cornerFaceNeighbors.c > 1 && cornerFaceNeighbors.c < 7);
    }
};

//Layer 1 part 1 - White Edges
void Megaminx::rotateSolveWhiteEdges(Megaminx* shadowDom)
{
    int i = 0;
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool facesSolved[12] = { false };
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedEdgesUnOrdered(0, piecesSolved);
        bool areEdgesFullySolved[5] = { false };
        shadowDom->DetectSolvedEdges(0, &areEdgesFullySolved[0]);
        int firstSolvedPiece = -1;
        for (int a = 0; a < 5; ++a) {
            facesSolved[1 + a] = piecesSolved[a];
            if (firstSolvedPiece == -1 && piecesSolved[a] == true)
                firstSolvedPiece = a;
        }
        while (i < 5 && areEdgesFullySolved[i] == true)
            i++;
        auto allSolved1 = std::all_of(std::begin(piecesSolved), std::end(piecesSolved), [](bool j) { return j; });
        auto allSolved2 = std::all_of(std::begin(areEdgesFullySolved), std::end(areEdgesFullySolved), [](bool j) { return j; }); //TT88-2
        if (i >= 5 && allSolved1 && allSolved2) {
            allSolved = true;   //TT12-huh
            break;
        }
        //means we got to the end but something else came undone, go back and fix it.
        else if (i >= 5) {
            i = 0;
            continue;
        }
        EdgeLayerAssist l{ shadowDom, i };
        assert(l.colormatchA != l.colormatchB); //sanity check.

        //Rotates the white face to its solved position, first solved edge matches up to its face.
        if (firstSolvedPiece != -1) {
            //NOTE: Doing this over and over is wasting moves solving the partial-solved top every time.
            //TODO: This means we need a plan for any 5-9 edge to get moved into any 0-4 slot even when top isn't solved.
            int findIfPieceSolved = shadowDom->findEdge(firstSolvedPiece); //always piece 0
            int offby = findIfPieceSolved - firstSolvedPiece;    //example: piece 0, 5 - 5 - 0 = 0, so no correction.
            if (findIfPieceSolved > 0 && findIfPieceSolved < 5 && offby > 0) {
                offby *= -1;
                shadowDom->shadowMultiRotate(WHITE, offby);
                continue;
            }
        }
        //Any matching pieces that end up on its matching face can be spun in with just 2 or 1 moves.
        if (l.ontopA && ((l.isOnRow34 && l.colormatchA) || l.isOnRow2)) {
            int offby = l.colormatchA ? (l.edgeFaceNeighbors.a - l.edgeHalfColorA) : (l.edgeFaceNeighbors.b - l.edgeHalfColorB);
            //Set up Rotated White top to be in-line with the face we want to spin in.
            shadowDom->shadowMultiRotate(WHITE, offby);
            if (l.isOnRow34 && l.colormatchA) {
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            }
            else if (l.isOnRow2 && l.colormatchA)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            else if (l.isOnRow2 && l.colormatchB)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        //Locates any straggler pieces on the bottom and bubbles them up to the top layers, as long as the face isnt protected by facesSolved pieces
        else if (l.isOnRow34 && l.dirToWhiteA != 0 && ((l.edgeFaceNeighbors.a < GRAY && !facesSolved[l.edgeFaceNeighbors.a - 1]) || l.edgeFaceNeighbors.a >= GRAY)) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        }
        else if (l.isOnRow34 && l.dirToWhiteB != 0 && ((l.edgeFaceNeighbors.b < GRAY && !facesSolved[l.edgeFaceNeighbors.b - 1]) || l.edgeFaceNeighbors.b >= GRAY)) {
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
        //TT22-L1 //TT30-L1
        else if (l.isOnRow1 && l.dirToWhiteB != 0) //&& !facesSolved[l.edgeFaceNeighbors.b - 1])
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;

        loopcount++;
        //break; //break stops it after only one round
        updateRotateQueueWithShadow(shadowDom);

    } while (!allSolved);
    //If its solved, get top white face spun oriented back to normal
    if (allSolved) {
        int findIfPieceSolved = shadowDom->findEdge(0); //always piece 0
        if (findIfPieceSolved > 0 && findIfPieceSolved < 5) {
            findIfPieceSolved *= -1;
            shadowDom->shadowMultiRotate(WHITE, findIfPieceSolved); //redundant.
        }
    }
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Layer 1 part 2 - White Corners
void Megaminx::rotateSolveWhiteCorners(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedCorners(0, piecesSolved);
        int i = 0; //the starting piece
        while (i < 5 && piecesSolved[i] == true)
            i++;
        if (i >= 5) {
            allSolved = true;
            continue;
        }
        CornerLayerAssist l{ shadowDom, i };
        std::vector<numdir> bulk;

        //Rotates the white face to its solved position, first solved EDGE matches up to its face.
        //Edges should already be solved, if not, get top white face spun oriented back to normal
        int edgesOffBy = shadowDom->findEdge(0); //always piece 0
        if (edgesOffBy > 0 && edgesOffBy < 5) {
            edgesOffBy *= -1;
            shadowDom->shadowMultiRotate(WHITE, edgesOffBy);
        }
        //Move incorrect corners out of the 0-4 slots moves them right down with the algo
        else if (l.isOnRow1 && ((i != l.sourceCornerIndex) || (i == l.sourceCornerIndex && l.CornerItselfA->data.flipStatus != 0)) && piecesSolved[i] == false) {
            int offby = RED;
            if (l.cornerFaceNeighbors.a == WHITE)
                offby += l.cornerFaceLocA;
            else if (l.cornerFaceNeighbors.b == WHITE)
                offby += l.cornerFaceLocB;
            else if (l.cornerFaceNeighbors.c == WHITE)
                offby += l.cornerFaceLocC;
            if (offby > 6)
                offby -= 5;
            if (offby >= 2) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[5].algo, g_faceNeighbors[offby]); // "R' DR' r dr"
                bulkShadowRotate(shadowDom, bulk);
            }
        }
        //Move correct corners straight up and in from 5-9 to 0-4
        else if (l.isOnRow2 && l.sourceCornerIndex - i - 5 == 0) {
            int offby = l.sourceCornerIndex - 5 + RED;
            if (offby > 6)
                offby -= 5;
            if (offby >= 2) {
                if (l.CornerItselfA->data.flipStatus == 2)
                    bulk = shadowDom->ParseAlgorithmString("f dr F' ", g_faceNeighbors[offby]);
                else
                    bulk = shadowDom->ParseAlgorithmString("R' DR' R", g_faceNeighbors[offby]);
                bulkShadowRotate(shadowDom, bulk);
            }
        }
        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        else if (l.isOnRow2) {
            int defaultDir = Face::CW;
            int offby = l.sourceCornerIndex - i - 5;
            megaminxColor turnface;
            //2nd rotation puts them on the gray face.
            if (l.ontopA && l.ontopB)
                turnface = l.cornerFaceNeighbors.c;
            else if (l.ontopA && l.ontopC)
                turnface = l.cornerFaceNeighbors.b;
            else if (l.ontopB && l.ontopC)
                turnface = l.cornerFaceNeighbors.a;
            defaultDir *= 2;
            shadowDom->shadowMultiRotate(turnface, defaultDir);
        }
        //Row 3 pieces go to gray face as temporary holding (1 CW turn) (ends up on row4)
        else if (l.isOnRow3) {
            int defaultDir = Face::CW;
            int offby = l.sourceCornerIndex - i - 10;
            int x, y;
            if (l.ontopA) {
                x = l.cornerFaceNeighbors.b;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopB) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopC) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.b;
            }
            int result = max(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = LIGHT_BLUE;
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2
        else if (l.isOnRow4) {
            int defaultDir = Face::CW;
            int offby = l.sourceCornerIndex - i - 18;
            shadowDom->shadowMultiRotate(GRAY, offby);
            int x = PINK + i;
            MMg(x, BEIGE);
            defaultDir *= 2;
            shadowDom->shadowMultiRotate(x, defaultDir);
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
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedEdges(5, piecesSolved);
        int i = 5; //the starting piece
        while (i < 10 && piecesSolved[i - 5] == true)
            i++;
        if (i >= 10) {
            allSolved = true;
            break;
        }
        EdgeLayerAssist l{ shadowDom,i };
        std::vector<numdir> bulk;

        if ((l.isOnRow2 && (l.sourceEdgeIndex != i || (l.sourceEdgeIndex == i && l.EdgeItselfA->data.flipStatus != 0))) ||
            (l.isOnRow34 && l.ontopA && l.edgeFaceLocA == 4 && l.edgeFaceNeighbors.a == l.edgeHalfColorA)) {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[21].algo, g_faceNeighbors[l.edgeFaceNeighbors.a]);    // "dl l dl L', dL' F' dL' f"
            bulkShadowRotate(shadowDom, bulk);
        }
        else if (l.isOnRow34 && l.ontopA && l.edgeFaceLocA == 3 && l.edgeFaceNeighbors.a == l.edgeHalfColorA) {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[22].algo, g_faceNeighbors[l.edgeFaceNeighbors.a]);    //"dR' R' dR' r, dr f dr F' "
            bulkShadowRotate(shadowDom, bulk);
        }
        //BUG?: Pieces still go the long-way around from 7->6->3->(skip)4->6->7->6->4 then algo....
        //TODO: Need to detect when the face matches up then go backwards.
        else if (l.isOnRow34 && l.dirToWhiteB != 0 && l.edgeFaceNeighbors.b >= GRAY) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
            int sourceEdgeIndexNext = shadowDom->findEdge(i);   //REFRESH
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == l.edgeFaceNeighbors.b)
                //check the solved for the next neighbor (vs what if we just check the original, face.loc.right) ?
                //make sure its the move going to the right
                if (edgeFaceNeighborsNext.a == g_faceNeighbors[l.edgeFaceNeighbors.a].right)
                    shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        //Layer 6 pieces are moved down to gray layer 7
        else if (l.isOnRow6) {
            if (l.dirToWhiteA != 0)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, -1 * l.dirToWhiteA);
            else if (l.dirToWhiteB != 0)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, -1 * l.dirToWhiteB);
        }
        //Layer 7 is an intermediate buffer
        else if (l.isOnRow7) {
            int row7 = l.sourceEdgeIndex - 28 - i;
            bool moved = shadowDom->shadowMultiRotate(GRAY, row7);
            //Align the GRAY layer 7 to be directly underneath the intended solve area
            if (moved == false) {
                if (l.dirToWhiteA != 0) {
                    shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
                    shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
                }
                else if (l.dirToWhiteB != 0) {
                    shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
                    shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
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
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedCorners(5, piecesSolved);
        int i = 5; //the starting piece
        while (i < 10 && piecesSolved[i - 5] == true)
            i++;
        if (i >= 10) {
            allSolved = true;
            break;
        }
        CornerLayerAssist l{ shadowDom, i };

        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        if (l.isOnRow2) {
            int defaultDir = Face::CW;
            int offby = l.sourceCornerIndex - i - 5;
            megaminxColor turnface;
            //2nd rotation puts them on the gray face.
            if (l.ontopA && l.ontopB)
                turnface = l.cornerFaceNeighbors.c;
            else if (l.ontopA && l.ontopC)
                turnface = l.cornerFaceNeighbors.b;
            else if (l.ontopB && l.ontopC)
                turnface = l.cornerFaceNeighbors.a;
            //If the piece is solved positioned, but color flipped wrong:
            if (l.sourceCornerIndex == i)
                defaultDir *= -1;
            // turn opposite direction row2->row3->row4 to use gray face then repeat
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
        }
        //Row 3 pieces go to gray face as temporary holding (1 CCW turn) (ends up on row4)
        else if (l.isOnRow3) {
            int defaultDir = Face::CCW;
            int offby = l.sourceCornerIndex - i - 10;
            int x = 0, y = 0;
            if (l.ontopA) {
                x = l.cornerFaceNeighbors.b;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopB) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopC) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.b;
            }
            //The loop point crosses over at the Pink 
            if (x < PINK)
                x += 5;
            if (y < PINK)
                y += 5;
            int result = max(x, y);
            MMg(result, BEIGE);
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2 Solved
        else if (l.isOnRow4) {
            int defaultDir = Face::CW;
            int offby = l.sourceCornerIndex - i - 18;
            shadowDom->shadowMultiRotate(GRAY, offby);
            int x = PINK + i;
            MMg(x, BEIGE);
            defaultDir *= 2;
            shadowDom->shadowMultiRotate(x, defaultDir);
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
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[10] = { false };
        shadowDom->DetectSolvedEdges(10, piecesSolved);
        shadowDom->DetectSolvedEdges(15, &piecesSolved[5]);
        int i = 10; //the piece
        while (i < 20 && piecesSolved[i - 10] == true)
            i++;
        if (i >= 20) {
            allSolved = true;
            break;
        }
        EdgeLayerAssist l{ shadowDom,i };
        std::vector<numdir> bulk;

        //Get ready for algorithms
        if ((l.isOnRow34 && (l.sourceEdgeIndex != i || (l.sourceEdgeIndex == i && l.EdgeItselfA->data.flipStatus != 0)))
            || (l.isOnRow7)) {
            colordirs loc;
            if (l.isOnRow7) {
                //Align GRAY top to the exact position for pre-drop-in.
                int offby = l.sourceEdgeIndex - 27 - i;
                if (l.isLeft) //row 4B's B-half is +1 from row4A's B-half (DARK_BLUE,LIGHT_GREEN) vs (DARK_BLUE,PINK)
                    offby -= 1;
                bool moved = shadowDom->shadowMultiRotate(GRAY, offby);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
                loc = (l.graymatchA) ? g_faceNeighbors[l.edgeFaceNeighbors.b] : g_faceNeighbors[l.edgeFaceNeighbors.a];
            }
            //obtain the non-gray face neighbor we need to be rotating
            else if (l.isOnRow34)
                loc = g_faceNeighbors[max(l.edgeFaceNeighbors.b, l.edgeFaceNeighbors.a)];
            //works to insert pieces from row7 to 3/4 and also pops wrong pieces out from 3/4 to 6            
            if ((l.isOnRow7 && l.isLeft) || (l.isOnRow4)) {
                if (l.EdgeItselfA->data.flipStatus == 0 || (l.isOnRow4))
                    bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[23].algo, loc); // {23, "F' R' F', F' r f"},
                else
                    bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[34].algo, loc); // {34, "U' R' DR' F F DR R" },
            }
            else if ((l.isOnRow7 && l.isRight) || (l.isOnRow3)) {
                if (l.EdgeItselfA->data.flipStatus == 0 || (l.isOnRow3))
                    bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[24].algo, loc); // {24, "f l f, f L' F' "},
                else
                    bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[35].algo, loc); // {35, "u l dl F' F' DL' L' "}
            }
            bulkShadowRotate(shadowDom, bulk);
        }
        //Row 6 - flip piece up to GRAY, then return the moved faces to unharm the low corners.
        else if (l.isOnRow6) {
            if (l.dirToWhiteA != 0) {
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, -1 * l.dirToWhiteA);
                shadowDom->shadowRotate(GRAY, l.dirToWhiteA);
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
            }
            else if (l.dirToWhiteB != 0) {
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, -1 * l.dirToWhiteB);
                shadowDom->shadowRotate(GRAY, l.dirToWhiteB);
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
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
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedCorners(10, piecesSolved);
        int i = 10; //the starting piece
        while (i < 15 && piecesSolved[i - 10] == true)
            i++;
        if (i >= 15) {
            allSolved = true;
            break;
        }
        CornerLayerAssist l{ shadowDom, i };
        std::vector<numdir> bulk;

        //Get the Pieces to drop-in ready on Row 4 (gray layer) solved into Row3
        //Any Row 3 pieces that are mis-solved use same algo to go up to gray layer (ends up on row4)
        if (l.isOnRow3) {
            int x, y;
            if (l.ontopA) {
                x = l.cornerFaceNeighbors.b;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopB) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopC) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.b;
            }
            int result = min(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = BEIGE;
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[1].algo, g_faceNeighbors[result]); // "r u R' U' "
            bulkShadowRotate(shadowDom, bulk);
        }
        else if (l.isOnRow4) {
            //Orient Gray Top layer (index goes in reverse)
            int offby = l.sourceCornerIndex - i * -1;
            shadowDom->shadowMultiRotate(GRAY, offby);
            //quick shortcut to know which face we're working on.
            int front = BEIGE - (i - 10);
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[1].algo, g_faceNeighbors[front]); // "r u R' U' "
            bulkShadowRotate(shadowDom, bulk);
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
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedEdges(20, piecesSolved);
        int i = 20; //the starting piece
        while (i < 25 && piecesSolved[i - 20] == true)
            i++;
        if (i >= 25) {
            allSolved = true;
            break;
        }
        EdgeLayerAssist l{ shadowDom,i };
        std::vector<numdir> bulk;

        //Get ready for algorithms
        if ((l.isOnRow6 && (l.sourceEdgeIndex != i || (l.sourceEdgeIndex == i && l.EdgeItselfA->data.flipStatus != 0)))
            || (l.isOnRow7)) {
            if (l.isOnRow7) {
                //assert(l.dirToWhiteA == 0);
                int offby = l.graymatchA ? (l.edgeFaceNeighbors.b - l.edgeHalfColorB) : (l.edgeFaceNeighbors.a - l.edgeHalfColorA);
                //Align GRAY top to the exact position for pre-drop-in.
                bool moved = shadowDom->shadowMultiRotate(GRAY, offby);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
            }
            //obtain the non-gray face neighbor we need to be oriented to
            int result = 0;
            int x = l.edgeFaceNeighbors.a;
            int y = l.edgeFaceNeighbors.b;
            bool isLeft = false, isRight = false;
            colordirs loc;
            if (l.isOnRow7) {
                result = (l.graymatchA) ? y : x;
                loc = g_faceNeighbors[result]; //loc.front
                isLeft = (loc.left == l.edgeHalfColorA);
                isRight = (loc.right == l.edgeHalfColorA);
            }
            else if (l.isOnRow6) {
                result = min(x, y);
                if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                    result = BEIGE;
                loc = g_faceNeighbors[result]; //loc.front
                isRight = true;
            }             
            //Check left/right faces for which direction to drop-in with isLeft/isRight
            int algo = isLeft ? 25 : isRight ? 26 : 25;
            //works to insert pieces from row7 to 6 and also pops wrong pieces out from 6 to 7
                                                                                    //{25, "U' L' u l, u f U' F' "},
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[algo].algo, loc);  //{26, "u r U' R', U' F' u f "},
            bulkShadowRotate(shadowDom, bulk);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

//Last Layer = Layer 7 EDGES (GRAY top)
//Cube must have gray side on top, layer 1+2+3+4+5+6 Solved
void Megaminx::rotateSolveLayer7Edges(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;    
    int loopcount = 0;
    int unknownloop = 0;
    //Loop:
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        int i = 25; //the starting piece
        //Stores piece and color status from the gray face, as detected by the functions
        bool piecesSolvedStrict[5] = { false };
        shadowDom->DetectSolvedEdges(i, &piecesSolvedStrict[0]);
        //Iterate past any already completely solved pieces in order.
        while (i < 30 && piecesSolvedStrict[i - 25] == true)
            i++;
        //Skip loop if we are completely solved.
        if (i >= 30) {
            allSolved = true;
            break;
        }
        bool grayFaceColorSolved[5] = { false };
        bool piecesSolvedMaybe[5] = { false };
        shadowDom->DetectSolvedEdgesUnOrdered(i, &piecesSolvedMaybe[0]);
        std::vector<int> pieceOrder(5);
        //Compile the accurate color solved maybe list.  (if rotated)
        for (int k = 0; k < 5; ++k) {
            Edge* EdgeItselfNext = shadowDom->faces[GRAY - 1].edge[k];
            if (EdgeItselfNext->data.flipStatus == 0)
                grayFaceColorSolved[k] = true;
            pieceOrder[k] = EdgeItselfNext->data.pieceNum;
        }
        //Asks some additional questions about what pieces/colors are concrete viable options
        int solvedCount = 0;
        int firstSolvedPiece = -1, firstUnsolvedPiece = -1;
        int lastSolvedPiece = -1, firstOffColorPiece = -1;
        for (int k = 0; k < 5; ++k) {
            if (piecesSolvedStrict[k]) {
                solvedCount++;
                lastSolvedPiece = k;
                if (firstSolvedPiece == -1)
                    firstSolvedPiece = k;
            }
            else if (firstUnsolvedPiece == -1)
                firstUnsolvedPiece = k;
            if (!grayFaceColorSolved[k] && firstOffColorPiece == -1)
                firstOffColorPiece = k;
        }
        if (lastSolvedPiece == 4 && firstSolvedPiece == 0)
            lastSolvedPiece = 0;
        if (!grayFaceColorSolved[4] && !grayFaceColorSolved[0])
            firstOffColorPiece = 4;

        //iterate through the piece/color lists and make some easy compound conditional aliases as conceptual shortcuts
        //twoSolvedPieces(a,b) - Lambda for Solved pieces
        auto twoSolvedPieces = [piecesSolvedStrict](int a, int b) { return (piecesSolvedStrict[a] && piecesSolvedStrict[b]); };
        bool twoAdjacentPieces =
             (twoSolvedPieces(0, 1)) || (twoSolvedPieces(1, 2)) || (twoSolvedPieces(2, 3)) || (twoSolvedPieces(3, 4)) || (twoSolvedPieces(4, 0));
        bool twoOppositePieces =
             (twoSolvedPieces(0, 2)) || (twoSolvedPieces(1, 3)) || (twoSolvedPieces(2, 4)) || (twoSolvedPieces(3, 0)) || (twoSolvedPieces(4, 1));
        //twoGraysUnsolved(a,b) - Lambda for Unsolved pieces
        auto twoGraysUnsolved = [grayFaceColorSolved](int a, int b) { MM(a); MM(b); return (!grayFaceColorSolved[a] && !grayFaceColorSolved[b]); }; //MM=overrotation support
        bool twoAdjacentOffColors = 
             (twoGraysUnsolved(0, 1)) || (twoGraysUnsolved(1, 2)) || (twoGraysUnsolved(2, 3)) || (twoGraysUnsolved(3, 4)) || (twoGraysUnsolved(4, 0));
        bool twoOppositeOffColors = 
             (twoGraysUnsolved(0, 2)) || (twoGraysUnsolved(1, 3)) || (twoGraysUnsolved(2, 4)) || (twoGraysUnsolved(3, 0)) || (twoGraysUnsolved(4, 1));        
        auto matchesOppoColor = [twoGraysUnsolved](int face) {
            face -= LIGHT_BLUE;
            return twoGraysUnsolved(face + 2, face + 4);
        };
        //Set up important piece variables
        std::vector<numdir> bulk;
        int sourceEdgeIndex = shadowDom->findEdge(i);
        int offby = sourceEdgeIndex - i;
        //auto currentPiece = shadowDom->getPieceArray<Edge>(sourceEdgeIndex);
        //bool currentpieceFlipStatusOK = currentPiece->data.flipStatus == 0;
        bool currentpieceFlipStatusOK = grayFaceColorSolved[i];
        //Check gray CORNERS also, "maybe" we want to preserve them.
        bool grayCornerColorSolved[5] = { false };
        shadowDom->DetectSolvedCorners(15, grayCornerColorSolved);
        bool allCornersAllSolved = std::all_of(std::begin(grayCornerColorSolved), std::end(grayCornerColorSolved), [](bool j) { return j; });
        bool allEdgeColorsSolved = std::all_of(std::begin(grayFaceColorSolved),   std::end(grayFaceColorSolved),   [](bool j) { return j; });

//START MAIN:

        //orient the first piece if it exists:
        int findIfPieceSolved = shadowDom->findEdge(25); //always piece 0
        if (findIfPieceSolved > 25 && findIfPieceSolved < 30 && !allCornersAllSolved && !piecesSolvedStrict[0] && offby != 0 && !twoAdjacentPieces) {
            //Test 3 passes.
            int offby = findIfPieceSolved - 25;
            shadowDom->shadowMultiRotate(GRAY, offby);
            continue;
        }
        //Rotates the GRAY face to any solved position, first out of order but solved EDGE rotates to match up to its face.
        else if (!piecesSolvedStrict[0] && !twoAdjacentPieces && !allCornersAllSolved && solvedCount >= (i - 25) && piecesSolvedMaybe[i - 25]) {
            int findIfPieceSolved = shadowDom->findEdge(i + firstSolvedPiece);
            int offby = findIfPieceSolved - i + firstSolvedPiece;
            if (findIfPieceSolved >= 25 && findIfPieceSolved < 30 && offby != 0) {
                shadowDom->shadowMultiRotate(GRAY, offby);
                continue;
            }
        }
//5-way cycle -2
         if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 28, 29, 25, 26, 27)) //TT55-3
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[40].algo, g_faceNeighbors[LIGHT_BLUE]);    //algo17@Blue (60 moves to 48)
        }
//5-way cycle +2
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 28, 29, 25, 26)) //TT53-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[41].algo, g_faceNeighbors[LIGHT_BLUE]);    //algo38@Blue (60 moves to 48)
        }
//TODO: Combine these 5 better
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 29, 27, 28, 25, 26)) //TT54-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[LIGHT_BLUE]);   //algo18@Green (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 25, 28, 29, 26)) //TT58-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[ORANGE]);    //algo18@Pink (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 28, 26, 29, 25)) //TT59-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[LIGHT_GREEN]);  //algo18@Beige (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 26, 28, 29, 27, 25)) //TT56-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[PINK]);  //algo18@Blue (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 26, 27, 29, 25, 28)) //TT57-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[BEIGE]); //algo18@Orange (60 moves to 32)
        }
//TODO: Combine these 5 also
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 28, 29, 26, 27, 25)) //TT86-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[LIGHT_BLUE]);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 26, 29, 25, 27, 28))
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[ORANGE]);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 29, 27, 25, 26, 28))
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[LIGHT_GREEN]);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 29, 25, 28, 26, 27))
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[PINK]);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 28, 25, 26, 29, 27))
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[BEIGE]);
        }

//BUNNY 4-COLOR-only (with 1 solved, covers all possibilities)
        else if (solvedCount == 1 && !allEdgeColorsSolved && allCornersAllSolved
             && twoGraysUnsolved(firstSolvedPiece + 1, firstSolvedPiece + 2)
             && twoGraysUnsolved(firstSolvedPiece + 3, firstSolvedPiece + 4)
        ) {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[29].algo, g_faceNeighbors[LIGHT_BLUE + firstSolvedPiece]);    //algo #29 Quad-Flip Colors
        }
//BUNNY 2-COLOR: (OFF-COLOR OPPOSITES)
        //If two opposite Pieces are colored wrong, Choose the face that places them @ 8' & 1' oclock and then run Algo #20 to invert those
        else if (twoOppositeOffColors && !allEdgeColorsSolved) {
            for (int op = LIGHT_BLUE; op <= BEIGE; ++op)
                if (matchesOppoColor(op))
                    bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, g_faceNeighbors[op]); //algo #20 = INVERT colors @ 8'/1'
        }
//BUNNY 2+2SWAP
// 2&5 + 3&4 (opposite horizontally)
        else if (solvedCount == 1 && allEdgeColorsSolved && allCornersAllSolved &&
              (checkPieceMatches(pieceOrder, 25, 29, 28, 27, 26) //TT79-2 (25)
            || checkPieceMatches(pieceOrder, 27, 26, 25, 29, 28) //TT78-2 (26)
            || checkPieceMatches(pieceOrder, 29, 28, 27, 26, 25) //TT77-2 (27)
            || checkPieceMatches(pieceOrder, 26, 25, 29, 28, 27) //TT61-2 (28)
            || checkPieceMatches(pieceOrder, 28, 27, 26, 25, 29)) //TT46-2 (29)

        ) {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[37].algo, g_faceNeighbors[LIGHT_BLUE + firstSolvedPiece]); //algo #37, 13*5=65 moves
            for (int i = 0; i < g_AlgoStrings[37].repeatX; ++i)
                bulkShadowRotate(shadowDom, bulk);
            updateRotateQueueWithShadow(shadowDom);
            continue;
        }

//BUNNY 2+2SWAP
// 2&3 + 4&5 (adjacent vertically)
        else if (// allCornersAllSolved && (//TT-69-2 TODO: when Corners arent solved we can skip more moves w/ E+C Algo.
               solvedCount == 1 && 
              (checkPieceMatches(pieceOrder, 25, 27, 26, 29, 28) //TT65-2 (25)
            || checkPieceMatches(pieceOrder, 29, 26, 28, 27, 25) //TT66-2 (26)
            || checkPieceMatches(pieceOrder, 26, 25, 27, 29, 28) //TT67-2 (27)
            || checkPieceMatches(pieceOrder, 29, 27, 26, 28, 25) //TT64-2 (28)
            || checkPieceMatches(pieceOrder, 26, 25, 28, 27, 29)) //TT68-2 (29)
        ) {
            //Streamlined previous algo19 from 44 moves to 32 :
            int start = LIGHT_BLUE + firstSolvedPiece + 1;
            MMg(start, BEIGE);
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[42].algo, g_faceNeighbors[start]); //algo#42=algo#19
        }

//MUSHROOM- //algo #14 3a-  (F/L Safe)

        else if (offby == 0 && solvedCount == 3 && !allEdgeColorsSolved && (twoGraysUnsolved(1, 2) || twoGraysUnsolved(2, 3))) //TT62-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece]);  //algo #14 3a-  (F/L Safe)
        }
        else if (offby == 4 && solvedCount == 1 && !allEdgeColorsSolved && twoGraysUnsolved(3, 4)) //TT-84-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece]);  //algo #14 3a-  (F/L Safe)
        }
        else if (offby == 3 && solvedCount == 1 && !allEdgeColorsSolved && twoGraysUnsolved(3, 4)) //TT-85-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece]);  //algo #14 3a-  (F/L Safe)
        }
        //first and second piece solved for sure, attached
        else if (
              (offby == 2 && solvedCount == 1 && allEdgeColorsSolved)
            ||(offby == 2 && solvedCount >= 2 && twoAdjacentPieces && (allEdgeColorsSolved || twoGraysUnsolved(1, 2) || twoGraysUnsolved(2, 3)))  //test5
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(0,1) && twoGraysUnsolved(3,4))
            ||(offby == 2 && solvedCount == 2 && twoAdjacentPieces && !currentpieceFlipStatusOK && twoSolvedPieces(0, 1) && twoGraysUnsolved(3, 4)) //TT11
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && !currentpieceFlipStatusOK && twoSolvedPieces(0, 4) && twoGraysUnsolved(2, 3)) //TT15
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && !currentpieceFlipStatusOK && twoSolvedPieces(3, 4) && twoGraysUnsolved(1, 2)) //TT29-2
            ||(offby == 4 && solvedCount == 2 && twoAdjacentPieces && allEdgeColorsSolved && twoSolvedPieces(1, 2)) //TT14
            ||(offby == 4 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(2, 3)) //TT27-2
            ||(offby == 4 && solvedCount == 2 && twoAdjacentPieces &&  currentpieceFlipStatusOK && twoSolvedPieces(2, 3) && grayFaceColorSolved[4] && twoGraysUnsolved(0, 1)) //TT9
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && !currentpieceFlipStatusOK && twoSolvedPieces(2, 3) && grayFaceColorSolved[4] && twoGraysUnsolved(0, 1)) //TT9
            ||(offby == 2 && solvedCount == 1 && piecesSolvedStrict[0] && !piecesSolvedStrict[1]) //Test2-pt2-pass      //Test4-fixns    //test5
            ||(offby == 3 && solvedCount == 1 && allCornersAllSolved && allEdgeColorsSolved && (piecesSolvedStrict[1] || piecesSolvedStrict[2])) //TT37-1 //TT42-2
            ||(offby == 3 && solvedCount == 1 && piecesSolvedStrict[0] && twoGraysUnsolved(3, 4))   //TestCube22-2Edges
            ||(offby == 1 && solvedCount == 1 && !currentpieceFlipStatusOK && piecesSolvedStrict[0] && twoGraysUnsolved(2, 3)) //TT20-2
        ) {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + lastSolvedPiece]);
        }
        else if (offby == 0 && solvedCount == 3 && twoAdjacentPieces && twoSolvedPieces(0,1) && piecesSolvedStrict[2] && twoGraysUnsolved(3,4))
        {                                                                                 //TestCube15-2-Edgesstuck
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[ORANGE]);
        }
        else if ((offby == 2 || offby == 3) && solvedCount == 1 && piecesSolvedStrict[4] && twoGraysUnsolved(2,3))
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE]);
        }
        else if (offby == 0 && !currentpieceFlipStatusOK && twoGraysUnsolved(0,1))
        { 
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE]);
        }
        else if (offby == 3 && solvedCount == 0 && allCornersAllSolved && !piecesSolvedStrict[0] && twoGraysUnsolved(1,2)) //TT35-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + offby]);
        }
        else if (offby == 2 && solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved)  //TT38-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE]);
        }
        else if (offby == 2 && solvedCount == 1 && allCornersAllSolved && piecesSolvedStrict[1] && twoGraysUnsolved(2,3)) //TT43-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE]);
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && !currentpieceFlipStatusOK && twoGraysUnsolved(4,0)) //TT45-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE]);
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(2,3)) //TT48-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE]);
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && pieceOrder[0] == 26) //TT50-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[PINK]);
        }
        else if (offby == 3 && solvedCount == 1 && piecesSolvedStrict[1] && allCornersAllSolved && twoGraysUnsolved(3,4) && pieceOrder[0] == 28) //TT51-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[PINK]);
        }
        else if (offby == 2 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(4,0) && pieceOrder[0] == 26) //TT52-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_GREEN]);
        }
        else if (offby == 1 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(3,4) && pieceOrder[0] == 28) //TT60-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE]);
        }
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 26, 29, 28, 25, 27) && allEdgeColorsSolved && allCornersAllSolved) //TT49-2 
        {   //(needs another mushroom- after this one to solve it)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[ORANGE]);
        }   //becomes "26 29 [27 28] 25"
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 28, 27, 29, 26, 25) && allEdgeColorsSolved && allCornersAllSolved) //TT73-2 
        {   //(needs another mushroom- after this one to solve it)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE]);
        }   //becomes "28 [26 27] 29 25"
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 27, 29, 28, 26, 25) && allEdgeColorsSolved && allCornersAllSolved) //TT87-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[PINK]);
        } //becomes "25] 27 28 26 [29"

//MUSHROOM+
        else if (offby == 3 && solvedCount == 1 && piecesSolvedStrict[0] && twoGraysUnsolved(2,3))
        {                                                                                    //TestCube14-2
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[ORANGE]); //3e+ #33 same. (F/L safe tho)
        }
        else if (
              (offby == 1 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(0,1) && (allEdgeColorsSolved || twoGraysUnsolved(2, 3)))  //test4-p2 ->(blue/orange)=PASS //TestCube6-2
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && allEdgeColorsSolved)
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(4,0) && !currentpieceFlipStatusOK && twoGraysUnsolved(1,2)) //TT18-2
            ||(offby == 4 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(1,2) && !currentpieceFlipStatusOK && twoGraysUnsolved(3,4)) //TT17-2
            ||(offby == 4 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(2,3) && !currentpieceFlipStatusOK && twoGraysUnsolved(4,0)) //TT23-2
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(3,4) && !currentpieceFlipStatusOK && twoGraysUnsolved(0,1)) //TT26-2
            ||(offby == 2 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(3,4) && twoGraysUnsolved(0,1)) //TT30-2
            ||(offby == 1 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(2,3) && twoGraysUnsolved(4,0)) //TT33-2
            ||(offby == 3 && solvedCount == 2 && twoAdjacentPieces && twoSolvedPieces(1,2))  //test3-pt2-manual->(orange/green)=PASS
            ||(offby == 1 && solvedCount == 1 && piecesSolvedStrict[0] && twoGraysUnsolved(3, 4) && grayFaceColorSolved[1] && grayFaceColorSolved[2])
            ||(offby  < 5 && solvedCount == 1 && piecesSolvedStrict[0] && !currentpieceFlipStatusOK && twoGraysUnsolved(1, 2)) //TT13   //TT19-2
            ||(offby == 2 && solvedCount == 1 && piecesSolvedStrict[4] && twoGraysUnsolved(0, 1)) //TT81-2
        ) {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE + lastSolvedPiece]); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 3 && solvedCount == 1 && piecesSolvedStrict[4] && allCornersAllSolved && twoGraysUnsolved(1,2)) //TT36-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE]); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 1 && solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved && pieceOrder[0] == 29) //TT44-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE + offby + 2]); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(3,4)) //TT47-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[PINK]); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 3 && solvedCount == 1 && allCornersAllSolved && twoGraysUnsolved(2, 3)) //TT80-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_GREEN]); //3e+ #33 same. (F/L safe tho)
        }
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 27, 29, 26, 25, 28) && allEdgeColorsSolved && allCornersAllSolved) //TT70-2 
        {   //(needs another mushroom+ after this one to solve it)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[BEIGE]); //3e+ #33 same. (F/L safe tho)
        }   //becomes "29 [26 27] 25 28"
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 29, 28, 26, 25, 27) && allEdgeColorsSolved && allCornersAllSolved) //TT72-2 
        {   //(needs another mushroom+ after this one to solve it)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_GREEN]); //3e+ #33 same. (F/L safe tho)
        }   //becomes "25] 28 26 27 [29"
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 28, 29, 26, 27, 25) && allEdgeColorsSolved && allCornersAllSolved) //TT75-2 
        {   //(needs another mushroom- after this one to solve it)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE]); //3e+ #33 same. (F/L safe tho)
        }   //becomes "28 [26 27] 29 25 "
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 27, 25, 29, 26, 28) && allEdgeColorsSolved && allCornersAllSolved) //TT82-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[ORANGE]); //3e+ #33 same. (F/L safe tho)
        }
        else if (solvedCount == 0 && checkPieceMatches(pieceOrder, 28, 25, 29, 27, 26) && allEdgeColorsSolved && allCornersAllSolved) //TT83-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[PINK]); //3e+ #33 same. (F/L safe tho)
        }

//HORSEFACE-
        //5 (+) iterations also preserves corners, 2 (+) iterations does not, 1 opposite (-) iteration does not.
        //opposites faces at 6' and 1' must be solved
        else if (offby >= 3 && solvedCount == 2 && twoOppositePieces)
        {
            colordirs loc;
            if (twoSolvedPieces(0, 2))
                loc = g_faceNeighbors[LIGHT_BLUE];
            else if (twoSolvedPieces(1, 3))
                loc = g_faceNeighbors[ORANGE];
            else if (twoSolvedPieces(2, 4))
                loc = g_faceNeighbors[ORANGE];
            else if (twoSolvedPieces(3, 0))
                loc = g_faceNeighbors[PINK];
            else if (twoSolvedPieces(4, 1))
                loc = g_faceNeighbors[BEIGE];
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, loc);    //algo #12 (3.1- CCW)
                for (int i = 0; i < g_AlgoStrings[12].repeatX; ++i)
                    bulkShadowRotate(shadowDom, bulk);
                updateRotateQueueWithShadow(shadowDom);
                continue;
            }
            else
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, loc);    //algo #13 (3.2+ CW) (opposite #12, w/ 1 rep)
        }
        else if (offby == 3 && piecesSolvedStrict[0] && !piecesSolvedStrict[1])
        {
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, g_faceNeighbors[LIGHT_BLUE]);    //algo #12 (3.1- CCW)
                for (int i = 0; i < g_AlgoStrings[12].repeatX; ++i)
                    bulkShadowRotate(shadowDom, bulk);
                updateRotateQueueWithShadow(shadowDom);
                continue;
            }
            else
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, g_faceNeighbors[LIGHT_BLUE]);    //algo #13 (3.2+ CW) (opposite #12, w/ 1 rep)
        }
        else if (offby == 3 && solvedCount == 2 && twoSolvedPieces(2, 4) && allCornersAllSolved) //TT-40-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, g_faceNeighbors[LIGHT_GREEN]);    //algo #12 (3.1- CCW)
            for (int i = 0; i < g_AlgoStrings[12].repeatX; ++i)
                bulkShadowRotate(shadowDom, bulk);
            updateRotateQueueWithShadow(shadowDom);
            continue;
        }

//HORSEFACE+
        else if (solvedCount == 2 && twoOppositePieces && (allEdgeColorsSolved || twoGraysUnsolved(1, 2)) && (offby == 1 || offby == 2)) //TT39-2 //TT41-2 //TT74-2
        {                                                    //testCube3-p2 ->(blue/green)=PASS  + //TT24-2
            colordirs loc;
            if (twoSolvedPieces(0, 2))
                loc = g_faceNeighbors[LIGHT_BLUE];
            else if (twoSolvedPieces(1, 3))
                loc = g_faceNeighbors[ORANGE];
            else if (twoSolvedPieces(2, 4))
                loc = g_faceNeighbors[ORANGE];
            else if (twoSolvedPieces(3, 0))
                loc = g_faceNeighbors[PINK];
            else if (twoSolvedPieces(4, 1))
                loc = g_faceNeighbors[BEIGE];
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, loc);    //algo #13 (3.2+ CW)
                for (int i = 0; i < g_AlgoStrings[13].repeatX; ++i)
                    bulkShadowRotate(shadowDom, bulk);
                updateRotateQueueWithShadow(shadowDom);
                continue;
            }
            else
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, loc);    //algo #12 (3.1- CCW)
        }
        else if ((offby == 3 && solvedCount == 1 && piecesSolvedStrict[0] && twoGraysUnsolved(1, 2) && grayFaceColorSolved[3] && grayFaceColorSolved[4])
            || (offby == 0 && !piecesSolvedStrict[0] && twoGraysUnsolved(4, 0))
            || (offby == 2 && solvedCount == 2 && twoSolvedPieces(0, 2) && twoGraysUnsolved(3, 4))    //TT10
        ) {
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, g_faceNeighbors[LIGHT_BLUE]);    //algo #13 (3.2+ CW)
                for (int i = 0; i < g_AlgoStrings[13].repeatX; ++i)
                    bulkShadowRotate(shadowDom, bulk);
                updateRotateQueueWithShadow(shadowDom);
                continue;
            }
            else
                bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, g_faceNeighbors[LIGHT_BLUE]);    //algo #12 (3.1- CCW)
        }

//twoAdjacentOffColors, then mushroom starting from there.
        else if (twoAdjacentOffColors) //TT-85-2,  //TT63-2
        {
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece]);  //algo #14 3a-  (F/L Safe)
        }

        else
            unknownloop++;
        //DO IT:
        bulkShadowRotate(shadowDom, bulk);
        updateRotateQueueWithShadow(shadowDom);
        //break; //break stops it after only one round

        loopcount++;
    } while (!allSolved);

    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
    assert(unknownloop == 0);
}

static int g_dirtyBitRDRD = 0;
static int g_dirtyFaceRDRD = 0;
static int g_dirtyCountRDRD = 0;

//Last Layer = Layer 7 CORNERS (GRAY top)
//Cube should have gray side on top, layer 1+2+3+4+5+6 Solved
void Megaminx::rotateSolve7thLayerCorners(Megaminx* shadowDom)
{
//COMMENTED OUT TEMPORARILY
    //bool areEdgesSolved[5] = { false };
    //shadowDom->DetectSolvedEdges(25, &areEdgesSolved[0]);
    //for (int k = 0; k < 5; ++k) {
    //    if (!areEdgesSolved[k])
    //        return;
    //}

    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    //Loop:
    do {
        //basic overflow protection:
        if (loopcount > 120)
            break;
        bool grayFaceColorSolved[5] = { false };
        bool piecesSolvedStrict[5] = { false };
        shadowDom->DetectSolvedCorners(15, piecesSolvedStrict);
        int i = 15; //the starting piece
        while (i < 20 && piecesSolvedStrict[i - 15] == true)
            i++;
        if (i >= 20) {
            allSolved = true;
            break;
        }
        //Set up main loop vars
        int sourceCornerIndex = shadowDom->findCorner(i);
        auto currentPiece = shadowDom->getPieceArray<Corner>(sourceCornerIndex);
        bool currentpieceFlipStatusOK = currentPiece->data.flipStatus == 0;
        int offby = sourceCornerIndex - i;
        std::vector<numdir> bulk;
        std::vector<int> pieceOrder (5);
        //Compile the accurate color solved maybe list.  (if rotated)
        for (int k = 0; k < 5; ++k) {
            Corner* CornerItselfNext = shadowDom->faces[GRAY - 1].corner[k];
            if (CornerItselfNext->data.flipStatus == 0)
                grayFaceColorSolved[k] = true;
            pieceOrder[k] = CornerItselfNext->data.pieceNum;
        }
        //iterate through the piece/color lists and make some easy compound conditional aliases
        bool fullySolvedOrder = checkPieceMatches(pieceOrder, 15, 16, 17, 18, 19);
        bool hasTwoAdjacentSolved = false;
        int adjacentStart = 0;
        for (int k = 0; k < 5; ++k) {
            if ((k + 15 == pieceOrder[k]) &&
                ((k < 4 && pieceOrder[k] + 1 == pieceOrder[k + 1]) ||
                (k == 4 && pieceOrder[4] == 19 && pieceOrder[0] == 15)))
            {
                hasTwoAdjacentSolved = true;
                adjacentStart = k;
            }
        }
//Not Needed. L7-Edges is mandatorily run as a pre-requisite.
        // firstly rotate solved Edge 0 (e#25) gray top back to default, by checking the first EDGE and setting that rotation.
        int findIfFirstEdgeDefault = shadowDom->findEdge(25); //always piece 0
        if (findIfFirstEdgeDefault > 25 && findIfFirstEdgeDefault < 30 && !piecesSolvedStrict[0] && offby != 0)
        {
            int offby = findIfFirstEdgeDefault - 25;
            shadowDom->shadowMultiRotate(GRAY, offby);
            continue;
        }
        //int safeStart = hasTwoAdjacentSolved ? adjacentStart : offby;

//START MAIN
        
        //skip to the end if positioned correctly
        if (fullySolvedOrder)
            goto startColorFlippingCorners;

        //skip to the middle if we can, starting with pairs
        else if (hasTwoAdjacentSolved)
            goto haveTwoAdjacentSolved;

//49 Testcases Total:

//10 "no visible progress" cases
        //(15 18 19 16 17)
        else if (checkPieceMatches(pieceOrder, 15, 18, 19, 16, 17))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with  --> "15 16 18 19 17" below (no pairs yet)

        //(16 15 19 18 17)
        else if (checkPieceMatches(pieceOrder, 16, 15, 19, 18, 17))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "17 16 19 18 15" below (no pairs yet)

        //(17 16 15 19 18) (swap 1&3+4/5)
        else if (checkPieceMatches(pieceOrder, 17, 16, 15, 19, 18))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "18 16 15 17 19" below (no pairs yet)

        //(18 17 15 19 16)
        else if (checkPieceMatches(pieceOrder, 18, 17, 15, 19, 16))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "17 15 18 19 16" below (no pairs yet)

        //(18 19 15 16 17)
        else if (checkPieceMatches(pieceOrder, 18, 19, 15, 16, 17))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "18 15 16 19 17" below. (no pairs yet)

        //(18 17 16 15 19)
        else if (checkPieceMatches(pieceOrder, 18, 17, 16, 15, 19))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "15 17 16 19 18" below (no pairs yet)

        //(15 19 18 17 16) - TestCube28-1 
        else if (checkPieceMatches(pieceOrder, 15, 19, 18, 17, 16))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //  --> continues with "19 18 15 17 16" below (no pairs yet)

        //(19 18 15 17 16) - TestCube27-1 
        else if (checkPieceMatches(pieceOrder, 19, 18, 15, 17, 16))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //  --> continues with "18 15 19 17 16" below (no pairs yet)

        //(17 18 19 15 16) - TestCube18-1 
        else if (checkPieceMatches(pieceOrder, 17, 18, 19, 15, 16))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "15 18 19 16 17" below (no pairs yet)

        //(19 18 17 16 15) - TT21-2
        else if (checkPieceMatches(pieceOrder, 19, 18, 17, 16, 15))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "15 19 17 16 18" belo (no pairs yet)

//39 cases to make pairs:
        //(18 15 19 17 16) - TestCube25-1 
        else if (checkPieceMatches(pieceOrder, 18, 15, 19, 17, 16))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //  --> continues with "15 16 19 17 18" below && hasTwoAdjacentSolved now - 15/16

        //(17 16 19 18 15) - TestCube26-2 
        else if (checkPieceMatches(pieceOrder, 17, 16, 19, 18, 15))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //  --> continues with "15 16 19 17 18" below && hasTwoAdjacentSolved now - 15/16

        //(15 19 17 16 18) - TestCube20-2 
        else if (checkPieceMatches(pieceOrder, 15, 19, 17, 16, 18))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } // --> continues with "15 16 19 17 18" below && hasTwoAdjacentSolved now - 15/16

        //(18 15 16 19 17)
        else if (checkPieceMatches(pieceOrder, 18, 15, 16, 19, 17))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "15 16 18 19 17" below && hasTwoAdjacentSolved now - 15/16

        //(16 18 15 19 17)
        else if (checkPieceMatches(pieceOrder, 16, 18, 15, 19, 17))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "15 16 18 19 17" below && hasTwoAdjacentSolved now - 15/16

        //(15 19 16 18 17)
        else if (checkPieceMatches(pieceOrder, 15, 19, 16, 18, 17))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "15 16 18 19 17" below && hasTwoAdjacentSolved now - 15/16

        //TestCube25-2 (17 18 16 19 15)
        else if (checkPieceMatches(pieceOrder, 17, 18, 16, 19, 15))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //  --> continues with "18 16 17 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(15 18 17 19 16) - TestCube22-3 
        else if (checkPieceMatches(pieceOrder, 15, 18, 17, 19, 16))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } // --> continues with "18 16 17 19 15" below && hasTwoAdjacentSolved now - 16/17
        
        //(18 19 17 15 16)
        else if (checkPieceMatches(pieceOrder, 18, 19, 17, 15, 16))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } // --> continues with "19 16 17 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(17 19 16 15 18)
        else if (checkPieceMatches(pieceOrder, 17, 19, 16, 15, 18))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "19 16 17 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(18 16 15 17 19)
        else if (checkPieceMatches(pieceOrder, 18, 16, 15, 17, 19))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "18 16 17 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(19 16 18 17 15)
        else if (checkPieceMatches(pieceOrder, 19, 16, 18, 17, 15))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "19 16 17 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(18 17 19 16 15)
        else if (checkPieceMatches(pieceOrder, 18, 17, 19, 16, 15))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "18 16 17 19 15" below && hasTwoAdjacentSolved now - 16/17
        //(18 16 19 15 17)
        else if (checkPieceMatches(pieceOrder, 18, 16, 19, 15, 17))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "18 16 17 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(19 16 15 18 17)
        else if (checkPieceMatches(pieceOrder, 19, 16, 15, 18, 17))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "19 16 17 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(19 17 15 16 18)
        else if (checkPieceMatches(pieceOrder, 19, 17, 15, 16, 18))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "19 16 17 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(18 19 16 17 15) - TT16-2
        else if (checkPieceMatches(pieceOrder, 18, 19, 16, 17, 15))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);       
        } //continues with --> "18 16 17 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(19 17 18 15 16)
        else if (checkPieceMatches(pieceOrder, 19, 17, 18, 15, 16))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with  --> "19 15 17 18 16" below && hasTwoAdjacentSolved now - 17/18

        //(16 19 18 15 17)
        else if (checkPieceMatches(pieceOrder, 16, 19, 18, 15, 17))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } // continues with  --> "16 19 17 18 15" below && hasTwoAdjacentSolved now - 17/18

        //(16 18 19 17 15)
        else if (checkPieceMatches(pieceOrder, 16, 18, 19, 17, 15))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "16 19 17 18 15" below && hasTwoAdjacentSolved now - 17/18

        //(16 15 17 19 18)
        else if (checkPieceMatches(pieceOrder, 16, 15, 17, 19, 18))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "19 15 17 18 16" below && hasTwoAdjacentSolved now - 17/18

        //(19 17 16 18 15)
        else if (checkPieceMatches(pieceOrder, 19, 17, 16, 18, 15))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "16 19 17 18 15" below && hasTwoAdjacentSolved now - 17/18

        //(19 15 18 16 17)
        else if (checkPieceMatches(pieceOrder, 19, 15, 18, 16, 17))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "19 15 17 18 16" below && hasTwoAdjacentSolved now - 17/18

        //(18 15 17 16 19)
        else if (checkPieceMatches(pieceOrder, 18, 15, 17, 16, 19))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "19 15 17 18 16" below && hasTwoAdjacentSolved now - 17/18

        //(16 19 15 17 18)
        else if (checkPieceMatches(pieceOrder, 16, 19, 15, 17, 18))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "16 19 17 18 15" below && hasTwoAdjacentSolved now - 17/18

        //(17 15 19 16 18)
        else if (checkPieceMatches(pieceOrder, 17, 15, 19, 16, 18))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "17 15 16 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(19 15 16 17 18) (all off by 1)
        else if (checkPieceMatches(pieceOrder, 19, 15, 16, 17, 18))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "17 15 16 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(16 17 18 19 15) (all off by 1 in other direction)
        else if (checkPieceMatches(pieceOrder, 16, 17, 18, 19, 15))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "16 17 15 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(16 17 19 15 18)
        else if (checkPieceMatches(pieceOrder, 16, 17, 19, 15, 18))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "16 17 15 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(17 19 15 18 16)
        else if (checkPieceMatches(pieceOrder, 17, 19, 15, 18, 16))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "16 17 15 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(17 16 18 15 19)
        else if (checkPieceMatches(pieceOrder, 17, 16, 18, 15, 19))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "17 15 16 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(17 15 18 19 16) 
        else if (checkPieceMatches(pieceOrder, 17, 15, 18, 19, 16))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "17 15 16 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(16 18 17 15 19)
        else if (checkPieceMatches(pieceOrder, 16, 18, 17, 15, 19))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "16 17 15 18 19" below && hasTwoAdjacentSolved now - 18/19

        //(17 18 15 16 19) swap 1&3+2&4
        else if (checkPieceMatches(pieceOrder, 17, 18, 15, 16, 19))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "17 15 16 18 19" below && hasTwoAdjacentSolved now - 18/19
                
        //(16 15 18 17 19)
        else if (checkPieceMatches(pieceOrder, 16, 15, 18, 17, 19))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);            
        } //continues with --> "16 17 15 18 19" below && hasTwoAdjacentSolved now - 18/19 

        //(19 18 16 15 17)
        else if (checkPieceMatches(pieceOrder, 19, 18, 16, 15, 17))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "15 18 16 17 19" below && hasTwoAdjacentSolved now - 19/15

        //(15 17 19 18 16)
        else if (checkPieceMatches(pieceOrder, 15, 17, 19, 18, 16))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        } //continues with --> "15 17 18 16 19" below && hasTwoAdjacentSolved now - 19/15

        //(15 17 16 19 18)
        else if (checkPieceMatches(pieceOrder, 15, 17, 16, 19, 18))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "15 17 18 16 19" below && hasTwoAdjacentSolved now - 19/15

        //(17 19 18 16 15)
        else if (checkPieceMatches(pieceOrder, 17, 19, 18, 16, 15))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        } //continues with --> "15 17 18 16 19" below && hasTwoAdjacentSolved now - 19/15

    haveTwoAdjacentSolved:
        //FINAL STAGE: Two adjacent pieces. One bulk move left till solved. (10 cases)
        //([15 16] 18 19 17) - 15/16 ok
        if (checkPieceMatches(pieceOrder, 15, 16, 18, 19, 17))
        {
            int safeStart = 0;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        }
        //([15 16] 19 17 18) - 15/16 ok
        else if (checkPieceMatches(pieceOrder, 15, 16, 19, 17, 18))
        {
            int safeStart = 0; safeStart = 5;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        }
        //(18 [16 17] 19 15) - 16/17 ok
        else if (checkPieceMatches(pieceOrder, 18, 16, 17, 19, 15))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        }
        //(19 [16 17] 15 18) - 16/17 ok
        else if (checkPieceMatches(pieceOrder, 19, 16, 17, 15, 18))
        {
            int safeStart = 1;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        }
        //(16 19 [17 18] 15) - 17/18 ok
        else if (checkPieceMatches(pieceOrder, 16, 19, 17, 18, 15))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        }
        //(19 15 [17 18] 16) - 17/18 ok
        else if (checkPieceMatches(pieceOrder, 19, 15, 17, 18, 16))
        {
            int safeStart = 2;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        }
        //(16 17 15 [18 19]) - 18/19 ok
        else if (checkPieceMatches(pieceOrder, 16, 17, 15, 18, 19))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];    //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        }
        //(17 15 16 [18 19]) - 18/19 ok
        else if (checkPieceMatches(pieceOrder, 17, 15, 16, 18, 19))
        {
            int safeStart = 3;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        }
        //(15] 17 18 16 [19) - 19/15 ok
        else if (checkPieceMatches(pieceOrder, 15, 17, 18, 16, 19))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart];        //Algo#7 Cycle- (CCW)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[7].algo, loc);
        }
        //(15] 18 16 17 [19) - 19/15 ok
        else if (checkPieceMatches(pieceOrder, 15, 18, 16, 17, 19))
        {
            int safeStart = 4;
            colordirs loc = g_faceNeighbors[LIGHT_BLUE + safeStart - 1];    //Algo#27 Cycle+ (Clockwise)
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc);
        }

//DO IT:
        bulkShadowRotate(shadowDom, bulk);

startColorFlippingCorners:
        //Do R'DR'rdr to color-flip each corner. (Dirties the lower rows) Each piece flip takes 2x, and each line resets at 6x.
        if (fullySolvedOrder && offby == 0 && g_dirtyCountRDRD != 6 && !currentpieceFlipStatusOK) {
            //only do this if the piece is mis-colored.
            int offset = (i - 15) - 1;
            if (offset == -1) offset = 4;
            const int target = LIGHT_BLUE + offset;               
            g_dirtyBitRDRD = 1;
            g_dirtyFaceRDRD = g_dirtyFaceRDRD ? g_dirtyFaceRDRD : target;
            //rotateOffset is how we rotate gray top to move unsolved piece back over top of our dirty face
            int rotateOffset = (target - g_dirtyFaceRDRD);
            if (g_dirtyCountRDRD != 0)
                shadowDom->shadowMultiRotate(GRAY, rotateOffset);
            bulk = shadowDom->ParseAlgorithmString(g_AlgoStrings[5].algo, g_faceNeighbors[g_dirtyFaceRDRD]); //algo #5 "R' DR' r dr"
            bulkShadowRotate(shadowDom, bulk);
            bulkShadowRotate(shadowDom, bulk);
            rotateOffset *= -1;
            if (g_dirtyCountRDRD != 0 )
                shadowDom->shadowMultiRotate(GRAY, rotateOffset);
            g_dirtyCountRDRD += 2;
            bulk = {};
        }
        else if (g_dirtyBitRDRD == 1 && g_dirtyCountRDRD == 6)
        {   //clear bits.
            g_dirtyBitRDRD = 0;
            g_dirtyFaceRDRD = 0;
            g_dirtyCountRDRD = 0;
        }

        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real queue    
    updateRotateQueueWithShadow(shadowDom);
}

void Megaminx::testingAlgostrings(Megaminx* shadowDom)
{
    if (!shadowDom) {
        shadowDom = new Megaminx();
        //needs the this* when called without first initializing with MakeShadowCubeClone().
        shadowDom->LoadNewEdgesFromOtherCube(this);
        shadowDom->LoadNewCornersFromOtherCube(this);
    }
    //for (; ;)   //call one at a time for now
    int algo = 12;
    {
        AlgoString a = g_AlgoStrings[algo];
        colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //front-face (adjustable)
        int repeat = (a.repeatX != NULL) ? a.repeatX : 1;
        for (int n = 0; n < repeat; ++n) {
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString(a.algo, loc);
            bulkShadowRotate(shadowDom, bulk);
        }
        //find where pieces actually are vs. where they're supposed to be
        std::vector<int> foundEdges(5);
        auto defaultEdges = faces[GRAY - 1].defaultEdges;
        for (int k = 0; k < 5; ++k) {
            const auto &piece = shadowDom->faces[GRAY - 1].getFacePiece<Edge>(k);
            foundEdges[k] = piece->data.pieceNum;
        }
        //output the difference as an int.
        std::vector<int> newDifference(5);
        for (int k = 0; k < 5; ++k)
            newDifference[k] = foundEdges[k] - defaultEdges[k];
        int printfbreakpoint = 0;   //break on this and inspect.
    }
    updateRotateQueueWithShadow(shadowDom); //actually output the algo's changes to main cube's GUI
}
