#ifndef __LOAD_H__
#define __LOAD_H__
#include <vector>
#include <string>

// load.cpp
void SaveCubetoFileMonolithic();
void SaveCubetoFile();
void RestoreCubeFromFile();
void RestoreCubeFromTEST(const std::string &testDir);
void RestoreOldCubeFromFile(const std::string &testdir);
void MakeShadowCubeClone();

void serializeVectorInt5ToFile(const std::vector<int> &vec, std::string filename);
std::string serializeVectorIntToString(const std::vector<int> &vec);
const std::vector<int> ReadPiecesFileVector(std::string filename);

#endif //__LOAD_H__
