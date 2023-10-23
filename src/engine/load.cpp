#include "megaminx.hpp"
#include "load.hpp"

//Cube SaveState filenames
#define EDGEFILE "EdgePositions30.dat"
#define EDGEFILECOLORS "EdgeColors30.dat"
#define CORNERFILE "CornerPositions20.dat"
#define CORNERFILECOLORS "CornerColors20.dat"
#define MEGAMINXBLOB "MegaminxSAVEFILE.txt"

/**
 * @brief Save/Store Cube - (Write 4 Vector Files)
 */
void SaveCubetoFile() {
    serializeVectorInt5ToFile(megaminx->getAllEdgePiecesPosition(), EDGEFILE);
    serializeVectorInt5ToFile(megaminx->getAllCornerPiecesPosition(), CORNERFILE);
    serializeVectorInt5ToFile(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt5ToFile(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}

/**
 * @brief Save/Store Cube - (Write 1 File with 4 lines)
 */
void SaveCubetoFileMonolithic() {
    std::ofstream file(MEGAMINXBLOB);
    file << "EdgePiecesPosition: " << serializeVectorIntToString(megaminx->getAllEdgePiecesPosition()) << "\n";
    file << "CornerPiecesPosition: " << serializeVectorIntToString(megaminx->getAllCornerPiecesPosition()) << "\n";
    file << "EdgePiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllEdgePiecesColorFlipStatus()) << "\n";
    file << "CornerPiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllCornerPiecesColorFlipStatus()) << "\n";
}

/**
 * @brief Restore/Load Cube - (Read 4 Vector Files)
 */
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

/**
 * @brief Shadow Cube - Clone real Megaminx viewmodel to invisible twin model
 */
void MakeShadowCubeClone() {
    if (shadowDom)
        delete shadowDom;
    shadowDom = new Megaminx();
    shadowDom->LoadNewEdgesFromOtherCube(megaminx);
    shadowDom->LoadNewCornersFromOtherCube(megaminx);
}

/**
 * @brief serializeVectorInt5ToFile - write vectors to file
 * @param vec - Input: a vector of any length
 * @param filename - Output: name of file to write ofstream to
 * @return void - wrote vector to file, 5 elements per line
 */
void serializeVectorInt5ToFile(std::vector<int> vec, std::string filename) {
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

/**
 * @brief serializeVectorIntToString - convert vectors to string
 * @param vec - Input: a vector of any length
 * @return string - Output: 5 elements per line in a string
 */
std::string serializeVectorIntToString(std::vector<int> vec) {
    std::ostringstream vectorString;
    int count = 0;
    for (auto &v : vec) {
        vectorString << " " << v << " ";
        count++;
    }
    assert(count==vec.size());
    return vectorString.str();
}

/**
 * @brief ReadPiecesFileVector - basic parser for reading vectors from file.
 * @param filename - Input: name of file to read from
 * @return vector - new vector containing elements from the file
 */
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
    //UNIT TEST:
    //serializeVectorInt5(readvector, "ReOutput" + filename );
    return readvector;
}
