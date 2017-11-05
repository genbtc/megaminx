#include <GL/glut.h>
#include "face.h"
#include <vector>
#include <map>

Face::Face()
{
	m_pos = 0;
    m_radius = 0;
    center = nullptr;
    turnDir = 0;
    thisNum = 0;
    _rotate = false;
    angle = 0;
    axis[0] = 0;
    axis[1] = 0.0001;
    axis[2] = -1;
}

void Face::attachEdgePieces(Edge& n, int numEdges)
{
	const auto pieceList = findPiece(n, numEdges);
	edge[0] = &n + pieceList[0];
	edge[1] = &n + pieceList[1];
	edge[2] = &n + pieceList[2];
	edge[3] = &n + pieceList[3];
	edge[4] = &n + pieceList[4];
	edgeNativePos = pieceList;
}

void Face::attachCornerPieces(Corner& n, int numCorners)
{
	const auto pieceList = findPiece(n, numCorners);
	corner[0] = &n + pieceList[0];
	corner[1] = &n + pieceList[1];
	corner[2] = &n + pieceList[2];
	corner[3] = &n + pieceList[3];
	corner[4] = &n + pieceList[4];
	cornerNativePos = pieceList;
}

/**
 * \brief  This finds the color to the center/Face (since a center is perm-attached to a face)
 *   and then iterates the entire list of pieces to find when the colors match, outputs a list.
 * \pieceRef Takes a reference to the [0]th member of Pointer_array of (either Corner/Edge's) 
 * \times how many times to iterate (for simplicity).
 * \return Returns the list of 5 positions where the starting face's pieces have ended up at.
 */
std::vector<int> Face::findPiece(Piece& pieceRef, int times) const
{
	std::vector<int> pieceList;
	const auto color = center->data._colorNum[0];
	for (int i = 0; i < times; ++i)
	{
	    const bool result = (&pieceRef)[i].matchesColor(color);
		if (result)
			pieceList.push_back(i);
	}
	return pieceList;
}

void Face::attachCenter(Center *a)
{
    center = a;
}

/**
 * \brief Pre-initialize center with a re-usable list 
 * \param a The center to attach
 * \param centerVertexBase array of geometric vertexes 
 */
void Face::attachCenter(Center *a, double* centerVertexBase)
{
	center = a;
	memcpy(&_vertex, centerVertexBase, sizeof(_vertex));
}

/**
 * \brief create our Face's Axes
 * We can re-use the Center axis creation
 * \param n is the number of this face
 */
void Face::initAxis(int n)
{
	thisNum = n;
    center->createAxis(n,axis);
    for (int i = 0; i < 5; ++i)
    {
        center->createAxis(n,_vertex[i]);
    }
}

/**
 * \brief Simple-Flips (inverts) one Edge-piece 
 * and then the other, individually.
 */
void Face::twoEdgesFlip(int a,int b)
{
    edge[a]->flip();
	edge[b]->flip();
}

//Functional Generic Switch that flips 
void Face::Flip(int a, int b, int c, int d, std::vector<int> pack)
{
	//Feed in 4 ints abcd representing the face's five corner indexes 0-4 
    // (hint: [0-4]=5, but we only need to flip 4 at once)
	//Feed in these vector lists like { 0, 1, 1, 0 }; telling each index how to flip
	// Boolean ? 1 = Flip piece once ||  0      = Flip twice 
	pack[0] ? corner[a]->flip() : corner[a]->flipTwice();
	pack[1] ? corner[b]->flip() : corner[b]->flipTwice();
	pack[2] ? corner[c]->flip() : corner[c]->flipTwice();
	pack[3] ? corner[d]->flip() : corner[d]->flipTwice();
}
//Named Flip Direction lists:
std::vector<int> FlipInwards = { 0, 1, 1, 0 };
std::vector<int> FlipOutwards = { 1, 0, 0, 1 };
std::vector<int> FlipBackwards = { 0, 0, 1, 1 };
std::vector<int> FlipForwards = { 1, 1, 0, 0 };
std::vector<int> FlipAlternatingBackwards = { 0, 1, 0, 1 };
std::vector<int> FlipAlternatingForwards = { 1, 0, 1, 0 };

void Face::QuadSwapCorners(std::vector<int> pack)
{
    swapCorners(pack[0], pack[1]);
    swapCorners(pack[2], pack[3]);
    swapCorners(pack[4], pack[5]);
    swapCorners(pack[6], pack[7]);
}

void Face::QuadSwapEdges(std::vector<int> pack)
{
    swapEdges(pack[0], pack[1]);
    swapEdges(pack[2], pack[3]);
    swapEdges(pack[4], pack[5]);
    swapEdges(pack[6], pack[7]);
}
/**
 * \brief 
 * \param right Each case is for each of the 12 faces, 
 * / in order to get it to switch colors after it rotates.
 */
