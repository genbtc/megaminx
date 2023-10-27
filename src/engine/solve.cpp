#include "megaminx.hpp"

//the most flexible
void Megaminx::DetectSolvedEdgesUnOrdered(int startI, bool piecesSolved[5])
{
    std::vector<int> piecesSeenOnTop;
    //populate piecesSolved
    int numSolved = 0;
    const int endI = startI + 5;
    //Check if any pieces are already in their assigned slots:
    for (int p = startI; p < endI; ++p) {
        const int pIndex = findEdge(p);
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
            const int pNext = (p != (endI - 1)) ? p + 1 : startI; //handle the loop numbering boundary overrun
            const int pIndex = findEdge(p);
            const int pNextIndex = findEdge(pNext);
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

//Function call to find out if the puzzle is fully solved.
bool Megaminx::isFullySolved()
{
    return (DetectIfAllSolved<Edge>() && DetectIfAllSolved<Corner>());
}

//Fastest way to detect if the puzzle is solved.
//Can be const but other refactors block,getPieceArray<T> cant be const
// so pieceData cant be flipped or swapped in the Reset function and shadow function
template <typename T>
bool Megaminx::DetectIfAllSolved()
{
    int allSolved = 0;
    const Piece* piece = getPieceArray<T>(0);
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
    const int endI = startI + 5;
    //Find out if any applicable startI-endI pieces are exactly in their slots.
    for (int p = startI; p < endI; ++p) {
        const auto pIndex = findPiece<T>(p);
        const auto piece = getPieceArray<T>(pIndex);
        //make sure its startI-endI and make sure the colors arent flipped
        if (pIndex >= startI && pIndex < endI && p == pIndex && piece->data.flipStatus == 0)
            piecesSolved[p - startI] = true;
    }
} //where T = Corner or Edge
void Megaminx::DetectSolvedCorners(int startI, bool piecesSolved[5]) { DetectSolvedPieces<Corner>(startI, &piecesSolved[0]); }
void Megaminx::DetectSolvedEdges(int startI, bool piecesSolved[5]) { DetectSolvedPieces<Edge>(startI, &piecesSolved[0]); }

class EdgeLayerAssist {
public:
    int sourceEdgeIndex;
    colorpiece edgeFaceNeighbors;
    int edgeFaceLocA, edgeFaceLocB;
    int dirToWhiteA, dirToWhiteB;
    const Edge *EdgeItselfA, *EdgeItselfB;
    int whichcolorEdgeA, whichcolorEdgeB;
    int edgeHalfColorA, edgeHalfColorB;
    bool colormatchA, colormatchB;
    bool isOnRow1, isOnRow2, isOnRow3, isOnRow34;
    bool isOnRow4, isOnRow5, isOnRow6;
    bool ontopHalfA, ontopHalfB;
    bool graymatchA, graymatchB;
    bool isRight, isLeft;
    const int row1 = 0, row2 = 5, row3 = 10, row4 = 15, row5 = 20, row6 = 25, rowLAST = 30;

    EdgeLayerAssist(Megaminx* shadowDom, int i) {
        sourceEdgeIndex = shadowDom->findEdge(i);
        //Determine which two faces the edge belongs to:
        edgeFaceNeighbors = g_edgePiecesColors[sourceEdgeIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the edges are positioned, from 0-4
        edgeFaceLocA = shadowDom->faces[edgeFaceNeighbors.a - 1].find5EdgePresent(i);
        assert(edgeFaceLocA != -1); //(-1 for fail, not found)
        edgeFaceLocB = shadowDom->faces[edgeFaceNeighbors.b - 1].find5EdgePresent(i);
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
        graymatchA = edgeFaceNeighbors.a == GRAY;
        graymatchB = edgeFaceNeighbors.b == GRAY;
        //Row Checks, reference based on horizontal parallel line
        isOnRow1 = (sourceEdgeIndex >= row1 && sourceEdgeIndex < row2);
        isOnRow2 = (sourceEdgeIndex >= row2 && sourceEdgeIndex < row3);
        isOnRow3 = (sourceEdgeIndex >= row3 && sourceEdgeIndex < row4);
        isOnRow34 = (sourceEdgeIndex >= row3 && sourceEdgeIndex < row5); //Middle W
        isOnRow4 = (sourceEdgeIndex >= row4 && sourceEdgeIndex < row5);
        isOnRow5 = (sourceEdgeIndex >= row5 && sourceEdgeIndex < row6);
        isOnRow6 = (sourceEdgeIndex >= row6 && sourceEdgeIndex < rowLAST);
        //Check if the faces we have are considered as the Top half of the cube.
        //Any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        ontopHalfA = (edgeFaceNeighbors.a > WHITE && edgeFaceNeighbors.a < GRAY);
        ontopHalfB = (edgeFaceNeighbors.b > WHITE && edgeFaceNeighbors.b < GRAY);
        //Line up things that are solved
        //drop-in directions: left is row 4, right is row 3 (determined from original I, not sourceEdgeIndex)
        isRight = (i >= row3 && i < row4);
        isLeft = (i >= row4 && i < row5);
    }
};

class CornerLayerAssist {
public:
    int sourceCornerIndex;
    colorpiece cornerFaceNeighbors;
    int cornerFaceLocA, cornerFaceLocB, cornerFaceLocC;
    const Corner *CornerItselfA;
    const int row1 = 0, row2 = 5, row3 = 10, row4 = 15, rowLAST = 20;
    bool isOnRow1, isOnRow2, isOnRow3, isOnRow4;
    bool ontopHalfA, ontopHalfB, ontopHalfC;

    CornerLayerAssist(Megaminx* shadowDom, int i) {
        sourceCornerIndex = shadowDom->findCorner(i);
        //Determine which two faces the corner belongs to:
        cornerFaceNeighbors = g_cornerPiecesColors[sourceCornerIndex];
        //Find everything and get it moved over to its drop-in point:
        //Determine where on those faces the corners are positioned, 0-4
        cornerFaceLocA = shadowDom->faces[cornerFaceNeighbors.a - 1].find5CornerPresent(i);
        assert(cornerFaceLocA != -1); //(-1 for fail, not found)
        cornerFaceLocB = shadowDom->faces[cornerFaceNeighbors.b - 1].find5CornerPresent(i);
        assert(cornerFaceLocB != -1); //should not happen
        cornerFaceLocC = shadowDom->faces[cornerFaceNeighbors.c - 1].find5CornerPresent(i);
        assert(cornerFaceLocC != -1); //should not happen
        CornerItselfA = shadowDom->faces[cornerFaceNeighbors.a - 1].corner[cornerFaceLocA];
        //Row Checks, Horizontal lines that dictate position relative to the origin
        isOnRow1 = (sourceCornerIndex >= row1 && sourceCornerIndex < row2);
        isOnRow2 = (sourceCornerIndex >= row2 && sourceCornerIndex < row3);
        isOnRow3 = (sourceCornerIndex >= row3 && sourceCornerIndex < row4);
        isOnRow4 = (sourceCornerIndex >= row4 && sourceCornerIndex < rowLAST);
        //Check if the faces we have are considered as the Top half of the cube.
        //Any matching pieces that end up on its matching face can be spun in 2 moves or 1.
        ontopHalfA = (cornerFaceNeighbors.a > WHITE && cornerFaceNeighbors.a < GRAY);
        ontopHalfB = (cornerFaceNeighbors.b > WHITE && cornerFaceNeighbors.b < GRAY);
        ontopHalfC = (cornerFaceNeighbors.c > WHITE && cornerFaceNeighbors.c < GRAY);
    }
};

//Layer 1 part 1 - White Edges ( )
void Megaminx::rotateSolveWhiteEdges(Megaminx* shadowDom)
{
    int i = 0;
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 0;
    const int endingPiece = startingPiece + 5;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        //initialize arrays to hold pieces that are already solved
        bool facesSolved[numFaces] = { false };
        bool piecesSolved[5] = { false };
        //Start detecting what pieces are solved
        shadowDom->DetectSolvedEdgesUnOrdered(startingPiece, piecesSolved);
        bool areEdgesFullySolved[5] = { false };
        shadowDom->DetectSolvedEdges(startingPiece, &areEdgesFullySolved[0]);
        int firstSolvedPiece = -1;
        //check solved-ness
        for (int a = 0; a < 5; ++a) {
            facesSolved[1 + a] = piecesSolved[a];
            if (firstSolvedPiece == -1 && piecesSolved[a])
                firstSolvedPiece = a;
        }
        //if its solved early <5, increase loop to continue to next piece
        while (i < endingPiece && areEdgesFullySolved[i])
            i++;
        //if everything is solved, its done
        const auto allSolved1 = std::all_of(std::begin(piecesSolved), std::end(piecesSolved), [](bool j) { return j; });
        const auto allSolved2 = std::all_of(std::begin(areEdgesFullySolved), std::end(areEdgesFullySolved), [](bool j) { return j; }); //TT88-2
        //entirely solved, done
        if (i >= endingPiece && allSolved1 && allSolved2) {
            allSolved = true;   //TT12-huh
            break;  //goto end
        }
        //means we got to the end of the first pass,
        // but something else came undone, go back and fix it, one more pass.
        else if (i >= endingPiece) {
            std::cout << "Debug L1E-1, loop iterated i past end but still unsolved, BUG? \n";
            i = startingPiece;
            continue;
        }
        //data storage types for current state
        const EdgeLayerAssist l{ shadowDom, i };
        assert(l.colormatchA != l.colormatchB); //sanity check.

        //Start manipulating the cube: Restore White Piece 0
        //Rotates the white face to its solved position, first solved edge matches up to its face.
        if (firstSolvedPiece != -1) {
            //NOTE: Doing this over and over is wasting moves solving the partial-solved top every time.
            //TODO: This means we need a plan for any 5-9 edge to get moved into any 0-4 slot even when top isn't solved.
            const int findIfPieceSolved = shadowDom->findEdge(firstSolvedPiece); //always piece 0
            int offby = findIfPieceSolved - firstSolvedPiece;    //example: piece 0, 5 - 5 - 0 = 0, so no correction.
            if ((findIfPieceSolved > 0) && (findIfPieceSolved < 5) && (offby > 0)) {
                offby *= -1;
                shadowDom->shadowMultiRotate(WHITE, offby);
                //this resets our starting point reset loop too.
                continue;
            }
        }
        //Any matching pieces that end up on its matching face can be spun in with just 2 or 1 moves.
        if (l.ontopHalfA && ((l.isOnRow34 && l.colormatchA) || l.isOnRow2)) {
            int fastspin = l.colormatchA ? (l.edgeFaceNeighbors.a - l.edgeHalfColorA)
                                         : (l.edgeFaceNeighbors.b - l.edgeHalfColorB);
            //Set up Rotated White top to be in-line with the face we want to spin in.
            shadowDom->shadowMultiRotate(WHITE, fastspin);
            //rotate pieces in (yolo, state was changed)
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
            //REFRESH (nested state)
            const int sourceEdgeIndexNext = shadowDom->findEdge(i);
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            const colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == l.edgeFaceNeighbors.b)
                //check if the next neighbor face is Solved aka blocked ?
                if (facesSolved[edgeFaceNeighborsNext.a - 1])
                    //make sure its the move going to the right
                    if (edgeFaceNeighborsNext.a == g_faceNeighbors[l.edgeFaceNeighbors.a].right)
                        shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        else if (l.isOnRow5 || l.isOnRow6 && l.dirToWhiteA != 0)
            shadowDom->shadowRotate(l.edgeFaceNeighbors.a, l.dirToWhiteA);
        else if (l.isOnRow5 || l.isOnRow6 && l.dirToWhiteB != 0)
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
    } while (!allSolved);
    //If its solved, get top white face spun oriented back to normal    //redundant
    if (allSolved) {
        int findIfPieceSolved = shadowDom->findEdge(startingPiece); //always piece 0
        if (findIfPieceSolved > 0 && findIfPieceSolved < 5) {
            findIfPieceSolved *= -1;
            shadowDom->shadowMultiRotate(WHITE, findIfPieceSolved);
        }
    }
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolveWhiteEdges 1 1 in " << loopcount << " loops" << std::endl;
}

//Layer 1 part 2 - White Corners ( #5, #0 )
void Megaminx::rotateSolveWhiteCorners(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 0;
    const int endingPiece = startingPiece + 5;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        //Check solved-ness
        shadowDom->DetectSolvedCorners(startingPiece, piecesSolved);
        int i = startingPiece;
        //solved pieces can be skipped
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            continue;   //goto end.
        }
        //data storage types for current state
        const CornerLayerAssist l{ shadowDom, i };
        std::vector<numdir> bulkAlgo;

        //Rotates the white face to its solved position, first solved EDGE matches up to its face.
        //Edges should already be solved, if not, get top white face spun oriented back to normal
        int edgesOffBy = shadowDom->findEdge(startingPiece); //always piece 0
        if (edgesOffBy > 0 && edgesOffBy < 5) {
            edgesOffBy *= -1;
            shadowDom->shadowMultiRotate(WHITE, edgesOffBy);
        }
        //Move incorrect corners out of the 0-4 slots moves them right down with the algo
        else if (l.isOnRow1 && ((i != l.sourceCornerIndex) || (i == l.sourceCornerIndex && l.CornerItselfA->data.flipStatus != 0))) {
            int offby = RED;
            if (l.cornerFaceNeighbors.a == WHITE)
                offby += l.cornerFaceLocA;
            else if (l.cornerFaceNeighbors.b == WHITE)
                offby += l.cornerFaceLocB;
            else if (l.cornerFaceNeighbors.c == WHITE)
                offby += l.cornerFaceLocC;
            if (offby > YELLOW)
                offby -= 5;
            int aoffby = l.sourceCornerIndex + RED;
            MMMg(aoffby,YELLOW);
            assert(offby == aoffby);
            if (offby >= 2) {
                bulkAlgo = shadowDom->ParseAlgorithmID(50, offby);      //    .algo = "R' DR' r dr"
                shadowDom->shadowBulkRotate(bulkAlgo);
            }
            assert(offby >= 2);   //temporary testing for confidence
            std::cout << "Debug L1C -row1 " << i << " sourcecornerindex: "  << l.sourceCornerIndex << " offby: " << offby << "\n";
        }
        //Move correct corners straight up and in from 5-9 to 0-4
        else if (l.isOnRow2 && l.sourceCornerIndex - i - endingPiece == 0) {
            int offby =  l.sourceCornerIndex - endingPiece + RED;
            if (offby > YELLOW)
                offby -= 5;
            int aoffby = i + RED;
            MMMg(aoffby,YELLOW);
            assert(offby == aoffby);
            if (offby >= 2) {
                if (l.CornerItselfA->data.flipStatus == 2)
                    bulkAlgo = shadowDom->ParseAlgorithmID(51, offby);  //    .algo = "f dr F' "
                else
                    bulkAlgo = shadowDom->ParseAlgorithmID(52, offby);  //    .algo = "R' DR' R"
                shadowDom->shadowBulkRotate(bulkAlgo);
            }
            assert(offby >= 2);   //DEV: temporary testing for confidence
            std::cout << "Debug L1C -row2 + matched index " << i << " sourcecornerindex: "  << l.sourceCornerIndex << " offby: " << offby << "\n";
        }
        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        else if (l.isOnRow2) {
            const int defaultDir = Face::CW;
            megaminxColor turnface=BLACK;
            if (l.ontopHalfA && l.ontopHalfB)
                turnface = l.cornerFaceNeighbors.c;
            else if (l.ontopHalfA && l.ontopHalfC)
                turnface = l.cornerFaceNeighbors.b;
            else if (l.ontopHalfB && l.ontopHalfC)
                turnface = l.cornerFaceNeighbors.a;
            // turn opposite direction row2->row3->row4 to use gray face then repeat
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
            // after 2nd rotation leaves them on the gray face.
            std::cout << "Debug L1C -row2 " << i << " sourcecornerindex: "  << l.sourceCornerIndex << " turnface: " << turnface << " offby: " << defaultDir << "\n";
        }
        //Row 3 pieces go to gray face as temporary holding (1 CW turn) (ends up on row4)
        else if (l.isOnRow3) {
            const int defaultDir = Face::CW;
            int x, y = 0;
            if (l.ontopHalfA) {
                x = l.cornerFaceNeighbors.b;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopHalfB) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopHalfC) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.b;
            }
            int facex = std::max(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                facex = LIGHT_BLUE;
            shadowDom->shadowRotate(facex, defaultDir);
            std::cout << "Debug L1C -row3 " << i << " sourcecornerindex: "  << l.sourceCornerIndex << " face: " << facex << " offby: " << defaultDir << "\n";
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2
        else if (l.isOnRow4) {
            int defaultDir = Face::CW * 2;
            int offby = l.sourceCornerIndex - i - 18;
            shadowDom->shadowMultiRotate(GRAY, offby);
            int facex = PINK + i;
            MMMg(facex, BEIGE);
            shadowDom->shadowMultiRotate(facex, defaultDir);
            std::cout << "Debug L1C -row4 " << i << " sourcecornerindex: "  << l.sourceCornerIndex << " face: " << facex << " offby: " << defaultDir << "\n";
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolveWhiteCorners 1 2 in " << loopcount << " loops" << std::endl;
}

//Layer 2 - Edges
void Megaminx::rotateSolveLayer2Edges(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 5;
    const int endingPiece = startingPiece + 5;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        //check solved-ness
        shadowDom->DetectSolvedEdges(startingPiece, piecesSolved);
        int i = startingPiece;
        //solved pieces can be skipped
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        //data storage types for current state
        const EdgeLayerAssist l{ shadowDom,i };
        std::vector<numdir> bulkAlgo;

        if ((l.isOnRow2 && (l.sourceEdgeIndex != i || (l.sourceEdgeIndex == i && l.EdgeItselfA->data.flipStatus != 0))) ||
            (l.isOnRow34 && l.ontopHalfA && l.edgeFaceLocA == 4 && l.edgeFaceNeighbors.a == l.edgeHalfColorA)) {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[1].algo, g_faceNeighbors[l.edgeFaceNeighbors.a], 1);    // "dl l dl L', dL' F' dL' f"
            shadowDom->shadowBulkRotate(bulkAlgo);
        }
        else if (l.isOnRow34 && l.ontopHalfA && l.edgeFaceLocA == 3 && l.edgeFaceNeighbors.a == l.edgeHalfColorA) {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[2].algo, g_faceNeighbors[l.edgeFaceNeighbors.a], 2);    //"dR' R' dR' r, dr f dr F' "
            shadowDom->shadowBulkRotate(bulkAlgo);
        }
        //BUG?: Pieces still go the long-way around from 7->6->3->(skip)4->6->7->6->4 then algo....
        //TODO: Need to detect when the face matches up then go backwards.
        else if (l.isOnRow34 && l.dirToWhiteB != 0 && l.edgeFaceNeighbors.b >= GRAY) {
            shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
            const int sourceEdgeIndexNext = shadowDom->findEdge(i);   //REFRESH
            //Moves a piece again from Row 4 to Row 6. (if the face it lands on is locked).
            //Determine which two faces the edge belongs to
            const colorpiece edgeFaceNeighborsNext = g_edgePiecesColors[sourceEdgeIndexNext];
            //we know the next piece has the same neighbor
            if (edgeFaceNeighborsNext.b == l.edgeFaceNeighbors.b)
                //check the solved for the next neighbor (vs what if we just check the original, face.loc.right) ?
                //make sure its the move going to the right
                if (edgeFaceNeighborsNext.a == g_faceNeighbors[l.edgeFaceNeighbors.a].right)
                    shadowDom->shadowRotate(l.edgeFaceNeighbors.b, l.dirToWhiteB);
        }
        //Layer 6 pieces are moved down to gray layer 7
        else if (l.isOnRow5) {
            if (l.dirToWhiteA != 0)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.a, -1 * l.dirToWhiteA);
            else if (l.dirToWhiteB != 0)
                shadowDom->shadowRotate(l.edgeFaceNeighbors.b, -1 * l.dirToWhiteB);
        }
        //gray Layer 7 acts as a temporary storage for pieces
        else if (l.isOnRow6) {
            int row7 = l.sourceEdgeIndex - 28 - i;
            const bool moved = shadowDom->shadowMultiRotate(GRAY, row7);
            //Align the GRAY layer 7 to be directly underneath the intended solve area
            if (!moved) {
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
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolveLayer2Edges 2 in " << loopcount << " loops" << std::endl;
}

//Layer 3 - Corners ( )
void Megaminx::rotateSolve3rdLayerCorners(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 5;
    const int endingPiece = startingPiece + 5;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        //check solved-ness
        shadowDom->DetectSolvedCorners(startingPiece, piecesSolved);
        int i = startingPiece;
        //solved pieces can be skipped
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        //data storage types for current state
        const CornerLayerAssist l{ shadowDom, i };

        //Row 2 pieces go to gray face as temporary holding (2-CW turns) (ends up on row4)
        if (l.isOnRow2) {
            int defaultDir = Face::CW;
            megaminxColor turnface=BLACK;
            if (l.ontopHalfA && l.ontopHalfB)
                turnface = l.cornerFaceNeighbors.c;
            else if (l.ontopHalfA && l.ontopHalfC)
                turnface = l.cornerFaceNeighbors.b;
            else if (l.ontopHalfB && l.ontopHalfC)
                turnface = l.cornerFaceNeighbors.a;
            // if the pieces position is solved, but color flipped wrong:
            if (l.sourceCornerIndex == i)
                defaultDir *= -1;
            // turn opposite direction row2->row3->row4 to use gray face then repeat
            shadowDom->shadowRotate(turnface, defaultDir);
            shadowDom->shadowRotate(turnface, defaultDir);
            // after 2nd rotation leaves them on the gray face.
        }
        //Row 3 pieces go to gray face as temporary holding (1 CCW turn) (ends up on row4)
        else if (l.isOnRow3) {
            const int defaultDir = Face::CCW;
            megaminxColor x, y = BLACK;
            if (l.ontopHalfA) {
                x = l.cornerFaceNeighbors.b;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopHalfB) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopHalfC) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.b;
            }
            //The loop point crosses over at the Pink
            int result = std::max(x, y);
            MMMg(result, BEIGE);
            shadowDom->shadowRotate(result, defaultDir);
        }
        //Get the Pieces off Row 4 (gray layer) and onto row 2 Solved
        else if (l.isOnRow4) {
            //locate our source piece
            int offby = l.sourceCornerIndex - i - 18;
            //get it out of row4 by orienting top.
            shadowDom->shadowMultiRotate(GRAY, offby);
            //locate where it went
            int x = PINK + i;
            MMMg(x, BEIGE);
            //get it onto row2 solved
            int defaultDir = Face::CW * 2;
            shadowDom->shadowMultiRotate(x, defaultDir);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolve3rdLayerCorners 3 in " << loopcount << " loops" << std::endl;
}

