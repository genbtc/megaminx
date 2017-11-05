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
    double* edgeVertexList = edge[0].edgeInit();
    for (int i = 0; i < numEdges; ++i)
    {
        edge[i].init(i, edgeVertexList);
    }
    double* cornerVertexList = corner[0].cornerInit();
    for (int i = 0; i < numCorners; ++i)
    {
	    corner[i].init(i, cornerVertexList);
    }
	initFacePieces();
}

void Megaminx::initFacePieces()
{
	double* centerVertexList = face[0].faceInit();
	for (int i = 0; i < numFaces; ++i)
	{
		center[i].init(i);
		face[i].initCenter(center + i, centerVertexList);
		face[i].initAxis(i);
		face[i].initEdge(edge[0], numEdges);
		face[i].initCorner(corner[0], numCorners);
	}
}

Megaminx::Megaminx()
{
	numFaces = sizeof(face) / sizeof(Face);
	numEdges = sizeof(edge) / sizeof(Edge);
	numCorners = sizeof(corner) / sizeof(Corner);
    solve();
}

void Megaminx::render()
{
	if (!rotating)
	{
		for (int i=0; i < numFaces; ++i)
			center[i].render();
		for (int i=0; i < numEdges; ++i)
			edge[i].render();
		for (int i=0; i < numCorners; ++i)
			corner[i].render();
	}
	else
	{
		for (int i=0, k=0; i < numFaces; ++i) {			
			if (&center[i] != face[_rSide].center)
				center[i].render();
		}
		for (int i=0, k=0; i < numEdges; ++i) {
			if (&edge[i] == face[_rSide].edge[k])
				k++;
			else
				edge[i].render();
		}
		for (int i=0, k=0; i < numCorners; ++i) {
			if (&corner[i] == face[_rSide].corner[k])
				k++;
			else
				corner[i].render();
		}
		if (face[_rSide].render()) {
			rotating = false;
		}
	}
}

void Megaminx::rotate(int num, int dir)
{
	if (!rotating) {
		rotating = true;
		_rSide = num;
		face[num].rotate(dir);
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
        face[i].placeParts(r);
    }
}

void Megaminx::swapOneCorner(int i, int x)
{
	face[i].corner[x]->flip();
}

void Megaminx::swapOneEdge(int i,int x)
{    
	face[i].edge[x]->flip();
}

void Megaminx::setCurrentFace(int i)
{
	g_currentFace = &face[i];
}

//sample temp. no good.
int Megaminx::resetFace(int n)
{
	return n;
}
//Find all edge pieces:
std::vector<int> Megaminx::findEdges(int i)
{
	return face[(i - 1)].findPiece(edge[0], numEdges);
}
//Find all corner pieces:
std::vector<int> Megaminx::findCorners(int i)
{
	return face[(i - 1)].findPiece(corner[0], numCorners);
}

void Megaminx::grayEdges(int n)
{
	auto grayEdges = face[(GRAY - 1)].findPiece(edge[0], numEdges);
}
int Megaminx::grayCorners(int n)
{
	//find gray Corners - findPiece returns { a,b,c,d,e }
	auto foundGrayCorners = face[(GRAY - 1)].findPiece(corner[0], numCorners);
	//To replace the non-gray corners we would first have to find any available 
    //  slots because we may have a gray corner up there we dont want to mess up.
	//So we would want to check if we do. If we do, that makes it harder 
    //	because it may be in the wrong spot,  in which case we can switch it first. 
	//Search the gray face's corners which is [25-29] - (but how do we know that ?)
	// We can to store the numbers when we initialize them? DONE. stored in edgeNativePos and cornerNativePos
	auto defaultGrayCorners = face[(GRAY - 1)].cornerNativePos;
	//Search face[gray].corner[0-4] for anything thats not gray and get them marked for removal.
	//Search face[gray].corner[0-4] for anything thats is gray and check if its in the right spot.
	return n;
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
		if (face[i].RayTest(start, end, &pt, t, epsilon))
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