bool Face::placeParts(int right)
{
	if (right == 1)
	{
		switch (thisNum)
		{
		case 0:
			QuadSwapCorners({ 0, 1, 1, 2, 2, 3, 3, 4 });
			QuadSwapEdges({ 0, 1, 1, 2, 2, 3, 3, 4 });
			break;
		case 1:
			Flip(0, 1, 2, 4, FlipBackwards);
			QuadSwapCorners({ 4, 0, 4, 2, 0, 3, 0, 1 });
			QuadSwapEdges({ 4, 1, 1, 3, 0, 1, 0, 2 });
			twoEdgesFlip(1, 2);
			break;
		case 2:
			twoEdgesFlip(0, 3);
			QuadSwapEdges({ 1, 0, 1, 2, 1, 3, 3, 4 });
			Flip(0, 1, 3, 4, FlipBackwards);
			QuadSwapCorners({ 0, 1, 0, 2, 2, 3, 2, 4 });
			break;
		case 3:
			QuadSwapEdges({ 3, 2, 4, 3, 0, 1, 1, 2 });
			twoEdgesFlip(1, 2);
			QuadSwapCorners({ 3, 4, 1, 3, 1, 2, 0, 1 });
			Flip(1, 2, 3, 4, FlipAlternatingBackwards);
			break;
		case 4:
			QuadSwapEdges({ 0, 1, 1, 2, 1, 3, 3, 4 });
			twoEdgesFlip(1, 2);
			QuadSwapCorners({ 0, 1, 0, 3, 0, 4, 0, 2 });
			Flip(1, 2, 3, 4, FlipAlternatingBackwards);
			break;
		case 5:
			QuadSwapEdges({ 2, 4, 2, 3, 0, 2, 0, 1 });
			twoEdgesFlip(1, 2);
			QuadSwapCorners({ 1, 3, 1, 4, 1, 2, 0, 1 });
			Flip(1, 2, 3, 4, FlipAlternatingBackwards);
			break;
		case 6:
			QuadSwapCorners({ 0, 1, 4, 0, 3, 4, 2, 3 });
			QuadSwapEdges({ 0, 1, 4, 0, 3, 4, 2, 3 });
			break;
		case 7:
			QuadSwapEdges({ 0, 3, 0, 4, 0, 2, 0, 1 });
			twoEdgesFlip(3, 4);
			QuadSwapCorners({ 0, 4, 0, 2, 0, 1, 0, 3 });
			Flip(0, 1, 3, 4, FlipOutwards);
			break;
		case 8:
			QuadSwapEdges({ 0, 1, 1, 2, 2, 4, 3, 4 });
			twoEdgesFlip(3, 4);
			QuadSwapCorners({ 0, 4, 1, 4, 1, 2, 2, 3 });
			Flip(0, 1, 3, 4, FlipAlternatingForwards);
			break;
		case 9:
			QuadSwapEdges({ 0, 1, 1, 2, 2, 4, 3, 4 });
			twoEdgesFlip(3, 4);
			QuadSwapCorners({ 0, 4, 1, 4, 1, 2, 2, 3 });
			Flip(0, 1, 3, 4, FlipAlternatingForwards);
			break;
		case 10:
			QuadSwapEdges({ 0, 1, 1, 3, 3, 4, 2, 4 });
			twoEdgesFlip(2, 4);
			QuadSwapCorners({ 0, 4, 1, 4, 1, 2, 2, 3 });
			Flip(0, 1, 3, 4, FlipAlternatingForwards);
			break;
		case 11:
			QuadSwapEdges({ 0, 3, 0, 4, 0, 2, 0, 1 });
			twoEdgesFlip(3, 4);
			QuadSwapCorners({ 0, 3, 0, 1, 0, 2, 0, 4 });
			Flip(0, 2, 3, 4, FlipAlternatingForwards);
			break;
		default:
			break;
		}
	}
	else
	{
		           //CLOCKWISE.
	    switch(thisNum)
		{
		case 0:
			QuadSwapCorners({ 0, 1, 4, 0, 3, 4, 2, 3 });
			QuadSwapEdges({ 0, 1, 4, 0, 3, 4, 2, 3 });
			break;
		case 1:
			Flip(0, 2, 3, 4, FlipForwards);
			QuadSwapCorners({ 0, 1, 0, 3, 4, 2, 4, 0 });
			QuadSwapEdges({ 0, 2, 0, 1, 1, 3, 4, 1 });
			twoEdgesFlip(0, 3);
			break;
		case 2:
			twoEdgesFlip(1, 2);
			QuadSwapEdges({ 3, 4, 1, 3, 1, 2, 1, 0 });
			Flip(1, 2, 3, 4, FlipAlternatingForwards);
			QuadSwapCorners({ 2, 4, 2, 3, 0, 2, 0, 1 });
			break;
		case 3:
			QuadSwapEdges({ 1, 2, 0, 1, 4, 3, 3, 2 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 0, 1, 1, 2, 1, 3, 3, 4 });
			Flip(0, 1, 3, 4, FlipForwards);
			break;
		case 4:
			QuadSwapEdges({ 3, 4, 1, 3, 1, 2, 0, 1 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 0, 2, 0, 4, 0, 3, 0, 1 });
			Flip(0, 1, 3, 4, FlipForwards);
			break;
		case 5:
			QuadSwapEdges({ 0, 1, 0, 2, 2, 3, 2, 4 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 0, 1, 1, 2, 1, 4, 1, 3 });
			Flip(0, 1, 3, 4, FlipForwards);
			break;
		case 6:
			QuadSwapCorners({ 0, 1, 1, 2, 2, 3, 3, 4 });
			QuadSwapEdges({ 0, 1, 1, 2, 2, 3, 3, 4 });
			break;
		case 7: //front clockwise;
		    QuadSwapEdges({ 0, 1, 0, 2, 0, 4, 0, 3 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 0, 3, 0, 1, 0, 2, 0, 4 });
			Flip(0, 1, 2, 3, FlipInwards);
			break;
		case 8:
			QuadSwapEdges({ 3, 4, 2, 4, 1, 2, 0, 1 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 2, 3, 1, 2, 1, 4, 0, 4 });
			Flip(0, 1, 2, 4, FlipInwards);
			break;
		case 9:
			QuadSwapEdges({ 3, 4, 2, 4, 1, 2, 0, 1 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 2, 3, 1, 2, 1, 4, 0, 4 });
			Flip(0, 1, 2, 4, FlipInwards);
			break;
		case 10:
			QuadSwapEdges({ 2, 4, 3, 4, 1, 3, 0, 1 });
			twoEdgesFlip(0, 2);
			QuadSwapCorners({ 2, 3, 1, 2, 1, 4, 0, 4 });
			Flip(0, 1, 2, 4, FlipInwards);
			break;
		case 11:
			QuadSwapEdges({ 0, 1, 0, 2, 0, 4, 0, 3 });
			twoEdgesFlip(0, 3);
			QuadSwapCorners({ 0, 4, 0, 2, 0, 1, 0, 3 });
			Flip(0, 1, 2, 4, FlipInwards);
			break;
		default:
			break;
		}
	}
	return true;
}


