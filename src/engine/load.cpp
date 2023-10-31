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
constexpr const char* OLDEDGEFILE("EdgeCurPos.dat");  //not needed really anymore.
constexpr const char* OLDEDGEFILECOLORS("EdgeColorPos.dat");
constexpr const char* OLDCORNERFILE("CornerCurPos.dat");
constexpr const char* OLDCORNERFILECOLORS("CornerColorPos.dat");
constexpr const char* OLDMEGAMINXBLOB("MegaminxSAVEFILE.txt");

struct megaminxPieceData {
    std::vector<int>  EdgePiecesPosition = {};
    std::vector<int>  CornerPiecesPosition = {};
    std::vector<int>  EdgePiecesColors = {};
    std::vector<int>  CornerPiecesColors = {};
    std::string       infilepath = "";
    std::string       outfiledir = "";

    void readFileConstruct(std::string testDir) {
        infilepath = testDir;
        EdgePiecesPosition = ReadPiecesFileVector(testDir+EDGEFILE);
        EdgePiecesColors = ReadPiecesFileVector(testDir+EDGEFILECOLORS);
        CornerPiecesPosition = ReadPiecesFileVector(testDir+CORNERFILE);
        CornerPiecesColors = ReadPiecesFileVector(testDir+CORNERFILECOLORS);
    }
    void cloneConstruct() {
        EdgePiecesPosition = megaminx->getAllEdgePiecesPosition();
        EdgePiecesColors = megaminx->getAllEdgePiecesColorFlipStatus();
        CornerPiecesPosition = megaminx->getAllCornerPiecesPosition();
        CornerPiecesColors = megaminx->getAllCornerPiecesColorFlipStatus();
    }
    void serializeMegaminxOutput() const {
        std::cout << "EdgePiecesPosition30: " << serializeVectorIntToString(megaminx->getAllEdgePiecesPosition()) << std::endl;
        std::cout << "EdgePiecesColorFlip20: " << serializeVectorIntToString(megaminx->getAllEdgePiecesColorFlipStatus()) << std::endl;
        std::cout << "CornerPiecesPosition30: " << serializeVectorIntToString(megaminx->getAllCornerPiecesPosition()) << std::endl;
        std::cout << "CornerPiecesColorFlip20: " << serializeVectorIntToString(megaminx->getAllCornerPiecesColorFlipStatus()) << std::endl;
    }
    void serializeThisOutput() const {
        std::cout << "EdgePiecesPosition30: " << serializeVectorIntToString(this->EdgePiecesPosition) << std::endl;
        std::cout << "EdgePiecesColorFlip20: " << serializeVectorIntToString(this->EdgePiecesColors) << std::endl;
        std::cout << "CornerPiecesPosition30: " << serializeVectorIntToString(this->CornerPiecesPosition) << std::endl;
        std::cout << "CornerPiecesColorFlip20: " << serializeVectorIntToString(this->CornerPiecesColors) << std::endl;
    }
    void SaveCubetoFile() const {
        serializeVectorInt5ToFile(megaminx->getAllEdgePiecesPosition(), outfiledir+EDGEFILE);
        serializeVectorInt5ToFile(megaminx->getAllEdgePiecesColorFlipStatus(), outfiledir+EDGEFILECOLORS);
        serializeVectorInt5ToFile(megaminx->getAllCornerPiecesPosition(), outfiledir+CORNERFILE);
        serializeVectorInt5ToFile(megaminx->getAllCornerPiecesColorFlipStatus(), outfiledir+CORNERFILECOLORS);
    }
    void SaveThistoFile() const {
        serializeVectorInt5ToFile(this->EdgePiecesPosition, outfiledir+EDGEFILE);
        serializeVectorInt5ToFile(this->EdgePiecesColors, outfiledir+EDGEFILECOLORS);
        serializeVectorInt5ToFile(this->CornerPiecesPosition, outfiledir+CORNERFILE);
        serializeVectorInt5ToFile(this->CornerPiecesColors, outfiledir+CORNERFILECOLORS);
    }
    void loadMegaMinx() const { //TODO: check int status
        megaminx->LoadNewEdgesFromVector(EdgePiecesPosition, EdgePiecesColors);
        megaminx->LoadNewCornersFromVector(CornerPiecesPosition, CornerPiecesColors);
    }
    void SaveCubetoFileMonolithic() {
        std::ofstream file(MEGAMINXBLOB); // E + C + E + C
        file << "EdgePiecesPosition: " << serializeVectorIntToString(megaminx->getAllEdgePiecesPosition()) << std::endl;
        file << "CornerPiecesPosition: " << serializeVectorIntToString(megaminx->getAllCornerPiecesPosition()) << std::endl;
        file << "EdgePiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllEdgePiecesColorFlipStatus()) << std::endl;
        file << "CornerPiecesColorFlip: " << serializeVectorIntToString(megaminx->getAllCornerPiecesColorFlipStatus()) << std::endl;
    }
    void SaveThistoFileMonolithic() {
        std::ofstream file(MEGAMINXBLOB); // E + C + E + C
        file << "EdgePiecesPosition: " << serializeVectorIntToString(this->EdgePiecesPosition) << std::endl;
        file << "CornerPiecesPosition: " << serializeVectorIntToString(this->CornerPiecesPosition) << std::endl;
        file << "EdgePiecesColorFlip: " << serializeVectorIntToString(this->EdgePiecesColors) << std::endl;
        file << "CornerPiecesColorFlip: " << serializeVectorIntToString(this->CornerPiecesColors) << std::endl;
    }
};
static megaminxPieceData mainData;

