#include "megaminx.h"
#include <cstdlib>

void Megaminx::solve()
{
    y = 0; 
    x = 0;
	_rSide = 0;
    rotating = false;
	undoCache[0] = 0; undoCache[1] = 0;
	//store the value of the base start vertexes (outside the loop)
    double* edgeVertexList = edges[0].edgeInit();
    for (int i = 0; i < numEdges; ++i)
    {
        edges[i].init(i, edgeVertexList);
    }
    double* cornerVertexList = corners[0].cornerInit();
    for (int i = 0; i < numCorners; ++i)
    {
	    corners[i].init(i, cornerVertexList);
    }
	initFacePieces();
}

void Megaminx::initFacePieces()
{
	double* centerVertexList = faces[0].faceInit();
	for (int i = 0; i < numFaces; ++i)
	{
		centers[i].init(i);
		faces[i].attachCenter(centers + i, centerVertexList);
		faces[i].initAxis(i);
		faces[i].attachEdgePieces(edges[0], numEdges);
		faces[i].attachCornerPieces(corners[0], numCorners);
	}
}

Megaminx::Megaminx()
{
	numFaces = sizeof(faces) / sizeof(Face);
	numEdges = sizeof(edges) / sizeof(Edge);
	numCorners = sizeof(corners) / sizeof(Corner);
    solve();
}

void Megaminx::render()
{
	if (!rotating)
	{
		for (int i=0; i < numFaces; ++i)
			centers[i].render();
		for (int i=0; i < numEdges; ++i)
			edges[i].render();
		for (int i=0; i < numCorners; ++i)
			corners[i].render();
	}
	else
	{
		for (int i=0, k=0; i < numFaces; ++i) {			
			if (&centers[i] != faces[_rSide].center)
				centers[i].render();
		}
		for (int i=0, k=0; i < numEdges; ++i) {
			if (&edges[i] == faces[_rSide].edge[k])
				k++;
			else
				edges[i].render();
		}
		for (int i=0, k=0; i < numCorners; ++i) {
			if (&corners[i] == faces[_rSide].corner[k])
				k++;
			else
				corners[i].render();
		}
		if (faces[_rSide].render()) {
			rotating = false;
		}
	}
}

void Megaminx::rotate(int num, int dir)
{
	if (!rotating) {
		rotating = true;
		_rSide = num;
		faces[num].rotate(dir);
	}
	undoCache[0] = num; undoCache[1] = dir;
}

void Megaminx::undo()
{
	if (undoCache[1] == 0 || undoCache[0] == 0) return;
	undoCache[1] *= -1;
	rotate(undoCache[0], undoCache[1]);
}

void Megaminx::scramble()
{
    for (int i = 0; i < numFaces; i++) {
        const int r = rand() % 2 * 2 - 1;
        faces[i].placeParts(r);
    }
}

void Megaminx::swapOneCorner(int i, int x)
{
	faces[i].corner[x]->flip();
}

void Megaminx::swapOneEdge(int i,int x)
{    
	faces[i].edge[x]->flip();
}

void Megaminx::setCurrentFace(int i)
{
	g_currentFace = &faces[i];
}

//sample temp. no good.
int Megaminx::resetFace(int n)
{
	return n;
}
//Find all edge pieces:
std::vector<int> Megaminx::findEdges(int i)
{
	return faces[(i - 1)].findPiece(edges[0], numEdges);
}
//Find all corner pieces:
std::vector<int> Megaminx::findCorners(int i)
{
	return faces[(i - 1)].findPiece(corners[0], numCorners);
}

/**
 * \brief Revert all the edge pieces on the Nth colored face back to normal.
 *			To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number
 * \return success
 */
int Megaminx::grayEdges(int color_n)
{
	auto activeFace = faces[(color_n - 1)];
	auto foundGrayEdges = activeFace.findPiece(edges[0], numEdges);
	auto defaultGrayEdges = activeFace.edgeNativePos;
	std::vector<int> isGrayList, isNotGrayList;
	for (int i = 0; i < 5; ++i)
	{
		const auto result = activeFace.edge[i]->matchesColor(color_n);
		result ? isGrayList.push_back(i) : isNotGrayList.push_back(i);
	}
	for (int i = 0; i < isNotGrayList.size(); ++i)
	{
		const auto nidx = isNotGrayList[i];
		const auto cidx = foundGrayEdges[i];
		activeFace.edge[nidx]->swapdata(edges[cidx].data);
	}
	return 1;
}

int Megaminx::grayCorners(int color_n)
{
	auto activeFace = faces[(color_n - 1)];
	//find gray Corners - findPiece returns { a,b,c,d,e }
	auto foundGrayCorners = activeFace.findPiece(corners[0], numCorners);
	//To replace the non-gray corners we would first have to find any available 
    //  slots because we may have a gray corner up there we dont want to mess up.
	//So we would want to check if we do. If we do, that makes it harder 
    //	because it may be in the wrong spot,  in which case we can switch it first. 
	//Search the gray face's corners which is [25-29] - (but how do we know that ?)
	// We can to store the numbers when we initialize them? DONE. stored in edgeNativePos and cornerNativePos
	auto defaultGrayCorners = activeFace.cornerNativePos;
	//Search face[gray].corner[0-4] for anything thats not gray and get them marked for removal.
	//Search face[gray].corner[0-4] for anything thats is gray and check if its in the right spot.
	std::vector<int> isGrayList,isNotGrayList;
	for (int i = 0; i < 5; ++i)
	{
		const auto result = activeFace.corner[i]->matchesColor(color_n);
		result ? isGrayList.push_back(i) : isNotGrayList.push_back(i);
	}
	for (int i = 0; i < isNotGrayList.size(); ++i)
	{
		const auto nidx = isNotGrayList[i];
		const auto cidx = foundGrayCorners[i];
		activeFace.corner[nidx]->swapdata(corners[cidx].data);
	}
	return 1;
}
bool Megaminx::RayTest(const Vec3d& start, const Vec3d& end, unsigned* id, double* t, double epsilon)
{
	unsigned int pointID = numFaces + 1;
	bool foundCollision = false;
	double minDistToStart = 10000000.0;
	double dst;
	Vec3d pt;
	for (unsigned int i = 0; i < numFaces; ++i)
	{
		if (faces[i].RayTest(start, end, &pt, t, epsilon))
		{
			dst = Distance(start, pt);
			if (dst < minDistToStart)
			{
				minDistToStart = dst;
				pointID = i;
				foundCollision = true;
			}
		}
	}

	*id = pointID;

	return foundCollision;
}