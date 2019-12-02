#include <vector>
#include <iostream>
#include <fstream>
#include "megaminx.h"

extern Megaminx* megaminx;
extern Megaminx* shadowDom;
void serializeVectorInt60(std::vector<int> list1, std::string filename);
void serializeVectorInt30(std::vector<int> list1, std::string filename);
void WritePiecesFile(std::string filename, bool corner);
const std::vector<int> ReadPiecesFileVector(std::string filename);

//Cube SaveState filenames
#define EDGEFILE "EdgePositions30.dat"
#define CORNERFILE "CornerPositions30.dat" 
#define EDGEFILECOLORS "EdgeColors30.dat"
#define CORNERFILECOLORS "CornerColors30.dat"

//Store Cube / Write VectorFile
void FromCubeToVectorFile() {
    serializeVectorInt30(megaminx->getAllEdgePiecesPosition(), EDGEFILE);
    serializeVectorInt30(megaminx->getAllCornerPiecesPosition(), CORNERFILE);
    serializeVectorInt30(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt30(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}

//Load Cube / Read VectorFile
void FromVectorFileToCube() {
    const std::vector<int> &readEdgevector = ReadPiecesFileVector(EDGEFILE);
    const std::vector<int> &readEdgeColorvector = ReadPiecesFileVector(EDGEFILECOLORS);
    const std::vector<int> &readCornervector = ReadPiecesFileVector(CORNERFILE);
    const std::vector<int> &readCornerColorvector = ReadPiecesFileVector(CORNERFILECOLORS);
    megaminx->LoadNewEdgesFromVector(readEdgevector, readEdgeColorvector);
    megaminx->LoadNewCornersFromVector(readCornervector, readCornerColorvector);
}

//Load Source Cube and store into Shadow Cube
void FromCubeToShadowCube() {
    if (shadowDom)
        delete shadowDom;
    shadowDom = new Megaminx();
    shadowDom->LoadNewEdgesFromOtherCube(megaminx);
    shadowDom->LoadNewCornersFromOtherCube(megaminx);
}

//helper function takes a 60 vector and writes each element to a text file by face
//(DEPRECATED)
void serializeVectorInt60(std::vector<int> vec, std::string filename) {
    std::ofstream file(filename);
    for (int i=1; i <= 12; ++i) {
        int f = ((i - 1) * 5);
        file << "---[ " << i << " ]---\n";
        //std::vector<int> f = corner ? megaminx->findCorners(i) : megaminx->findEdges(i);
        file << vec[f+0] << " " << vec[f+1] << " " << vec[f+2] << " " << vec[f+3] << " " << vec[f+4] << "\n";
    }
    file.close();
}

//helper function takes a 30 vector and writes each element to a text file
void serializeVectorInt30(std::vector<int> vec, std::string filename) {
    std::ofstream file(filename);
    int count = 0;
    for (auto &v : vec) {
        if (v < 10)
            file << " ";
        file << v << " ";
        count++;
        if (count % 5 == 0)
            file << "\n";
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
    int s = getMaxNumberOfPieces<T>();
    assert(readPieces.size() == s);
    assert(readPieceColors.size() == s);
    int count = 0;
    Piece* pieceArray = getPieceArray<T>(0);
    for (int i = 0; i < readPieces.size(); ++i) {
        auto &p = readPieces[i];
        if (pieceArray[p].data.pieceNum != p) {
            pieceArray[pieceArray[p].data.pieceNum].swapdata(pieceArray[p].data);
            i = -1; //loop reset needed or else we skip swaps when the pair reverts backwards.
        }
    }
    for (int i = 0; i < readPieceColors.size(); ++i) {
        auto &p = readPieces[i];
        auto &c = readPieceColors[i];
        //Pieces are in the right place but maybe wrong orientation, so Flip the colors:
        while (pieceArray[p].data.flipStatus != c)
            pieceArray[p].flip();
    }
    return 0;
} //where T = Corner or Edge
int Megaminx::LoadNewCornersFromVector(const std::vector<int> &readCorners, const std::vector<int> &readCornerColors) {
    return LoadNewPiecesFromVector<Corner>(readCorners, readCornerColors);
}
int Megaminx::LoadNewEdgesFromVector(const std::vector<int> &readEdges, const std::vector<int> &readEdgeColors) {
    return LoadNewPiecesFromVector<Edge>(readEdges, readEdgeColors);
}

