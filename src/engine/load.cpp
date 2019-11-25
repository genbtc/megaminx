#include <vector>
#include <iostream>
#include <fstream>
#include "megaminx.h"

extern Megaminx* megaminx;
extern Megaminx* shadowDom;
void serializeVectorInt(std::vector<int> list1, std::string filename);
void WritePiecesFile(std::string filename, bool corner);
const std::vector<int> ReadPiecesFileVector(std::string filename);
void FromCubeToVectorFile();
void FromVectorFileToCube();
void FromCubeToShadowCube();

//Cube SaveState filenames
#define EDGEFILE "EdgeCurPos.dat"
#define CORNERFILE "CornerCurPos.dat" 
#define EDGEFILECOLORS "EdgeColorPos.dat"
#define CORNERFILECOLORS "CornerColorPos.dat"

//Store Cube (Write VectorFile)
void FromCubeToVectorFile() {
    WritePiecesFile(EDGEFILE, false);
    WritePiecesFile(CORNERFILE, true);
    serializeVectorInt(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}
//Load Cube (Read VectorFile)
void FromVectorFileToCube() {
    const std::vector<int> &readEdgevector = ReadPiecesFileVector(EDGEFILE);
    const std::vector<int> &readEdgeColorvector = ReadPiecesFileVector(EDGEFILECOLORS);
    const std::vector<int> &readCornervector = ReadPiecesFileVector(CORNERFILE);
    const std::vector<int> &readCornerColorvector = ReadPiecesFileVector(CORNERFILECOLORS);
    for (int i = 0; i < 12; ++i) {
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
extern void serializeVectorInt(std::vector<int> list1, std::string filename) {
    std::ofstream file(filename);
    file << "{ ";
    for (auto l : list1) {
        file << l << ", ";
    }
    file << " }\n";
    file.close();
}

//call findpieces and write them to output file
void WritePiecesFile(std::string filename, bool corner)
{
    std::ofstream file(filename);
    for (int i = 1; i <= 12; ++i) {
        file << "----------[ " << i << " ]----------\n";
        std::vector<int> f = corner ? megaminx->findCorners(i) : megaminx->findEdges(i);
        file << f[0] << ", " << f[1] << ", " << f[2] << ", " << f[3] << ", " << f[4] << "\n";
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


//Generic template to Reset all the pieces to their default value. simple.
template <typename T>
int Megaminx::resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve)
{
    if (std::is_same<T, Edge>::value)
        return resetFacesEdges(color_n, defaultPieces, solve);
    else if (std::is_same<T, Corner>::value)
        return resetFacesCorners(color_n, defaultPieces, solve);
    return 0;
}

template <typename T>
int Megaminx::LoadNewPiecesFromVector(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors)
{
    assert(readPieces.size() == 60);
    assert(readPieceColors.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        std::vector<int> loadPieces = { readPieces[f + 0],readPieces[f + 1],readPieces[f + 2],readPieces[f + 3],readPieces[f + 4] };
        resetFacesPieces<T>(face, loadPieces, false);
        //resetFivePieces<T>(loadPieces);
    }
    for (int face = 0; face < 12; ++face) {
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