//Layer 4 - Edges ( #35, #23, #24 , #34 )
//Cube must have gray side on top, layer 1+2+3 Solved and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer4Edges(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 10;
    const int middlePiece = startingPiece + 5; //second row
    const int endingPiece = startingPiece + 10; //two rows end
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[10] = { false };
        //check solved-ness
        shadowDom->DetectSolvedEdges(startingPiece, piecesSolved);
        shadowDom->DetectSolvedEdges(middlePiece, &piecesSolved[5]);
        int i = startingPiece; //the piece
        //solved pieces can be skipped
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        //data storage types for current state
        const EdgeLayerAssist l{ shadowDom,i };
        std::vector<numdir> bulkAlgo;

        //Get ready for algorithms
        if ((l.isOnRow34 && (l.sourceEdgeIndex != i || (l.sourceEdgeIndex == i && l.EdgeItselfA->data.flipStatus != 0)))
            || (l.isOnRow6)) {
            colordirs loc;
            if (l.isOnRow6) {
                //Align GRAY top to the exact position for pre-drop-in.
                int offby = l.sourceEdgeIndex - 27 - i;
                if (l.isLeft) //row 4B's B-half is +1 from row4A's B-half (DARK_BLUE,LIGHT_GREEN) vs (DARK_BLUE,PINK)
                    offby -= 1;
                const bool moved = shadowDom->shadowMultiRotate(GRAY, offby);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
                loc = (l.graymatchA) ? g_faceNeighbors[l.edgeFaceNeighbors.b] : g_faceNeighbors[l.edgeFaceNeighbors.a];
            }
            //obtain the non-gray face neighbor we need to be rotating
            else if (l.isOnRow34)
                loc = g_faceNeighbors[std::max(l.edgeFaceNeighbors.b, l.edgeFaceNeighbors.a)];
            //works to insert pieces from row7 to 3/4 and also pops wrong pieces out from 3/4 to 6
            if ((l.isOnRow6 && l.isLeft) || (l.isOnRow4)) {
                if (l.EdgeItselfA->data.flipStatus == 0 || (l.isOnRow4))
                    bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[3].algo, loc, 3); // {23, "F' R' F', F' r f"},
                else
                    bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[4].algo, loc, 4); // {34, "U' R' DR' F F DR R" },
            }
            else if ((l.isOnRow6 && l.isRight) || (l.isOnRow3)) {
                if (l.EdgeItselfA->data.flipStatus == 0 || (l.isOnRow3))
                    bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[6].algo, loc, 6); // {24, "f l f, f L' F' "},
                else
                    bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[5].algo, loc, 5); // {35, "u l dl F' F' DL' L' "}
            }
            shadowDom->shadowBulkRotate(bulkAlgo);
        }
        //Row 6 - flip piece up to GRAY, then return the moved faces to unharm the low corners.
        else if (l.isOnRow5) {
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
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolveLayer4Edges 4 in " << loopcount << " loops" << std::endl;
}

