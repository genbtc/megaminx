#include "megaminx.h"
#include <cstdlib>
#include <algorithm>
#include <iterator>

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
 * \brief Toggle the colors of a single Corner piece
 * \param i Nth-face's number (color) 0-11
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::swapOneCorner(int i, int x)
{
	faces[i].corner[x]->flip();
}
/**
 * \brief Toggle the colors of a single Edge piece
 * \param i Nth-face's number (color) 0-11
 * \param x Nth-Corner's index 0-4
 */
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
//
/**
 * \brief Find all edge pieces.
 */
std::vector<int> Megaminx::findEdges(int i)
{
	return faces[(i - 1)].findPiece(edges[0], numEdges);
}

/**
 * \brief Find all corner pieces.
 */
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
	auto foundEdges = findEdges(color_n);
	auto defaultEdges = faces[(color_n - 1)].edgeNativePos;
	int total = 0;
//	for (int i = 0; i < foundEdges.size(); ++i)
//	{
//		bool weGoodFam = false;
//	    for (int j = 0; j < defaultEdges.size(); ++j)
//		{
//			if (defaultEdges[j] == foundEdges[i])
//				weGoodFam=true;
//		}
//		if (weGoodFam)
//			continue;
//		//it sometimes does 1 less than it needs to.
//		//it can miss one when there is already one on the board.
//		bool match = faces[(color_n - 1)].edge[i]->matchesColor(color_n);
//		if (!match)
//		{
//			total++;
//			faces[(color_n - 1)].edge[i]->swapdata(edges[foundEdges[i]].data);
//		}
//	}
	//Debugs the after state
	//foundEdges.clear();
	foundEdges = findEdges(color_n);
	for (int i = 0; i < numEdges; ++i)
	{
		for (int j = 0; j < foundEdges.size(); ++j) 
		{
			bool result = edges[foundEdges[j]].matchesColor(color_n);
			if (result)
			{
				total++;
				if (faces[(color_n - 1)].edge[j]->matchesColor(color_n))
					continue;
				edges[foundEdges[j]].swapdata(faces[(color_n - 1)].edge[j]->data);
			}
		}
	}
	foundEdges = findEdges(color_n);
	if (!(foundEdges == defaultEdges))
	{
		resetFacesEdges(color_n);
	}
	printf("%i",total);
	return 1;
}

/**
 * \brief Revert all the Corners pieces on the Nth colored face back to normal.
 *			To do so we must swap the pieces that are in there, OUT.
 * \param color_n N'th Face/Color Number
 * \return success
 */
int Megaminx::resetFacesCorners(int color_n)
{
    const auto activeFace = faces[(color_n - 1)];
	auto foundCorners = findCorners(color_n);
	sort(foundCorners.begin(), foundCorners.end());
	auto defaultCorners = activeFace.cornerNativePos;
	for (int i = 0; i < 5; ++i)
	{
		auto result = activeFace.corner[i]->matchesColor(color_n);
		if (!result)
			activeFace.corner[i]->swapdata(corners[foundCorners[i]].data);
	}
	//Debugs the after state

	for (int i = 0; i < numCorners; ++i)
	{
	bool result = corners[i].matchesColor(color_n);
	if (result)
		foundCorners.push_back(i);
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