#ifndef __LOAD_H__
#define __LOAD_H__
#include <vector>
#include <string>

// load.cpp
void SaveCubetoFileMonolithic();
void SaveCubetoFile();
void RestoreCubeFromFile();
void RestoreCubeFromTEST(std::string testDir);
void RestoreOldCubeFromFile(std::string testdir);
void MakeShadowCubeClone();

void serializeVectorInt5ToFile(std::vector<int> vec, std::string filename);
std::string serializeVectorIntToString(std::vector<int> vec);
const std::vector<int> ReadPiecesFileVector(std::string filename);

#endif //__LOAD_H__