//Layer 5 - Corners ( #1 )
void Megaminx::rotateSolve5thLayerCorners(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 10;
    const int endingPiece = startingPiece + 5;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        //check solved-ness
        shadowDom->DetectSolvedCorners(startingPiece, piecesSolved);
        int i = startingPiece;
        //solved pieces can be skipped
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        //data storage types for current state
        const CornerLayerAssist l{ shadowDom, i };
        std::vector<numdir> bulkAlgo;

        //Get the Pieces to drop-in ready on Row 4 (gray layer) solved into Row3
        //Any Row 3 pieces that are mis-solved use same algo to go up to gray layer (ends up on row4)
        if (l.isOnRow3) {
            int x, y;
            if (l.ontopHalfA) {
                x = l.cornerFaceNeighbors.b;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopHalfB) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.c;
            }
            else if (l.ontopHalfC) {
                x = l.cornerFaceNeighbors.a;
                y = l.cornerFaceNeighbors.b;
            }
            int result = std::min(x, y);
            if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                result = BEIGE;
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[1].algo, g_faceNeighbors[result], 1); // "r u R' U' "
            shadowDom->shadowBulkRotate(bulkAlgo);
        }
        else if (l.isOnRow4) {
            //Orient Gray Top layer (index goes in reverse)
            int offby = l.sourceCornerIndex - i * -1;
            shadowDom->shadowMultiRotate(GRAY, offby);
            //quick shortcut to know which face we're working on.
            const int front = BEIGE - (i - startingPiece);
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[1].algo, g_faceNeighbors[front], 1); // "r u R' U' "
            shadowDom->shadowBulkRotate(bulkAlgo);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolve5thLayerCorners 5 in " << loopcount << " loops" << std::endl;
}

