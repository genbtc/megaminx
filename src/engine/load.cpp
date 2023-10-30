#include "megaminx.hpp"
#include "load.hpp"

//Cube SaveState filenames
constexpr const char* EDGEFILE("EdgePositions30.dat");
constexpr const char* EDGEFILECOLORS("EdgeColors30.dat");
constexpr const char* CORNERFILE("CornerPositions20.dat");
constexpr const char* CORNERFILECOLORS("CornerColors20.dat");
constexpr const char* MEGAMINXBLOB("MegaminxSAVEFILE.txt");
//OLD Cube SaveState filenames - never gonna work, format incompatible now.
// Redo save/load algorithm and make load.hpp @ 9b85ebddf10cfe30fd1db5947d5d8a65b8f5912a 2023/03/05
constexpr const char* OLDEDGEFILE("EdgeCurPos.dat");
constexpr const char* OLDEDGEFILECOLORS("EdgeColorPos.dat");
constexpr const char* OLDCORNERFILE("CornerCurPos.dat");
constexpr const char* OLDCORNERFILECOLORS("CornerColorPos.dat");
constexpr const char* OLDMEGAMINXBLOB("MegaminxSAVEFILE.txt");
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
 * @property not used
 */
[[deprecated]]
void SaveCubetoFileMonolithic() {
    std::ofstream file(MEGAMINXBLOB);
    file << "EdgePiecesPosition: " << serializeVectorIntToString(megaminx->getAllEdgePiecesPosition()) << std::endl;
    file << "CornerPiecesPosition: " << serializeVectorIntToString(megaminx->getAllCornerPiecesPosition()) << std::endl;
    file << "EdgePiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllEdgePiecesColorFlipStatus()) << std::endl;
    file << "CornerPiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllCornerPiecesColorFlipStatus()) << std::endl;
}

/**
 * @brief Restore/Load Cube - (Read 4 Vector Files)
 */
void RestoreCubeFromFile() {
    //TODO: Crashes w/ segfault if the files dont exist. check for them.
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

//[[deprecated]]
void RestoreOldCubeFromFile(std::string testDir) {
    MakeShadowCubeClone();
    //TODO: Crashes w/ segfault if the files dont exist. check for them first.
    std::cout << "STATUS BEFORE: \n"    ;
    const std::vector<int> readEdgevector = ReadPiecesFileVector(testDir+OLDEDGEFILE);
    const std::vector<int> readEdgeColorvector = ReadPiecesFileVector(testDir+OLDEDGEFILECOLORS);
    const std::vector<int> readCornervector = ReadPiecesFileVector(testDir+OLDCORNERFILE);
    const std::vector<int> readCornerColorvector = ReadPiecesFileVector(testDir+OLDCORNERFILECOLORS);
    std::cout << "EdgePiecesPosition: " << serializeVectorIntToString(readEdgevector) << std::endl;
    std::cout << "CornerPiecesPosition: " << serializeVectorIntToString(readEdgeColorvector) << std::endl;
    std::cout << "EdgePiecesColorFlip: " << serializeVectorIntToString(readCornervector) << std::endl;
    std::cout << "CornerPiecesColorFlip: " << serializeVectorIntToString(readCornerColorvector) << std::endl;
    if ((readEdgevector.size() != 60 && readCornervector.size() != 60 ) &&
        (readEdgeColorvector.size() != 60 && readCornerColorvector.size() != 60)) {
        std::cerr << "There was an error reading the input file - "
                     "(internal contents corrupt or size mismatch) !!!!!!" << std::endl;
        return;
    }
    //read 12 lines * 5 = 60
    int es,cs = 0;
    es=megaminx->LoadNewEdgesFromVector(readEdgevector, readEdgeColorvector);
    cs=megaminx->LoadNewCornersFromVector(readCornervector, readCornerColorvector);
    std::cout << "STATUS AFTER: \n"    ;
    std::cout << "EdgePiecesPosition: " << serializeVectorIntToString(megaminx->getAllEdgePiecesPosition()) << std::endl;
    std::cout << "CornerPiecesPosition: " << serializeVectorIntToString(megaminx->getAllCornerPiecesPosition()) << std::endl;
    std::cout << "EdgePiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllEdgePiecesColorFlipStatus()) << std::endl;
    std::cout << "CornerPiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllCornerPiecesColorFlipStatus()) << std::endl;


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
[[deprecated]]
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
    //UNIT TEST, reoutput as another file:
    serializeVectorInt5ToFile(readvector, "New6060-"+filename );
    return readvector;
}


//[[deprecated]]
template <typename T>
int Megaminx::LoadNewPiecesFromVector(const std::vector<int> &readPieces, const std::vector<int> &readPieceColors)
{
    assert(readPieces.size() == 60);
    assert(readPieceColors.size() == 60);
    for (int face = 1; face <= 12; ++face) {
        int f = ((face - 1) * 5);
        const std::vector<int> loadPieces = { readPieces[f + 0],readPieces[f + 1],readPieces[f + 2],readPieces[f + 3],readPieces[f + 4] };
        resetFacesPieces<T>(face, loadPieces, false); //false = no-defaults/color
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

//Resets--------------------------------------------------------------------------------------------------------------//
/**
 * \brief Generic <template> to Reset all the Face pieces to their default value.
 * \param color_n N'th Face/Color Number (1-12)
 * \return 1 if anything moved, 0 if not
 * \deprecated Not Currently Used But Has Good Template is_same Machinery
 */
template <typename T>
int Megaminx::resetFacesPieces(int color_n, const std::vector<int> &defaultPieces, bool solve)
{
    if (std::is_same<T, Edge>::value)
        return resetFacesEdges(color_n, defaultPieces, solve);
    else if (std::is_same<T, Corner>::value)
        return resetFacesCorners(color_n, defaultPieces, solve);
    return 0;
} //where T = Corner or Edge (templated in header)
//had to be moved here because it was called for in the old algo.
