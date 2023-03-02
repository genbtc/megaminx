#ifndef __LOAD_H__
#define __LOAD_H__
#include <vector>
#include <string>

void serializeVectorInt5(std::vector<int> vec, std::string filename);
const std::vector<int> ReadPiecesFileVector(std::string filename);

#endif