//Layer 6 - Edges ( #26, #25 )
//Cube must have gray side on top, layer 1+2+3+4+5 Solved, and rest of puzzle Unsolved
void Megaminx::rotateSolveLayer6Edges(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 20;
    const int endingPiece = startingPiece + 5;
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        shadowDom->DetectSolvedEdges(startingPiece, piecesSolved);
        int i = startingPiece;
        //solved pieces can be skipped
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        //data storage types for current state
        const EdgeLayerAssist l{ shadowDom,i };
        std::vector<numdir> bulkAlgo;

        //Get ready for algorithms
        if ((l.isOnRow5 && (l.sourceEdgeIndex != i || (l.sourceEdgeIndex == i && l.EdgeItselfA->data.flipStatus != 0)))
            || (l.isOnRow6)) {
            if (l.isOnRow6) {
                //assert(l.dirToWhiteA == 0);
                int offby = l.graymatchA ? (l.edgeFaceNeighbors.b - l.edgeHalfColorB) : (l.edgeFaceNeighbors.a - l.edgeHalfColorA);
                //Align GRAY top to the exact position for pre-drop-in.
                const bool moved = shadowDom->shadowMultiRotate(GRAY, offby);
                //Align the GRAY layer 7 to be directly underneath the intended solve area
                if (moved)
                    continue;
            }
            //obtain the non-gray face neighbor we need to be oriented to
            int result = 0;
            const int x = l.edgeFaceNeighbors.a;
            const int y = l.edgeFaceNeighbors.b;
            bool isLeft = false, isRight = false;
            colordirs loc;
            if (l.isOnRow6) {
                result = (l.graymatchA) ? y : x;
                loc = g_faceNeighbors[result]; //loc.front
                isLeft = (loc.left == l.edgeHalfColorA);
                isRight = (loc.right == l.edgeHalfColorA);
            }
            else if (l.isOnRow5) {
                result = std::min(x, y);
                if ((x == BEIGE && y == LIGHT_BLUE) || (y == BEIGE && x == LIGHT_BLUE))
                    result = BEIGE;
                loc = g_faceNeighbors[result]; //loc.front
                isRight = true;
            }
            //Check left/right faces for which direction to drop-in with isLeft/isRight
            int algo = isLeft ? 7 : isRight ? 8 : 7;
            //works to insert pieces from row7 to 6 and also pops wrong pieces out from 6 to 7
            //{7, "U' L' u l, u f U' F' "},    //{8, "u r U' R', U' F' u f "},
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStringsLayer[algo].algo, loc, algo);
            shadowDom->shadowBulkRotate(bulkAlgo);
        }
        else //unknown error occured, canary in the coalmine that somethings wrong.
            unknownloop++;
        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolveLayer6Edges 6 in " << loopcount << " loops" << std::endl;
}

//Last Layer = Layer 7 EDGES (GRAY top) ( #14, #20, #13, #14 )
//Cube must have gray side on top, layer 1+2+3+4+5+6 Solved
void Megaminx::rotateSolveLayer7Edges(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    int unknownloop = 0;
    const int startingPiece = 25;
    const int endingPiece = startingPiece + 5;
    //Loop:
    do {
        //basic overflow protection:
        if (loopcount > 101)
            break;
        bool piecesSolved[5] = { false };
        //check solved-ness, Stores piece and color status from the gray face
        shadowDom->DetectSolvedEdges(startingPiece, piecesSolved);
        int i = startingPiece;
        //Solved pieces can be skipped (Iterate past any already completely solved pieces)
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        bool grayFaceColorSolved[5] = { false };
        bool piecesSolvedMaybe[5] = { false };
        shadowDom->DetectSolvedEdgesUnOrdered(i, piecesSolvedMaybe);
        int nPiecesPresent = shadowDom->faces[GRAY-1].find5EdgePresent(i);
    //Populate the current order state:
        //std::vector<int> pieceOrder = shadowDom->faces[GRAY-1].findEdgesOrder();
        std::vector<int> pieceOrder(5);
        for (int k = 0; k < 5; ++k) {
            Edge* EdgeItselfNext = shadowDom->faces[GRAY - 1].edge[k];
            pieceOrder[k] = EdgeItselfNext->data.pieceNum;
            if (EdgeItselfNext->data.flipStatus == 0)
                grayFaceColorSolved[k] = true;
        }
        std::vector<int> pieceFoundOrder= megaminx->faces[GRAY - 1].findEdgesOrder();
        std::vector<int> grayFaceColors = megaminx->faces[GRAY - 1].findEdgesColorFlipStatus();
        bool grayFaceColorSolved2[5] = { false };
        for (int k = 0; k < 5; ++k)
            grayFaceColorSolved2[k] = (grayFaceColors.at(k) == 0);

    //iterate through the piece/color lists and make some easy compound conditional aliases as conceptual shortcuts
        //twoSolvedPieces(a,b) - Lambda for Solved pieces
        const auto twoSolvedPieces = [piecesSolved](int a, int b) { return (piecesSolved[a] && piecesSolved[b]); };
        const bool twoAdjacentPieces =
            (twoSolvedPieces(0, 1)) || (twoSolvedPieces(1, 2)) || (twoSolvedPieces(2, 3)) || (twoSolvedPieces(3, 4)) || (twoSolvedPieces(4, 0));
        const bool twoOppositePieces =
            (twoSolvedPieces(0, 2)) || (twoSolvedPieces(1, 3)) || (twoSolvedPieces(2, 4)) || (twoSolvedPieces(3, 0)) || (twoSolvedPieces(4, 1));
        //twoGraysUnsolved(a,b) - Lambda for Unsolved pieces
        const auto twoGraysUnsolved = [grayFaceColorSolved](int a, int b) { MM5(a); MM5(b); return (!grayFaceColorSolved[a] && !grayFaceColorSolved[b]); }; //MM=overrotation support
        const bool twoAdjacentOffColors =
            (twoGraysUnsolved(0, 1)) || (twoGraysUnsolved(1, 2)) || (twoGraysUnsolved(2, 3)) || (twoGraysUnsolved(3, 4)) || (twoGraysUnsolved(4, 0));
        const bool twoOppositeOffColors =
            (twoGraysUnsolved(0, 2)) || (twoGraysUnsolved(1, 3)) || (twoGraysUnsolved(2, 4)) || (twoGraysUnsolved(3, 0)) || (twoGraysUnsolved(4, 1));
        const auto matchesOppoColor = [twoGraysUnsolved](int face) {
            face -= LIGHT_BLUE;
            return twoGraysUnsolved(face + 2, face + 4);
        };
    //Asks some additional questions about what pieces/colors are concrete viable options
        int solvedCount = 0;
        int firstSolvedPiece = -1, firstUnsolvedPiece = -1;
        int lastSolvedPiece = -1, firstOffColorPiece = -1;
        for (int k = 0; k < 5; ++k) {
            if (piecesSolved[k]) {
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

    //Set up important piece variables
        //data storage types for current state (DEV: no assist?)
        std::vector<numdir> bulkAlgo;
        const int sourceEdgeIndex = shadowDom->findEdge(i);
        const int offby = sourceEdgeIndex - i;
        const bool currentpieceFlipStatusOK = grayFaceColorSolved[i - startingPiece];
        //Check gray CORNERS also, "maybe" we want to preserve them.
        bool grayCornerColorSolved[5] = { false };
        shadowDom->DetectSolvedCorners(15, grayCornerColorSolved);
        bool allCornersAllSolved = std::all_of(std::begin(grayCornerColorSolved), std::end(grayCornerColorSolved), [](bool j) { return j; });
        bool allEdgeColorsSolved = std::all_of(std::begin(grayFaceColorSolved),   std::end(grayFaceColorSolved),   [](bool j) { return j; });

//START MAIN:

        //orient the first piece if it exists:
        const int findIfPieceSolved = shadowDom->findEdge(startingPiece); //always piece first
        if (findIfPieceSolved > startingPiece && findIfPieceSolved < 30 && !allCornersAllSolved && !piecesSolved[0] && offby != 0 && !twoAdjacentPieces) {
            //Test 3 passes.
            int offby = findIfPieceSolved - startingPiece;
            shadowDom->shadowMultiRotate(GRAY, offby);
            continue;
        }
        //TODO: never fires. redundant to above.
        //Rotates the GRAY face to any solved position, first out of order but solved EDGE rotates to match up to its face.
        else if (!piecesSolved[0] && !twoAdjacentPieces && !allCornersAllSolved && solvedCount >= (i - startingPiece) && piecesSolvedMaybe[i - startingPiece]) {
            const int findIfPieceSolved = shadowDom->findEdge(i + firstSolvedPiece);
            int offby = findIfPieceSolved - i + firstSolvedPiece;
            if (findIfPieceSolved >= startingPiece && findIfPieceSolved < 30 && offby != 0) {
                shadowDom->shadowMultiRotate(GRAY, offby);
                continue;
            }
        }

//Begin solvedCount=0:
        //2, 2, 2, 2, 2,      = modby, Algo # 40
        //-2, -2, -2, -2, -2, = modby, Algo # 41

//5-way cycle (-2,-2,-2,-2,-2) #Algo#40
        if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 28, 29, 25, 26, 27)) //TT55-3
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[40].algo, g_faceNeighbors[LIGHT_BLUE], 40);    //algo17@Blue (60 moves to 48)
        }
//5-way cycle (+2,+2,+2,+2,+2) #Algo#41
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 28, 29, 25, 26)) //TT53-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[41].algo, g_faceNeighbors[LIGHT_BLUE], 41);    //algo38@Blue (60 moves to 48)
        }

