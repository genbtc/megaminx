#include "megaminx.h"
#include <cstdlib>
#include <cassert>
#include <cmath>

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
    solve();
}

void Megaminx::iterateAllPieces()
{
	for (int i = 0; i < numFaces; ++i)
		centers[i].render();
	for (int i = 0; i < numEdges; ++i)
		edges[i].render();
	for (int i = 0; i < numCorners; ++i)
		corners[i].render();	
}

void Megaminx::render()
{
	if (!rotating)
	{
		iterateAllPieces();
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
	num -= 1; 	//Convert 1-numFaces Faces into array 0-11
	assert(num < numFaces);
    assert(dir == 1 || dir == -1);
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
		//numFaces faces - turn one each, randomizing direction
		for (int i = 0; i < numFaces; i++) {
			const int r = std::rand() % 2 * 2 - 1;
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
	assert(i < numFaces);
	assert(x < 5);
	faces[i].corner[x]->flip();
}
/**
 * \brief Toggle the colors of a single Edge piece
 * \param i Nth-face's number (color) 0-11
 * \param x Nth-Corner's index 0-4
 */
void Megaminx::swapOneEdge(int i,int x)
{    
	assert(i < numFaces);
	assert(x < 5);
	faces[i].edge[x]->flip();
}

/**
 * \brief Makes a pointer to g_currentFace
 * \param i Nth-face number index 0-11
 */
void Megaminx::setCurrentFaceActive(int i)
{
	i -= 1;    	//Convert 1-numFaces Faces into array 0-11
	assert(i < numFaces);
	g_currentFace = &faces[i];
	assert(g_currentFace->thisNum == i);
}

//shortcut to reset all the things on a face and set it to active.
//Takes 1-12
int Megaminx::resetFace(int n)
{
	assert(n < numFaces);
	resetFacesEdges(n);
	resetFacesCorners(n);
	setCurrentFaceActive(n);
	return n;
}
//
/**
 * \brief Find all edge pieces.
 */
std::vector<int> Megaminx::findEdges(int i)
{
	i -= 1;   	//Convert 1-numFaces Faces into array 0-11
	assert(i < numFaces);
	return faces[i].findPiece(edges[0], numEdges);
}

/**
 * \brief Find all corner pieces.
 */
std::vector<int> Megaminx::findCorners(int i)
{
	i -= 1;   	//Convert 1-numFaces Faces into array 0-11
	assert(i < numFaces);
	return faces[i].findPiece(corners[0], numCorners);
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
	const auto defaultEdges = activeFace.edgeNativePos;	
	auto foundEdges = findEdges(color_n);
	for (int j = 0; j < foundEdges.size(); ++j) 
	{
		if (edges[foundEdges[j]].matchesColor(color_n))
		{
			if (activeFace.edge[j]->matchesColor(color_n))
				continue;
			edges[foundEdges[j]].swapdata(activeFace.edge[j]->data);
		}
	}
	foundEdges = findEdges(color_n);
	//assert check just double checking:
	assert(foundEdges == defaultEdges);
	auto epos = activeFace.edgeColorPos;
	for (int j = 0; j < foundEdges.size(); ++j)
	{		
		while (activeFace.edge[j]->data._colorNum[epos[j]] != color_n)
			activeFace.edge[j]->flip();
	}
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
	const auto defaultCorners = activeFace.cornerNativePos;	
	auto foundCorners = findCorners(color_n);
	for (int j = 0; j < foundCorners.size(); ++j) 
	{
		if (corners[foundCorners[j]].matchesColor(color_n))
		{
			if (activeFace.corner[j]->matchesColor(color_n))
				continue;
			corners[foundCorners[j]].swapdata(activeFace.corner[j]->data);
		}
	}
	foundCorners = findCorners(color_n);
	//assert check just double checking:
    assert(foundCorners == defaultCorners);
	auto cpos = activeFace.cornerColorPos;
	for (int j = 0; j < foundCorners.size(); ++j)
	{
		while (activeFace.corner[j]->data._colorNum[cpos[j]] != color_n)
			activeFace.corner[j]->flip();
	}
	return 1;
}