/**
 * \brief OpenGL Display function. Calling this makes the faces rotate,the only real move.
 * \return true if we full-spun, to tell the parent function that _rotate=false and to set rotating=false also.
 */
bool Face::render()
{
	//8 is the current speed for turnDir(rotational).
    if (_rotate) angle += turnDir * 8;
	if (angle >= 56 || angle <= -56)
	{
		if (_rotate) angle -= turnDir * 2;
	}
    glPushMatrix();
    glRotated(angle, axis[0], axis[1], axis[2]);

    for (int i = 0; i < 5; ++i)
    {
        corner[i]->render();
        edge[i]->render();
        glVertex3dv(_vertex[i]);
    }
    center->render();
    glLineWidth(4);
    glColor3d(0, 0, 0);
    glPopMatrix();

    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i)
    {
        glVertex3d(_vertex[i][0] * 1.005, _vertex[i][1] * 1.005, _vertex[i][2] * 1.005);
    }
    glEnd();
    
    if (angle >= 72 || angle <= -72)
    {
        angle = 0;
        _rotate = false;
        return placeParts(turnDir);
    }
    return false;
}

/**
 * \brief Calling this sets off a chain of events in the render loops to rotate.
 * \param _turnDir turn direction - 1 for Right, -1 for left.
 */
void Face::rotate(int _turnDir)
{
    _rotate = true;
    turnDir = _turnDir;
}

/**
 * \brief given two indexes, swap the corners.
 */
void Face::swapCorners(int n, int k)
{
	corner[n]->swapdata(corner[k]->data);
}

/**
 * \brief given two indexes, swap the edges.
 */
void Face::swapEdges(int n, int k)
{
	edge[n]->swapdata(edge[k]->data);
}

/**
 * \brief This is supposed to help generate a Ray of the face of a buncha points on a plane.
 * \return Couldnt get any meaningful results because we dont have Vec3D's yet
 */
bool Face::RayTest(const Vec3d &start, const Vec3d &end, Vec3d *pt, double *t, double epsilon)
{
	*pt = ClosestPoint(start, end, m_pos, t);
	double len = Distance(*pt, m_pos);

	return len < (m_radius + epsilon);
}

bool Face::RayPlaneIntersection(Vec3d normal, Vec3d ray)
{
	/*
	float denom = normal.DotProduct(ray.direction);
	if (std::abs(denom) > 0.0001f) // your favorite epsilon
		{
			float t = (center - ray.origin).dot(normal) / denom;
			if (t >= 0) return true; // you might want to allow an epsilon here too
		}
	return false;
	*/
}