//TODO: Combine these 5 better #Algo#39
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 29, 27, 28, 25, 26)) //TT54-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[LIGHT_BLUE], 39);   //algo18@Green (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 25, 28, 29, 26)) //TT58-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[ORANGE], 39);    //algo18@Pink (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 28, 26, 29, 25)) //TT59-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[LIGHT_GREEN], 39);  //algo18@Beige (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 26, 28, 29, 27, 25)) //TT56-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[PINK], 39);  //algo18@Blue (60 moves to 32)
        }
//5-way cycle (+1,+2,-1,+2,+1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 26, 27, 29, 25, 28)) //TT57-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[39].algo, g_faceNeighbors[BEIGE], 39); //algo18@Orange (60 moves to 32)
        }
//TODO: Combine these 5 also (.modby=2,2,1,1,-1) #Algo#43
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 28, 29, 26, 27, 25)) //TT86-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[LIGHT_BLUE], 43);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 26, 29, 25, 27, 28))
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[ORANGE], 43);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 29, 27, 25, 26, 28))
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[LIGHT_GREEN], 43);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 29, 25, 28, 26, 27))
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[PINK], 43);
        }
//5-way Reverse cycle (-1,-2,+1,-2,-1)
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 28, 25, 26, 29, 27))
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[43].algo, g_faceNeighbors[BEIGE], 43);
        }
//End solvedCount=0:

//BUNNY 4-COLOR-only (with 1 solved, covers all possibilities) #Algo#21
//#7LL: Step 1, Edge Orientation, 2&3 and 4&5, Flip Colors only (Invert 4 in place)
        else if (solvedCount == 1 && !allEdgeColorsSolved && allCornersAllSolved
             && twoGraysUnsolved(firstSolvedPiece + 1, firstSolvedPiece + 2)
             && twoGraysUnsolved(firstSolvedPiece + 3, firstSolvedPiece + 4)
        ) {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[21].algo, g_faceNeighbors[LIGHT_BLUE + firstSolvedPiece], 21);    //algo #29 Quad-Flip Colors
        }
//BUNNY 2-COLOR: (OFF-COLOR OPPOSITES) #Algo#20
//#7LL: Step 2: Edge Position, opposite swaps & flip/Invert
        //If two opposite Pieces are colored wrong, Choose the face that places them @ 8' & 1' oclock and then run Algo #20 to invert those
        else if (twoOppositeOffColors && !allEdgeColorsSolved) {
            for (int op = LIGHT_BLUE; op <= BEIGE; ++op)
                if (matchesOppoColor(op))
                    bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[20].algo, g_faceNeighbors[op], 20); //algo #20 = INVERT colors @ 8'/1'
        }
//BUNNY 2+2SWAP  #Algo#37
// 2&5 + 3&4 (opposite horizontally)
        else if (solvedCount == 1 && allEdgeColorsSolved && allCornersAllSolved &&
              (checkPieceMatches(pieceOrder, 25, 29, 28, 27, 26) //TT79-2 (25)
            || checkPieceMatches(pieceOrder, 27, 26, 25, 29, 28) //TT78-2 (26)
            || checkPieceMatches(pieceOrder, 29, 28, 27, 26, 25) //TT77-2 (27)
            || checkPieceMatches(pieceOrder, 26, 25, 29, 28, 27) //TT61-2 (28)
            || checkPieceMatches(pieceOrder, 28, 27, 26, 25, 29)) //TT46-2 (29)
        ) {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[37].algo, g_faceNeighbors[LIGHT_BLUE + firstSolvedPiece], 37); //algo #37, 13*5=65 moves
            for (int i = 0; i < g_AlgoStrings[37].repeatX; ++i)
                shadowDom->shadowBulkRotate(bulkAlgo);
        }

//BUNNY 2+2SWAP  #Algo#42=/=Algo#19
// 2&3 + 4&5 (adjacent vertically)
        else if (solvedCount == 1 &&
            // allCornersAllSolved && (//TT-69-2 TODO: when Corners arent solved we can skip more moves w/ E+C Algo.// OK?
              (checkPieceMatches(pieceOrder, 25, 27, 26, 29, 28) //TT65-2 (25)
            || checkPieceMatches(pieceOrder, 29, 26, 28, 27, 25) //TT66-2 (26)
            || checkPieceMatches(pieceOrder, 26, 25, 27, 29, 28) //TT67-2 (27)
            || checkPieceMatches(pieceOrder, 29, 27, 26, 28, 25) //TT64-2 (28)
            || checkPieceMatches(pieceOrder, 26, 25, 28, 27, 29)) //TT68-2 (29)
        ) {
            //Streamlined previous algo19 from 44 moves to 32 : (in doing that, did we go backwards with something)
            int start = LIGHT_BLUE + firstSolvedPiece + 1;
            MMMg(start, BEIGE);
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[42].algo, g_faceNeighbors[start], 42); //algo#42=algo#19
        }

//MUSHROOM- #Algo#14 (all) 3a- (F/L Safe)
        //twoAdjacentOffColors #Algo14#  -  then restart loop and pick up at mushroom to finish
        else if (twoAdjacentOffColors) //TT-85-2,  //TT63-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece], 14);  //algo #14 3a-  (F/L Safe)
        }
        //Not Needed Start
        else if (offby == 0 && solvedCount == 3 && !allEdgeColorsSolved && (twoGraysUnsolved(1, 2) || twoGraysUnsolved(2, 3))) //TT62-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece], 14);  //algo #14 3a-  (F/L Safe)
            assert(0);
        }
        else if ((offby == 4 || offby == 3) && solvedCount == 1 && !allEdgeColorsSolved && twoGraysUnsolved(3, 4)) //TT-84-2 + //TT-85-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + firstOffColorPiece], 14);  //algo #14 3a-  (F/L Safe)
            assert(0);
        }
        else if (solvedCount == 3 && !twoAdjacentOffColors) {
            std::cout << "solvedCount3 not accounted for \n"
                      << "Bug in rotateSolveLayer7Edges() where solvedCount == 3 && !twoAdjacentOffColors\n";
            assert(!(solvedCount == 3 && !twoAdjacentOffColors));
        }
        //Not Needed End
        else if (offby == 0 && solvedCount == 3 && twoAdjacentPieces && twoSolvedPieces(0,1) && piecesSolved[2] && twoGraysUnsolved(3,4))
        {                                                                                 //TestCube15-2-Edgesstuck
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[ORANGE], 14);
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
            ||(offby == 2 && solvedCount == 1 && piecesSolved[0] && !piecesSolved[1]) //Test2-pt2-pass      //Test4-fixns    //test5
            ||(offby == 3 && solvedCount == 1 && allCornersAllSolved && allEdgeColorsSolved && (piecesSolved[1] || piecesSolved[2])) //TT37-1 //TT42-2
            ||(offby == 3 && solvedCount == 1 && piecesSolved[0] && twoGraysUnsolved(3, 4))   //TestCube22-2Edges
            ||(offby == 1 && solvedCount == 1 && !currentpieceFlipStatusOK && piecesSolved[0] && twoGraysUnsolved(2, 3)) //TT20-2
        ) {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + lastSolvedPiece], 14);
        }
        else if ((offby == 2 || offby == 3) && solvedCount == 1 && piecesSolved[4] && twoGraysUnsolved(2,3))
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE], 14);
        }
        else if (offby == 2 && solvedCount == 1 && allCornersAllSolved && piecesSolved[1] && twoGraysUnsolved(2,3)) //TT43-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE], 14);
        }
        else if (offby == 3 && solvedCount == 1 && piecesSolved[1] && allCornersAllSolved && twoGraysUnsolved(3,4) && pieceOrder[0] == 28) //TT51-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[PINK], 14);
        }
        else if (offby == 0 && !currentpieceFlipStatusOK && twoGraysUnsolved(0,1))
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE], 14);
        }
        //still havent solved anything
        else if (offby == 3 && solvedCount == 0 && allCornersAllSolved && !piecesSolved[0] && twoGraysUnsolved(1,2)) //TT35-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE + offby], 14);
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && !currentpieceFlipStatusOK && twoGraysUnsolved(4,0)) //TT45-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE], 14);
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(2,3)) //TT48-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE], 14);
        }
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && pieceOrder[0] == 26) //TT50-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[PINK], 14);
        }
        else if (offby == 2 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(4,0) && pieceOrder[0] == 26) //TT52-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_GREEN], 14);
        }
        else if (offby == 1 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(3,4) && pieceOrder[0] == 28) //TT60-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE], 14);
        }
        else if (offby == 2 && solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved)  //TT38-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[BEIGE], 14);
        }
        //we really havent solved anything but everythings solved
        else if (solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved && checkPieceMatches(pieceOrder, 26, 29, 28, 25, 27)) //TT49-2
        {   //(needs another mushroom- after this one to solve it)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[ORANGE], 14);
        }   //becomes "26 29 [27 28] 25"
        else if (solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved && checkPieceMatches(pieceOrder, 28, 27, 29, 26, 25)) //TT73-2
        {   //(needs another mushroom- after this one to solve it)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[LIGHT_BLUE], 14);
        }   //becomes "28 [26 27] 29 25"
        else if (solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved && checkPieceMatches(pieceOrder, 27, 29, 28, 26, 25)) //TT87-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[14].algo, g_faceNeighbors[PINK], 14);
        }   //becomes "25] 27 28 26 [29"