/**
 * @brief Save/Store Cube - (Write 4 Vector Files)
 */
void SaveCubetoFile() {
    serializeVectorInt5ToFile(megaminx->getAllEdgePiecesPosition(), EDGEFILE);
    serializeVectorInt5ToFile(megaminx->getAllCornerPiecesPosition(), CORNERFILE);
    serializeVectorInt5ToFile(megaminx->getAllEdgePiecesColorFlipStatus(), EDGEFILECOLORS);
    serializeVectorInt5ToFile(megaminx->getAllCornerPiecesColorFlipStatus(), CORNERFILECOLORS);
}


[[deprecated]]  // - old cube loader (for old tests/saves)
void RestoreOldCubeFromFile(std::string testDir) {

           //MakeShadowCubeClone();
           //TODO: Crashes w/ segfault if the files dont exist. check for them first.
    std::cout << "STATUS BEFORE: \n";
    megaminxPieceData td;
    td.readFileConstruct(testDir);
    //Read New Piece Data
    td.serializeThisOutput();
    if ((td.EdgePiecesPosition.size() != 60 && td.CornerPiecesPosition.size() != 60 ) &&
        (td.EdgePiecesColors.size() != 60 && td.CornerPiecesColors.size() != 60)) {
        std::cerr << "There was an error reading the input file - "
                     "(internal contents corrupt or size mismatch) !!!!!!" << std::endl;
        return;
    }
    //read 12 lines * 5 faces = 60 piecs
    td.loadMegaMinx();
    //Absorbed New piece Data:
    std::cout << "STATUS AFTER: \n";
    td.serializeMegaminxOutput();
    //Reoutput as file:
    //UNIT TESTS, reoutputs as another file converted type:
    td.outfiledir = "tests/6060to3020/"+td.infilepath;
    SaveCubetoFile();
}

/**
 * @brief Restore/Load Cube from Tests- (Read 4 Vector Files)
 * \param string testDir - READ FROM TESTS DIRECTORY
 */
void RestoreCubeFromTEST(std::string testDir) {
    //TODO: Crashes w/ segfault if the files dont exist. check for them.
    if (shadowDom)
        delete shadowDom;
    shadowDom = new Megaminx();
    const std::vector<int> &readEdgevector = ReadPiecesFileVector(testDir+EDGEFILE);
    const std::vector<int> &readEdgeColorvector = ReadPiecesFileVector(testDir+EDGEFILECOLORS);
    const std::vector<int> &readCornervector = ReadPiecesFileVector(testDir+CORNERFILE);
    const std::vector<int> &readCornerColorvector = ReadPiecesFileVector(testDir+CORNERFILECOLORS);
    megaminx->LoadNewEdgesFromVector(readEdgevector, readEdgeColorvector, shadowDom);
    megaminx->LoadNewCornersFromVector(readCornervector, readCornerColorvector, shadowDom);
}

/**
 * @brief Restore/Load Cube from Save - (Read 4 Vector Files)
 * \note   ("" = default current dir)
 */
void RestoreCubeFromFile() {
    RestoreCubeFromTEST("");
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
    return readvector;
}


//[[deprecated]]  - old cube loader (for old tests/saves)
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
