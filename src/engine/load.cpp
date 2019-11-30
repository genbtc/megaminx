#include <vector>
#include <iostream>
#include <fstream>
#include "megaminx.h"

extern Megaminx* megaminx;
extern Megaminx* shadowDom;
void serializeVectorInt(std::vector<int> list1, std::string filename);
void WritePiecesFile(std::string filename, bool corner);
const std::vector<int> ReadPiecesFileVector(std::string filename);
//void FromCubeToVectorFile();
//void FromVectorFileToCube();
//void FromCubeToShadowCube();

//Cube SaveState filenames
#define EDGEFILE "EdgePositions.dat"
#define CORNERFILE "CornerPositions.dat" 
#define EDGEFILECOLORS "EdgeColors.dat"
#define CORNERFILECOLORS "CornerColors.dat"

//Store Cube / Write VectorFile
void FromCubeToVectorFile() {
    serializeVectorInt(megaminx->getAllEdgePiecesPosition(), EDGEFILE);
    serializeVectorInt(megaminx->getAllCornerPiecesPosition(), CORNERFILE);
    serializeVectorInt(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}

//Load Cube / Read VectorFile
void FromVectorFileToCube() {
    const std::vector<int> &readEdgevector = ReadPiecesFileVector(EDGEFILE);
    const std::vector<int> &readEdgeColorvector = ReadPiecesFileVector(EDGEFILECOLORS);
    const std::vector<int> &readCornervector = ReadPiecesFileVector(CORNERFILE);
    const std::vector<int> &readCornerColorvector = ReadPiecesFileVector(CORNERFILECOLORS);
    for (int i = 0; i < 20; ++i) { //brute force get the data in. 12 was working. 1 isnt.
        megaminx->LoadNewEdgesFromVector(readEdgevector, readEdgeColorvector);
        megaminx->LoadNewCornersFromVector(readCornervector, readCornerColorvector);
    }
}

//Load Source Cube and store into Shadow Cube
void FromCubeToShadowCube() {
    if (shadowDom)
        delete shadowDom;
    shadowDom = new Megaminx();
    shadowDom->LoadNewEdgesFromOtherCube(megaminx);
    shadowDom->LoadNewCornersFromOtherCube(megaminx);
}

//helper function takes a vector and writes the { } object and each element to a text file
void serializeVectorInt(std::vector<int> vec, std::string filename) {
    std::ofstream file(filename);
    for (int i=1; i <= 12; ++i) {
        int f = ((i - 1) * 5);
        file << "---[ " << i << " ]---\n";
        //std::vector<int> f = corner ? megaminx->findCorners(i) : megaminx->findEdges(i);
        file << vec[f+0] << " " << vec[f+1] << " " << vec[f+2] << " " << vec[f+3] << " " << vec[f+4] << "\n";
    }
    file.close();
}

//basic parser for reading vectors from file.
const std::vector<int> ReadPiecesFileVector(std::string filename)
{
    std::ifstream input(filename);
    std::vector<int> readvector;
    std::string line;                          // iterate each line
    while (std::getline(input, line)) {        // getline returns the stream by reference, so this handles EOF
        std::stringstream ss(line);            // create a stringstream out of each line
        int readint = -1;                      // start a new node
        while (ss) {                           // while the stream is good
            std::string word;                  // get first word
            if (ss >> word) {
                if ((word.find("{") == 0) || (word.find("}") == 0))
                    continue;
                if ((word.find("---") == 0) || (word[0] == '-'))
                    break;
                readint = std::stoi(word); //convert to int
                if (readint < 0)
                    break;
                readvector.push_back(readint);
            }
        }
    }
    //TEST:
    //serializeVectorInt(readvector, "ReOutput" + filename );
    return readvector;
}


template <typename T>
int Megaminx::LoadNewPiecesFromVector(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors)
{
    assert(readPieces.size() == 60);
    assert(readPieceColors.size() == 60);
    for (int face = 1; face <= 1; ++face) {
        int f = ((face - 1) * 5);
        Piece* pieceArray = getPieceArray<T>(0);
        std::vector<int> loadPieces = { readPieces[f + 0],readPieces[f + 1],readPieces[f + 2],readPieces[f + 3],readPieces[f + 4] };
        //std::sort(loadPieces.begin(), loadPieces.end());
        //resetFacesPieces<T>(face, loadPieces, false);
        //resetFivePiecesV<T>(loadPieces);
        
        for (int i = 0; i < 5; ++i) {
            std::vector<int> foundFacePieces = findPiecesOrder<T>(face);
            Face activeFace = faces[(face - 1)];
            Piece* facePiece = activeFace.getFacePiece<T>(i);
            if (facePiece->data.pieceNum != loadPieces[i]) {
                pieceArray[facePiece->data.pieceNum].swapdata(pieceArray[loadPieces[i]].data);
            }
        }
    }
    for (int face = 0; face < 1; ++face) {
        int f = face * 5;
        //Pieces can be in the right place but maybe in wrong orientation, so flip the colors:
        for (int i = 0; i < 5; ++i) {
            Piece* piece = faces[face].getFacePiece<T>(i);
            while (piece->data.flipStatus != readPieceColors[f + i])
                piece->flip();
        }
    }
    return 1;
} //where T = Corner or Edge
int Megaminx::LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors) {
    return LoadNewPiecesFromVector<Corner>(readCorners, readCornerColors);
}
int Megaminx::LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors) {
    return LoadNewPiecesFromVector<Edge>(readEdges, readEdgeColors);
}