//MUSHROOM+ #Algo#33 (all)
        else if (offby == 3 && solvedCount == 1 && piecesSolved[0] && twoGraysUnsolved(2,3))
        {                                                                                    //TestCube14-2
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[ORANGE], 33); //3e+ #33 same. (F/L safe tho)
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
            ||(offby == 1 && solvedCount == 1 && piecesSolved[0] && twoGraysUnsolved(3, 4) && grayFaceColorSolved[1] && grayFaceColorSolved[2])
            ||(offby  < 5 && solvedCount == 1 && piecesSolved[0] && !currentpieceFlipStatusOK && twoGraysUnsolved(1, 2)) //TT13   //TT19-2
            ||(offby == 2 && solvedCount == 1 && piecesSolved[4] && twoGraysUnsolved(0, 1)) //TT81-2
        ) {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE + lastSolvedPiece], 33); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 3 && solvedCount == 1 && piecesSolved[4] && allCornersAllSolved && twoGraysUnsolved(1,2)) //TT36-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE], 33); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 3 && solvedCount == 1 && allCornersAllSolved && twoGraysUnsolved(2, 3)) //TT80-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_GREEN], 33); //3e+ #33 same. (F/L safe tho)
        }
        //still havent solved anything:
        else if (offby == 4 && solvedCount == 0 && allCornersAllSolved && twoGraysUnsolved(3,4)) //TT47-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[PINK], 33); //3e+ #33 same. (F/L safe tho)
        }
        else if (offby == 1 && solvedCount == 0 && allCornersAllSolved && allEdgeColorsSolved && pieceOrder[0] == 29) //TT44-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE + offby + 2], 33); //3e+ #33 same. (F/L safe tho)
        }
        //we really havent solved anything but everythings solved
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved&& checkPieceMatches(pieceOrder, 28, 29, 26, 27, 25)) //TT75-2
        {   //(needs another mushroom- after this one to solve it)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_BLUE], 33); //3e+ #33 same. (F/L safe tho)
        }   //becomes "28 [26 27] 29 25 "
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved&& checkPieceMatches(pieceOrder, 27, 25, 29, 26, 28)) //TT82-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[ORANGE], 33); //3e+ #33 same. (F/L safe tho)
        }
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved&& checkPieceMatches(pieceOrder, 29, 28, 26, 25, 27)) //TT72-2
        {   //(needs another mushroom+ after this one to solve it)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[LIGHT_GREEN], 33); //3e+ #33 same. (F/L safe tho)
        }   //becomes "25] 28 26 27 [29"
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved&& checkPieceMatches(pieceOrder, 28, 25, 29, 27, 26)) //TT83-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[PINK], 33); //3e+ #33 same. (F/L safe tho)
        }
        else if (solvedCount == 0 && allEdgeColorsSolved && allCornersAllSolved && checkPieceMatches(pieceOrder, 27, 29, 26, 25, 28)) //TT70-2
        {   //(needs another mushroom+ after this one to solve it)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[33].algo, g_faceNeighbors[BEIGE], 33); //3e+ #33 same. (F/L safe tho)
        }   //becomes "29 [26 27] 25 28"

//HORSEFACE- #Algo12# and Algo#13#
        //5 (+) iterations also preserves corners, 2 (+) iterations does not, 1 opposite (-) iteration does not.
        //opposites faces at 6' and 1' must be solved
        else if (offby >= 3 && solvedCount == 2 && twoOppositePieces)
        {
            colordirs loc;
            for (int i = 0, j = 2; i < 5 ; MM4(j), i++, j++) {
                if (twoSolvedPieces(i, j))
                    loc = g_faceNeighbors[LIGHT_BLUE + i];
            }
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, loc, 12);    //algo #12 (3.1- CCW)
                for (int i = 0; i < g_AlgoStrings[12].repeatX; ++i)
                    shadowDom->shadowBulkRotate(bulkAlgo);
            }
            else
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, loc, 13);    //algo #13 (3.2+ CW) (opposite #12, w/ 1 rep)
        }
        else if (offby == 3 && piecesSolved[0] && !piecesSolved[1])
        {
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, g_faceNeighbors[LIGHT_BLUE], 12);    //algo #12 (3.1- CCW)
                for (int i = 0; i < g_AlgoStrings[12].repeatX; ++i)
                    shadowDom->shadowBulkRotate(bulkAlgo);
            }
            else
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, g_faceNeighbors[LIGHT_BLUE], 13);    //algo #13 (3.2+ CW) (opposite #12, w/ 1 rep)
        }
        else if (offby == 3 && solvedCount == 2 && twoSolvedPieces(2, 4) && allCornersAllSolved) //TT-40-2
        {
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, g_faceNeighbors[LIGHT_GREEN], 12);    //algo #12 (3.1- CCW)
            for (int i = 0; i < g_AlgoStrings[12].repeatX; ++i)
                shadowDom->shadowBulkRotate(bulkAlgo);
        }

//HORSEFACE+ #Algo12# and Algo#13#
        else if ((offby == 1 || offby == 2) && solvedCount == 2 && twoOppositePieces && (allEdgeColorsSolved || twoGraysUnsolved(1, 2))) //TT39-2 //TT41-2 //TT74-2
        {                                                    //testCube3-p2 ->(blue/green)=PASS  + //TT24-2
            colordirs loc;
            for (int i = 0, j = 2; i < 5 ; MM4(j), i++, j++) {
                if (twoSolvedPieces(i, j))
                    loc = g_faceNeighbors[LIGHT_BLUE + i];
            }
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, loc, 13);    //algo #13 (3.2+ CW)
                for (int i = 0; i < g_AlgoStrings[13].repeatX; ++i)
                    shadowDom->shadowBulkRotate(bulkAlgo);
            }
            else
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, loc, 12);    //algo #12 (3.1- CCW)
        }
        else if ((offby == 3 && solvedCount == 1 && piecesSolved[0] && twoGraysUnsolved(1, 2) && grayFaceColorSolved[3] && grayFaceColorSolved[4])
              || (offby == 2 && solvedCount == 2 && twoSolvedPieces(0, 2) && twoGraysUnsolved(3, 4))    //TT10
        ) {
            //5 iterations goes "+" and preserves corners, 1 iteration goes "-" and does not.
            if (allCornersAllSolved) {
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[13].algo, g_faceNeighbors[LIGHT_BLUE], 13);    //algo #13 (3.2+ CW)
                for (int i = 0; i < g_AlgoStrings[13].repeatX; ++i)
                    shadowDom->shadowBulkRotate(bulkAlgo);
            }
            else
                bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[12].algo, g_faceNeighbors[LIGHT_BLUE], 12);    //algo #12 (3.1- CCW)
        }

    //Finish up
        else
            unknownloop++;
        //DO IT:
        shadowDom->shadowBulkRotate(bulkAlgo);
        updateRotateQueueWithShadow(shadowDom);

        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    //DEV: Error Checking, make sure we don't progress past any ambiguous states
    assert(unknownloop == 0);
    std::cout << "Solved rotateSolveLayer7Edges 7 1 in " << loopcount << " loops" << std::endl;
}

static int g_dirtyBitRDRD = 0;
static int g_dirtyFaceRDRD = 0;
static int g_dirtyCountRDRD = 0;

