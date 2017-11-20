#include "megaminx.h"
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <algorithm>

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
    num -= 1; 	//Convert 1-12 Faces into array [0-11]
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
 * \param i Nth-face's number (color) [0-11]
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
 * \param i Nth-face's number (color) [0-11]
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
 * \param i Nth-face number index (1-12)
 */
void Megaminx::setCurrentFaceActive(int i)
{
    i -= 1;    	//Convert 1-numFaces Faces into array 0-11
    assert(i < numFaces);
    g_currentFace = &faces[i];
    assert(g_currentFace->thisNum == i);
}

//shortcut to reset all the things on a face and set it to active.
//Takes (1-12)
int Megaminx::resetFace(int n)
{
    assert(n <= numFaces);
    resetFacesEdges(n);
    resetFacesCorners(n);
    setCurrentFaceActive(n);
    return n;
}
//
/**
 * \brief Find all edge pieces.[0-11]
 */
std::vector<int> Megaminx::findEdges(int i)
{
    i -= 1;   	//Convert 1-numFaces Faces into array 0-11
    assert(i < numFaces);
    return faces[i].findPiece(edges[0], numEdges);
}

/**
 * \brief Find all corner pieces.[0-11]
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
 * \param color_n N'th Face/Color Number (1-12)
 * \return success
 */
int Megaminx::resetFacesEdges(int color_n)
{
	assert(color_n >= 1 && color_n <= 12);
    const auto activeFace = faces[(color_n - 1)];
    const auto defaultEdges = activeFace.edgeNativePos;	
	auto foundEdges = findEdges(color_n);
	assert(foundEdges.size() == 5);
	for (int j = 0; j < foundEdges.size(); ++j) 
    {	    
	    if (activeFace.edge[j]->matchesColor(color_n))
		    continue;
	    edges[foundEdges[j]].swapdata(activeFace.edge[j]->data);
	    j = -1;
    }
	auto epos = activeFace.edgeColorPos;
    auto foundEdges2 = findEdges(color_n);
	//assert check just double checking - we dont want to get stuck in while
    assert(foundEdges2 == defaultEdges);
	assert(foundEdges2.size() == 5);
	//Pieces are in the right place but maybe wrong orientation, so Swap the colors:
	for (int j = 0; j < foundEdges2.size(); ++j)
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
	assert(color_n >= 1 && color_n <= 12);
    const auto activeFace = faces[(color_n - 1)];
    const auto defaultCorners = activeFace.cornerNativePos;	
    auto foundCorners = findCorners(color_n);
	assert(foundCorners.size() == 5);
    for (int j = 0; j < foundCorners.size(); ++j) 
    {
	    if (activeFace.corner[j]->matchesColor(color_n))
		    continue;
	    corners[foundCorners[j]].swapdata(activeFace.corner[j]->data);
	    j = -1;
    }
	auto cpos = activeFace.cornerColorPos;
	auto foundCorners2 = findCorners(color_n);
    //assert check just double checking - we dont want to get stuck in while	
	assert(foundCorners2 == defaultCorners);
	assert(foundCorners2.size() == 5);
	//Pieces are in the right place but maybe wrong orientation, so Swap the colors:
	for (int j = 0; j < foundCorners2.size(); ++j)
    {
        while (activeFace.corner[j]->data._colorNum[cpos[j]] != color_n)
            activeFace.corner[j]->flip();
    }
    return 1;
}


/**
 * \brief Takes camera position angles and tells what face is most showing
 * \param x camera_angleX
 * \param y camera_angleY
 * \return face # color-int (1-12) as result.
 */
int Megaminx::getCurrentFaceFromAngles(int x, int y) const
{
    //Vars:
    const int r = 72;   	//face angles
    const int d = r / 2;  	//36 half of face 	
    const auto s = 60; 	//START_ANGLE from main.cpp
	int face = 0; 	//color-int (1-12) as result.
	//Angle Conditions:
	const auto y1  = y >= (s - d) && y <= (s + d);      			// 60
	const auto y1b = y >= (s + 240 - d) && y <= (s + 240 + d);    	//300 (other opposite)
	const auto y2  = y >= (s + 180 - d) && y <= (s + 180 + d);      //240
	const auto y2b = y >= (s +  60 - d) && y <= (s +  60 + d);      //120 (other opposite)
	const auto y3  = y >= (s + 120 - d) && y <= (s + 120 + d);      //180
	const auto y4a = y >= (0 - d)   && y <= (0 + d);       			//0
	const auto y4b = y >= (360 - d) && y <= (360 + d);				//360
	int toplist[5] = { BONE, PINK, LIGHT_GREEN, ORANGE, LIGHT_BLUE };   //{12,11,10,9,8}
	int botlist[5] = { YELLOW, PURPLE, GREEN, RED, BLUE };     			//{6,5,4,3,2}
	//Top half - Part 1:
	if(y1 && x < d + r)
		face = LIGHT_BLUE;
	//Bottom half - Part 1:	
	else if(y2 && x < d + r)
		face = BLUE;	
	for (int i = 0; i < 5; ++i)
	{
		if (y1 && x >= d + r * i && x < d + r * (i + 1))
			face = toplist[i];
		else if (y2 && x >= d + r * i && x < d + r * (i + 1))
			face = botlist[i];
	}
	if (face) return face;
	//Top half - Part 2: offset by 180 Degrees, therefore the starting point is a diff color(+2)
	//Bottom half - Part 2: offset by 180 Degrees, therefore the starting point is a diff color(+2).	
	std::rotate(std::begin(toplist), std::begin(toplist) + 2, std::end(toplist));
	std::rotate(std::begin(botlist), std::begin(botlist) + 2, std::end(botlist));
	for (int i = 0; i < 5; ++i)
	{
		if (y1b && x >= r * i && x < r * (i + 1))
			face = toplist[i];
		else if (y2b && x >= r * i && x < r * (i + 1))
			face = botlist[i];
	}
	if (face) return face;
	if (y3 && !face)	//Bottom {1}
		face = WHITE;
	else if ((y4a || y4b) && !face)	//Top {7}
		face = GRAY;
	return face;
}
