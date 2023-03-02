#include "megaminx.hpp"
#include "load.hpp"

extern Megaminx* megaminx;
extern Megaminx* shadowDom;
//pre-declarations (^ these should be passed as params, no need for extern globals)

//Cube SaveState filenames
#define EDGEFILE "EdgePositions30.dat"
#define EDGEFILECOLORS "EdgeColors30.dat"
#define CORNERFILE "CornerPositions20.dat" 
#define CORNERFILECOLORS "CornerColors20.dat"
//TODO: combine into 1 file-format, its un-human un-readable un-correlatable numbers as is.

//Save/Store Cube - (Write VectorFile)
void SaveCubetoFile() {
    serializeVectorInt5(megaminx->getAllEdgePiecesPosition(), EDGEFILE);
    serializeVectorInt5(megaminx->getAllCornerPiecesPosition(), CORNERFILE);
    serializeVectorInt5(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt5(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}

//Restore/Load Cube - (Read VectorFile)
void RestoreCubeFromFile() {
    if (shadowDom)
        delete shadowDom;
    shadowDom = new Megaminx();
    const std::vector<int> &readEdgevector = ReadPiecesFileVector(EDGEFILE);
    const std::vector<int> &readEdgeColorvector = ReadPiecesFileVector(EDGEFILECOLORS);
    const std::vector<int> &readCornervector = ReadPiecesFileVector(CORNERFILE);
    const std::vector<int> &readCornerColorvector = ReadPiecesFileVector(CORNERFILECOLORS);
    megaminx->LoadNewEdgesFromVector(readEdgevector, readEdgeColorvector, shadowDom);
    megaminx->LoadNewCornersFromVector(readCornervector, readCornerColorvector, shadowDom);
}

//Load Source Cube and store into Shadow Cube
void MakeShadowCubeClone() {
    if (shadowDom)
        delete shadowDom;
    shadowDom = new Megaminx();
    shadowDom->LoadNewEdgesFromOtherCube(megaminx);
    shadowDom->LoadNewCornersFromOtherCube(megaminx);
}

//helper function takes a vector of any length and writes 5 elements per line to a text file
void serializeVectorInt5(std::vector<int> vec, std::string filename) {
    std::ofstream file(filename);
    int count = 0;
    for (auto &v : vec) {
        file << " " << v << " ";
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
    while (std::getline(input, line)) {   // getline returns the stream by reference, so this handles EOF
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
    //serializeVectorInt5(readvector, "ReOutput" + filename );
    return readvector;
}