//Last Layer = Layer 7 CORNERS (GRAY top) ( Algo#26(was7) , Algo#27, some Algo#50(was5) )
//Cube should have Gray Face (12) on top, layer 1+2+3+4+5+6 Solved already
void Megaminx::rotateSolve7thLayerCorners(Megaminx* shadowDom)
{
    //Loop management:
    bool allSolved = false;
    int loopcount = 0;
    const int startingPiece = 15;
    const int endingPiece = startingPiece + 5;
    //Loop:
    do {
        //basic overflow protection:
        if (loopcount > 120)
            break;
        //bool grayFaceColorSolved[5] = { false };
        bool piecesSolved[5] = { false };
        //check solved-ness
        shadowDom->DetectSolvedCorners(startingPiece, piecesSolved);
        int i = startingPiece;
        //can skip if solved
        while (i < endingPiece && piecesSolved[i - startingPiece])
            i++;
        //entirely solved, done
        if (i >= endingPiece) {
            allSolved = true;
            break;  //goto end
        }
        //Set up main loop variables:
        //data storage types for current state
        std::vector<numdir> bulkAlgo;
        //TODO: use CornerLayerAssist for layer 7 like we do for 1,3,5?
        const int sourceCornerIndex = shadowDom->findCorner(i);
        const int offby = sourceCornerIndex - i;
        //Populate the current order state:
        std::vector<int> pieceOrder(5);
        bool grayFaceColorSolved[5] = { false };
        for (int k = 0; k < 5; ++k) {
            Corner* CornerItselfNext = shadowDom->faces[GRAY - 1].corner[k];
            pieceOrder[k] = CornerItselfNext->data.pieceNum;
            if (CornerItselfNext->data.flipStatus == 0)
                grayFaceColorSolved[k] = true;
        }
        //std::vector<int> pieceOrder = megaminx->faces[GRAY - 1].findCornersOrder();
        std::vector<int> pieceOrderB = megaminx->findCornerPieces(megaminx->m_seventhLayerCorners);
        std::vector<int> grayFaceColorsB = megaminx->faces[GRAY - 1].findCornersColorFlipStatus();
        bool grayFaceColorSolvedB[5] = { false };
        for (int k = 0; k < 5; ++k)
            grayFaceColorSolvedB[k] = (grayFaceColorsB.at(k) == 0);
        bool grayCornerColorSolvedB[5] = { false };
        shadowDom->DetectSolvedCorners(startingPiece, grayCornerColorSolvedB);
        //iterate through the piece/color lists and make some easy compound conditional aliases
        const bool fullySolvedOrder = checkPieceMatches(pieceOrder, 15, 16, 17, 18, 19);
        const auto twoGraysUnsolved = [grayFaceColorSolved](int a, int b) { MM5(a); MM5(b); return (!grayFaceColorSolved[a] && !grayFaceColorSolved[b]); }; //MM=overrotation support
//        const bool twoAdjacentOffColors =
//            (twoGraysUnsolved(0, 1)) || (twoGraysUnsolved(1, 2)) || (twoGraysUnsolved(2, 3)) || (twoGraysUnsolved(3, 4)) || (twoGraysUnsolved(4, 0));
        const auto twoPiecesSolved = [piecesSolved](int a, int b) { return (piecesSolved[a] && piecesSolved[b]); };
        const bool twoAdjacentPieces =
            (twoPiecesSolved(0, 1)) || (twoPiecesSolved(1, 2)) || (twoPiecesSolved(2, 3)) || (twoPiecesSolved(3, 4)) || (twoPiecesSolved(4, 0));
//        const bool twoOppositePieces =
//            (twoPiecesSolved(0, 2)) || (twoPiecesSolved(1, 3)) || (twoPiecesSolved(2, 4)) || (twoPiecesSolved(3, 0)) || (twoPiecesSolved(4, 1));

//START MAIN
        //SoNot Needed. L7-Edges is mandatorily run as a pre-requisite.
        //DEVTODO: turn this into a unit test
        // firstly rotate solved Edge 0 (e#25) gray top back to default, by checking the first EDGE and setting that rotation.
        const int findIfFirstEdgeDefault = shadowDom->findEdge(25); //always piece first
        if (findIfFirstEdgeDefault > 25 && findIfFirstEdgeDefault < 30 && !piecesSolved[0] && offby != 0)
        {
            int offby = findIfFirstEdgeDefault - 25;
            shadowDom->shadowMultiRotate(GRAY, offby);
            continue;
        }

        //skip to the end if positioned correctly
        if (fullySolvedOrder)
            goto startColorFlippingCorners;
        //skip to the middle if we can, starting with pairs
//        else if (twoAdjacentPieces)
//            goto haveTwoAdjacentSolved;

//49 Testcases Total:
// Everything is #Algo#26 and #Algo#27#
//10 "no visible pairs progress" cases
        //(15 18 19 16 17)
        else if (checkPieceMatches(pieceOrder, 15, 18, 19, 16, 17))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with  --> "[15 16] 18 19 17" below (no pairs yet)

        //(16 15 19 18 17)
        else if (checkPieceMatches(pieceOrder, 16, 15, 19, 18, 17))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "17 16 19 18 15" below (no pairs yet)

        //(17 16 15 19 18) (swap 1&3+4/5)
        else if (checkPieceMatches(pieceOrder, 17, 16, 15, 19, 18))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "18 16 15 17 19" below (no pairs yet)

        //(19 18 17 16 15) - TT21-2
        else if (checkPieceMatches(pieceOrder, 19, 18, 17, 16, 15))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "15 19 17 16 18" belo (no pairs yet)

        //(18 17 15 19 16)
        else if (checkPieceMatches(pieceOrder, 18, 17, 15, 19, 16))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "17 15 18 19 16" below (no pairs yet)

        //(18 19 15 16 17)
        else if (checkPieceMatches(pieceOrder, 18, 19, 15, 16, 17))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "18 15 16 19 17" below. (no pairs yet)

        //(18 17 16 15 19)
        else if (checkPieceMatches(pieceOrder, 18, 17, 16, 15, 19))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "15 17 16 19 18" below (no pairs yet)

        //(15 19 18 17 16) - TestCube28-1
        else if (checkPieceMatches(pieceOrder, 15, 19, 18, 17, 16))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "19 18 15 17 16" below (no pairs yet)

        //(19 18 15 17 16) - TestCube27-1
        else if (checkPieceMatches(pieceOrder, 19, 18, 15, 17, 16))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "18 15 19 17 16" below (no pairs yet)

        //(17 18 19 15 16) - TestCube18-1
        else if (checkPieceMatches(pieceOrder, 17, 18, 19, 15, 16))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "15 18 19 16 17" below (no pairs yet)

//39 cases to make pairs:
        //(18 15 19 17 16) - TestCube25-1
        else if (checkPieceMatches(pieceOrder, 18, 15, 19, 17, 16))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "[15 16] 19 17 18" below && hasTwoAdjacentSolved now - 15/16

        //(17 16 19 18 15) - TestCube26-2
        else if (checkPieceMatches(pieceOrder, 17, 16, 19, 18, 15))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "[15 16] 19 17 18" below && hasTwoAdjacentSolved now - 15/16

        //(15 19 17 16 18) - TestCube20-2
        else if (checkPieceMatches(pieceOrder, 15, 19, 17, 16, 18))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "[15 16] 19 17 18" below && hasTwoAdjacentSolved now - 15/16

        //(18 15 16 19 17)
        else if (checkPieceMatches(pieceOrder, 18, 15, 16, 19, 17))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "[15 16] 18 19 17" below && hasTwoAdjacentSolved now - 15/16

        //(16 18 15 19 17)
        else if (checkPieceMatches(pieceOrder, 16, 18, 15, 19, 17))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "[15 16] 18 19 17" below && hasTwoAdjacentSolved now - 15/16

        //(15 19 16 18 17)
        else if (checkPieceMatches(pieceOrder, 15, 19, 16, 18, 17))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "[15 16] 18 19 17" below && hasTwoAdjacentSolved now - 15/16

        //TestCube25-2 (17 18 16 19 15)
        else if (checkPieceMatches(pieceOrder, 17, 18, 16, 19, 15))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "18 [16 17] 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(15 18 17 19 16) - TestCube22-3
        else if (checkPieceMatches(pieceOrder, 15, 18, 17, 19, 16))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "18 [16 17] 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(18 19 17 15 16)
        else if (checkPieceMatches(pieceOrder, 18, 19, 17, 15, 16))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "19 [16 17] 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(17 19 16 15 18)
        else if (checkPieceMatches(pieceOrder, 17, 19, 16, 15, 18))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "19 [16 17] 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(18 16 15 17 19)
        else if (checkPieceMatches(pieceOrder, 18, 16, 15, 17, 19))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "18 [16 17] 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(19 16 18 17 15)
        else if (checkPieceMatches(pieceOrder, 19, 16, 18, 17, 15))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "19 [16 17] 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(18 17 19 16 15)
        else if (checkPieceMatches(pieceOrder, 18, 17, 19, 16, 15))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "18 [16 17] 19 15" below && hasTwoAdjacentSolved now - 16/17
        //(18 16 19 15 17)
        else if (checkPieceMatches(pieceOrder, 18, 16, 19, 15, 17))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "18 [16 17] 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(19 16 15 18 17)
        else if (checkPieceMatches(pieceOrder, 19, 16, 15, 18, 17))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "19 [16 17] 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(19 17 15 16 18)
        else if (checkPieceMatches(pieceOrder, 19, 17, 15, 16, 18))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "19 [16 17] 15 18" below && hasTwoAdjacentSolved now - 16/17

        //(18 19 16 17 15) - TT16-2
        else if (checkPieceMatches(pieceOrder, 18, 19, 16, 17, 15))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "18 [16 17] 19 15" below && hasTwoAdjacentSolved now - 16/17

        //(19 17 18 15 16)
        else if (checkPieceMatches(pieceOrder, 19, 17, 18, 15, 16))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with  --> "19 15 [17 18] 16" below && hasTwoAdjacentSolved now - 17/18

        //(16 19 18 15 17)
        else if (checkPieceMatches(pieceOrder, 16, 19, 18, 15, 17))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with  --> "16 19 [17 18] 15" below && hasTwoAdjacentSolved now - 17/18

        //(16 18 19 17 15)
        else if (checkPieceMatches(pieceOrder, 16, 18, 19, 17, 15))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "16 19 [17 18] 15" below && hasTwoAdjacentSolved now - 17/18

        //(16 15 17 19 18)
        else if (checkPieceMatches(pieceOrder, 16, 15, 17, 19, 18))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "19 15 [17 18] 16" below && hasTwoAdjacentSolved now - 17/18

        //(19 17 16 18 15)
        else if (checkPieceMatches(pieceOrder, 19, 17, 16, 18, 15))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "16 19 [17 18] 15" below && hasTwoAdjacentSolved now - 17/18

        //(19 15 18 16 17)
        else if (checkPieceMatches(pieceOrder, 19, 15, 18, 16, 17))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "19 15 [17 18] 16" below && hasTwoAdjacentSolved now - 17/18

        //(18 15 17 16 19)
        else if (checkPieceMatches(pieceOrder, 18, 15, 17, 16, 19))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "19 15 [17 18] 16" below && hasTwoAdjacentSolved now - 17/18

        //(16 19 15 17 18)
        else if (checkPieceMatches(pieceOrder, 16, 19, 15, 17, 18))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "16 19 [17 18] 15" below && hasTwoAdjacentSolved now - 17/18

        //(17 15 19 16 18)
        else if (checkPieceMatches(pieceOrder, 17, 15, 19, 16, 18))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "17 15 16 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(19 15 16 17 18) (all off by 1)
        else if (checkPieceMatches(pieceOrder, 19, 15, 16, 17, 18))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "17 15 16 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(16 17 18 19 15) (all off by 1 in other direction)
        else if (checkPieceMatches(pieceOrder, 16, 17, 18, 19, 15))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "16 17 15 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(16 17 19 15 18)
        else if (checkPieceMatches(pieceOrder, 16, 17, 19, 15, 18))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "16 17 15 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(17 19 15 18 16)
        else if (checkPieceMatches(pieceOrder, 17, 19, 15, 18, 16))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "16 17 15 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(17 16 18 15 19)
        else if (checkPieceMatches(pieceOrder, 17, 16, 18, 15, 19))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "17 15 16 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(17 15 18 19 16)
        else if (checkPieceMatches(pieceOrder, 17, 15, 18, 19, 16))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "17 15 16 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(16 18 17 15 19)
        else if (checkPieceMatches(pieceOrder, 16, 18, 17, 15, 19))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "16 17 15 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(17 18 15 16 19) swap 1&3+2&4
        else if (checkPieceMatches(pieceOrder, 17, 18, 15, 16, 19))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "17 15 16 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(16 15 18 17 19)
        else if (checkPieceMatches(pieceOrder, 16, 15, 18, 17, 19))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "16 17 15 [18 19]" below && hasTwoAdjacentSolved now - 18/19

        //(19 18 16 15 17)
        else if (checkPieceMatches(pieceOrder, 19, 18, 16, 15, 17))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "15] 18 16 17 [19" below && hasTwoAdjacentSolved now - 19/15

        //(15 17 19 18 16)
        else if (checkPieceMatches(pieceOrder, 15, 17, 19, 18, 16))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        } //continues with --> "15] 17 18 16 [19" below && hasTwoAdjacentSolved now - 19/15

        //(15 17 16 19 18)
        else if (checkPieceMatches(pieceOrder, 15, 17, 16, 19, 18))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "15] 17 18 16 [19" below && hasTwoAdjacentSolved now - 19/15

        //(17 19 18 16 15)
        else if (checkPieceMatches(pieceOrder, 17, 19, 18, 16, 15))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        } //continues with --> "15] 17 18 16 [19" below && hasTwoAdjacentSolved now - 19/15

    haveTwoAdjacentSolved:
        //FINAL STAGE: Two adjacent pieces. One bulk move left till solved. (10 cases)
        //([15 16] 18 19 17) - 15/16 ok
        if (checkPieceMatches(pieceOrder, 15, 16, 18, 19, 17))
        {
            const int startFace = LIGHT_BLUE + 0;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        }
        //([15 16] 19 17 18) - 15/16 ok
        else if (checkPieceMatches(pieceOrder, 15, 16, 19, 17, 18))
        {
            const int startFace = LIGHT_BLUE + 5;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        }
        //(18 [16 17] 19 15) - 16/17 ok
        else if (checkPieceMatches(pieceOrder, 18, 16, 17, 19, 15))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        }
        //(19 [16 17] 15 18) - 16/17 ok
        else if (checkPieceMatches(pieceOrder, 19, 16, 17, 15, 18))
        {
            const int startFace = LIGHT_BLUE + 1;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        }
        //(16 19 [17 18] 15) - 17/18 ok
        else if (checkPieceMatches(pieceOrder, 16, 19, 17, 18, 15))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        }
        //(19 15 [17 18] 16) - 17/18 ok
        else if (checkPieceMatches(pieceOrder, 19, 15, 17, 18, 16))
        {
            const int startFace = LIGHT_BLUE + 2;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        }
        //(16 17 15 [18 19]) - 18/19 ok
        else if (checkPieceMatches(pieceOrder, 16, 17, 15, 18, 19))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace];    //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        }
        //(17 15 16 [18 19]) - 18/19 ok
        else if (checkPieceMatches(pieceOrder, 17, 15, 16, 18, 19))
        {
            const int startFace = LIGHT_BLUE + 3;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        }
        //(15] 17 18 16 [19) - 19/15 ok
        else if (checkPieceMatches(pieceOrder, 15, 17, 18, 16, 19))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace];        //Algo#26(was7) Cycle- (CCW)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[26].algo, loc, 26);
        }
        //(15] 18 16 17 [19) - 19/15 ok
        else if (checkPieceMatches(pieceOrder, 15, 18, 16, 17, 19))
        {
            const int startFace = LIGHT_BLUE + 4;
            colordirs loc = g_faceNeighbors[startFace - 1];    //Algo#27 Cycle+ (Clockwise)
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[27].algo, loc, 27);
        }
