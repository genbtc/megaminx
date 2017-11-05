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


/**
 * \brief Implement an undo cache,aka: Edit... Undo. 
 */
void Megaminx::undo()
{
	if (undoCache[1] == 0 || undoCache[0] == 0) return;
	undoCache[1] *= -1;
	rotate(undoCache[0], undoCache[1]);
}

//600 iterations.
void Megaminx::scramble()
{
	//Do 50 iterations of scrambling (like a human)
	for (int q = 0; q < 50; q++)
	{
		//12 faces - turn one each, randomizing direction
		for (int i = 0; i < numFaces; i++) {
			const int r = rand() % 2 * 2 - 1;
			faces[i].placeParts(r);
		}
	}
}

/**
 * \brief Toggle the colors of a single piece
 * \param i Nth-face's number (color) 0-11
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::swapOneCorner(int i, int x)
{
	faces[i].corner[x]->flip();
}

void Megaminx::swapOneEdge(int i,int x)
{    
	faces[i].edge[x]->flip();
}

/**
 * \brief Makes a pointer to g_currentFace
 * \param i Nth-face number index 0-11
 */
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
int Megaminx::resetFacesEdges(int color_n)
{
	const auto activeFace = faces[(color_n - 1)];
	auto foundEdges = activeFace.findPiece(edges[0], numEdges);
	auto defaultEdges = activeFace.edgeNativePos;
	for (int i = 0; i < 5; ++i)
	{
		const auto result = activeFace.edge[i]->matchesColor(color_n);
		if (!result)
		{
			const auto foundE = foundEdges[i];
			activeFace.edge[i]->swapdata(edges[foundE].data);
		}
	}
	return 1;
}

//works
int Megaminx::resetFacesCorners(int color_n)
{
	//alias activeface to face[n]
    const auto activeFace = faces[(color_n - 1)];
	//find gray Corners - findPiece returns { a,b,c,d,e }
	auto foundCorners = activeFace.findPiece(corners[0], numCorners);
//	std::vector<int> foundCorners;
//	for(int i = 0 ; i < numCorners ; ++i)
//	{
//		const bool result = corners[i].matchesColor(color_n);
//		if(result)
//			foundCorners.push_back(i);
//	}
	//To replace the non-gray corners we would first have to find any available 
    //  slots because we may have a gray corner up there we dont want to mess up.
	//So we would want to check if we do. If we do, that makes it harder 
    //	because it may be in the wrong spot,  in which case we can switch it first. 
	//EX: Search the gray face's corners which is [25-29] - (but how do we know that ?)
	//DONE: We've stored the original numbers when we initialized them.	In the 
    //  Face:attachCenter/Edge functions we store edge/cornerNativePos (as stdvec)
	auto defaultCorners = activeFace.cornerNativePos;
	//Search activeface's corner's [0-4] for anything thats not gray and swap with the first found corner
	for (int i = 0; i < 5; ++i)
	{
		const auto result = activeFace.corner[i]->matchesColor(color_n);
		if (!result)
		{
			const auto foundC = foundCorners[i];
			activeFace.corner[i]->swapdata(corners[foundC].data);
		}
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