//DO IT:
finalizePosition:
        shadowDom->shadowBulkRotate(bulkAlgo);

startColorFlippingCorners:
        const auto currentPiece = shadowDom->getPieceArray<Corner>(sourceCornerIndex);
        const bool currentpieceFlipStatusOK = currentPiece->data.flipStatus == 0;

        //Do R'DR'rdr to color-flip each corner. (Dirties the lower rows) Each piece flip takes 2x, and each line resets at 6x.
        if (fullySolvedOrder && offby == 0 && g_dirtyCountRDRD != 6 && !currentpieceFlipStatusOK) {
            //only do this if the piece is mis-colored.
            int offset = (i - startingPiece) - 1;
            if (offset == -1) offset = 4;
            const int target = LIGHT_BLUE + offset;
            g_dirtyBitRDRD = 1;
            g_dirtyFaceRDRD = g_dirtyFaceRDRD ? g_dirtyFaceRDRD : target;
            //rotateOffset is how we rotate gray top to move unsolved piece back over top of our dirty face
            int rotateOffset = (target - g_dirtyFaceRDRD);
            if (g_dirtyCountRDRD != 0)
                shadowDom->shadowMultiRotate(GRAY, rotateOffset);
            bulkAlgo = shadowDom->ParseAlgorithmString(g_AlgoStrings[50].algo, g_faceNeighbors[g_dirtyFaceRDRD], 50);
            shadowDom->shadowBulkRotate(bulkAlgo);  //  .num = 50 ,  .algo = "R' DR' r dr"
            shadowDom->shadowBulkRotate(bulkAlgo);  //  .num = 50 ,  .algo = "R' DR' r dr"
            rotateOffset *= -1;
            if (g_dirtyCountRDRD != 0 )
                shadowDom->shadowMultiRotate(GRAY, rotateOffset);
            g_dirtyCountRDRD += 2;
            bulkAlgo = {};
        }
        else if (g_dirtyBitRDRD == 1 && g_dirtyCountRDRD == 6)
        {   //clear bits.
            g_dirtyBitRDRD = 0;
            g_dirtyFaceRDRD = 0;
            g_dirtyCountRDRD = 0;
        }

        loopcount++;
    } while (!allSolved);
    //After all loops, load the shadow Queue into the real megaminx queue,
    //Commit solved state.
    updateRotateQueueWithShadow(shadowDom);
    std::cout << "Solved rotateSolveLayer7Corners 7 2 in " << loopcount << " loops" << std::endl;
}

//This is the result of a solved cube + algorithm and then testing the gray faces' EDGES
void Megaminx::testingAlgostrings(Megaminx* shadowDom)
{
    for (int algo=1; algo<ALL_ALGORITHMS; algo++) {
        shadowDom = new Megaminx();
        const AlgoString al = g_AlgoStrings[algo];
        const colordirs loc = g_faceNeighbors[LIGHT_BLUE];    //front-face (DEV adjustable)
        const int repeat = al.repeatX ? al.repeatX : 1;
        for (int n = 0; n < repeat; ++n) {
            std::vector<numdir> bulk = shadowDom->ParseAlgorithmString(al.algo, loc, algo);
            shadowDom->shadowBulkRotate(bulk);
            std::cout <<  bulk.size() << "  = algo length, Algo # " << algo << std::endl;
        }
        //find where pieces actually are
        std::vector<int> foundEdges = shadowDom->faces[GRAY - 1].findEdgesOrder();
        // vs. where they're supposed to be
        std::vector<int> defaultEdges = faces[GRAY - 1].defaultEdges;
        //output the difference as an int.
        std::vector<int> newDifference(5);
        for (int k = 0; k < 5; ++k) {
            newDifference[k] = foundEdges[k] - defaultEdges[k];
            rM3Mr(newDifference[k]);
//DEV       //Print the difference:
//DEV       std::cout << newDifference[k] << ", ";
        }
        std::vector<int> modby(5);
        for (int k = 0; k < 5; ++k) {
//DEV       std::cout << foundEdges[k] << ", ";
            if (foundEdges[k] == 25)
                modby[0]=newDifference[k];
            if (foundEdges[k] == 26)
                modby[1]=newDifference[k];
            if (foundEdges[k] == 27)
                modby[2]=newDifference[k];
            if (foundEdges[k] == 28)
                modby[3]=newDifference[k];
            if (foundEdges[k] == 29)
                modby[4]=newDifference[k];
        }
//DEV   std::cout << "  = foundEdges:  # " << algo << std::endl;
        for (int k = 0; k < 5; ++k) {
            std::cout << modby[k] << ", ";
        }
        std::cout << "  = modby, Algo # " << algo << std::endl;
        //also, not all results are valid conceptually. do not trust, verify.
        delete shadowDom;
    }
//DEV    updateRotateQueueWithShadow(shadowDom); //actually output the algo's changes to main cube's GUI
}
